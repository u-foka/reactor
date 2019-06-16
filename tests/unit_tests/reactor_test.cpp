#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <thread>
#include <chrono>
#include <future>
#include <vector>

#include "reactor.hpp"

using namespace std::chrono_literals;
namespace sph = std::placeholders;

using testing::_;
using testing::Invoke;

struct reactor : public ::testing::Test
{
    iws::reactor *inst;

    reactor() : inst(nullptr) {}

    void SetUp()
    {
        inst = new iws::reactor();
    }

    void TearDown()
    {
        delete inst;
    }

    class mock_test_addon
    {
     public:
        MOCK_METHOD1(doit, void(std::string));
        auto doit_fun() 
        {
            return std::bind(&mock_test_addon::doit, this, sph::_1);
        }
    };

    template<typename T>
    class mock_test_addon_filter : public iws::reactor::addon_filter<T>
    {
     public:
        typedef typename iws::reactor::addon_func_map<T>::type map_type;
        MOCK_METHOD1_T(mock_func, void(map_type &));
        mock_test_addon_filter() 
            : iws::reactor::addon_filter<T>(std::bind(&mock_test_addon_filter<T>::mock_func, this, sph::_1))
        {}
    };

    class i_test
    {
    public:
        virtual ~i_test(){}
        struct test_addon {
            typedef i_test interface;
            typedef std::function<void(std::string)> func;
        };
        virtual int get_id() = 0;
    };

    template <int id, typename ...Args>
    class test : public i_test
    {
    public:
        const std::tuple<Args...> args;
        test(Args &&...args) : args(std::forward<Args>(args)...) {}

        int get_id() { return id; }

    };

    template <typename T>
    class test_contract : public iws::reactor::typed_contract<T>
    {
    public:
       const iws::reactor::index _index;

       test_contract(const std::string &instance = std::string())
          : _index(typeid(T), instance)
       {}

       virtual const iws::reactor::index &get_index() const
       {
          return _index;
       }
    };
};

TEST_F(reactor, instantiate)
{

}

TEST_F(reactor, basic)
{
    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<test<0>, test<0>, false>()));

    EXPECT_EQ(0, inst->get(test_contract<test<0>>()).get_id());
}

TEST_F(reactor, missing)
{
    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<test<1>, test<1>, false>()));

    EXPECT_THROW(inst->get(test_contract<test<-1>>()).get_id(), iws::reactor::type_not_registred_exception);
}

TEST_F(reactor, interface)
{
    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<i_test, test<2>, false>()));

    EXPECT_EQ(2, inst->get(test_contract<i_test>()).get_id());
}

TEST_F(reactor, throw_reregister)
{
    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<i_test, test<3>, false>()));
    EXPECT_THROW(inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<i_test, test<4>, false>())),
        iws::reactor::type_already_registred_exception);
}

TEST_F(reactor, priority)
{
    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<i_test, test<5>, false>()));
    inst->register_factory(std::string(), iws::reactor::prio_unittest,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<i_test, test<6>, false>()));

    EXPECT_EQ(6, inst->get(test_contract<i_test>()).get_id());
}

TEST_F(reactor, object_cache)
{
    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<i_test, test<5>, false>()));

    EXPECT_EQ(5, inst->get(test_contract<i_test>()).get_id());

    inst->register_factory(std::string(), iws::reactor::prio_unittest,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<i_test, test<6>, false>()));

    EXPECT_EQ(5, inst->get(test_contract<i_test>()).get_id());
}

TEST_F(reactor, unregister)
{
    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<i_test, test<5>, false>()));
    inst->register_factory(std::string(), iws::reactor::prio_unittest,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<i_test, test<6>, false>()));
    inst->unregister_factory(std::string(), iws::reactor::prio_unittest, typeid(i_test));

    EXPECT_EQ(5, inst->get(test_contract<i_test>()).get_id());
}

TEST_F(reactor, named_factory)
{
    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<i_test, test<7>, false>()));
    inst->register_factory("named", iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<i_test, test<8>, false>()));

    EXPECT_EQ(7, inst->get(test_contract<i_test>()).get_id());
    EXPECT_EQ(8, inst->get(test_contract<i_test>("named")).get_id());

    // Fallback to default
    EXPECT_EQ(7, inst->get(test_contract<i_test>("other")).get_id());
}

TEST_F(reactor, named_objects_differ)
{
    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<i_test, test<9>, false>()));

    EXPECT_NE(&inst->get(test_contract<i_test>()), &inst->get(test_contract<i_test>("first")));
    EXPECT_NE(&inst->get(test_contract<i_test>("first")), &inst->get(test_contract<i_test>("second")));
}

TEST_F(reactor, instance_name)
{
    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>
            (new iws::reactor::factory<test<10, const std::string &>, test<10, const std::string &>, true>()));

    EXPECT_EQ("first", std::get<0>(inst->get(test_contract<test<10, const std::string &> >("first")).args));
    EXPECT_EQ("second", std::get<0>(inst->get(test_contract<test<10, const std::string &> >("second")).args));
}

TEST_F(reactor, passed_args)
{
   typedef test<11, const std::string &> test11;
   typedef test<12, const std::string &, const std::string &> test12;

   inst->register_factory(std::string(), iws::reactor::prio_normal,
      std::unique_ptr<iws::reactor::factory_base>
         (new iws::reactor::factory<test11, test11, false, std::string>("arg1")));
   inst->register_factory(std::string(), iws::reactor::prio_normal,
      std::unique_ptr<iws::reactor::factory_base>
         (new iws::reactor::factory<test12, test12, true, std::string>("arg2")));

   EXPECT_EQ("arg1", std::get<0>(inst->get(test_contract<test11>()).args));
   EXPECT_EQ("", std::get<0>(inst->get(test_contract<test12>()).args));
   EXPECT_EQ("arg2", std::get<1>(inst->get(test_contract<test12>()).args));
   EXPECT_EQ("other", std::get<0>(inst->get(test_contract<test12>("other")).args));
   EXPECT_EQ("arg2", std::get<1>(inst->get(test_contract<test12>("other")).args));
}

TEST_F(reactor, passed_args_global)
{
   typedef test<13, const std::string &> test13;
   typedef test<14, const std::string &, const std::string &> test14;

   iws::reactor::factory_registrator<test13, test13, false, true> reg1(iws::reactor::prio_normal, std::string("arg1"));
   iws::reactor::factory_registrator<test14, test14, true, true> reg2(iws::reactor::prio_normal, std::string("arg2"));

   EXPECT_EQ("arg1", std::get<0>(iws::r.get(test_contract<test13>()).args));
   EXPECT_EQ("", std::get<0>(iws::r.get(test_contract<test14>()).args));
   EXPECT_EQ("arg2", std::get<1>(iws::r.get(test_contract<test14>()).args));
   EXPECT_EQ("other", std::get<0>(iws::r.get(test_contract<test14>("other")).args));
   EXPECT_EQ("arg2", std::get<1>(iws::r.get(test_contract<test14>("other")).args));
}

TEST_F(reactor, factory_wrapper)
{
    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(
            new iws::reactor::factory_wrapper<test<15, const std::string &, int>>([](const std::string &instance)
    {
        return std::make_shared<test<15, const std::string &, int>>(instance, 42);
    })));

    EXPECT_EQ("", std::get<0>(inst->get(test_contract<test<15, const std::string &, int> >()).args));
    EXPECT_EQ(42, std::get<1>(inst->get(test_contract<test<15, const std::string &, int> >()).args));
}

TEST_F(reactor, registrator)
{
    {
        iws::reactor::factory_registrator<i_test, test<16>, false, true> registrator(iws::reactor::prio_normal);
    }
    {
        iws::reactor::factory_registrator<i_test, test<17>, false, false> registrator(iws::reactor::prio_normal);
        EXPECT_EQ(17, iws::r.get(test_contract<i_test>()).get_id());
    }

    auto problematic = [](){
        iws::reactor::factory_registrator<i_test, test<18>, false, true> registrator(iws::reactor::prio_normal);
    };
    EXPECT_THROW(problematic(), iws::reactor::type_already_registred_exception);
}

TEST_F(reactor, concurrent_get)
{
    // Exacly the same object has to be received for each threads per run

    constexpr int rounds = 100;
    constexpr int threads = 100;

    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<test<19>, test<19>, false>()));

    auto fun = [&](){
        return &inst->get(test_contract<test<19>>());
    };

    std::vector<std::future<test<19>*>> futures;
    futures.resize(threads);

    std::map<test<19>*, int> res;

    for (int i = 0; i < rounds; ++i)
    {
        res.clear();

        for (auto it = futures.begin(); it != futures.end(); ++it)
        {
            *it = std::async(fun);
        }

        for (auto it = futures.begin(); it != futures.end(); ++it)
        {
            res[(*it).get()]++;
        }

        EXPECT_EQ(1ul, res.size());
        EXPECT_EQ(threads, res.begin()->second);

        inst->reset_objects();
    }
}

TEST_F(reactor, holder)
{
    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<test<20>, test<20>, false>()));

    auto &obj = inst->get(test_contract<test<20>>());
    auto holder = inst->get_ptr(obj);

    EXPECT_EQ(&obj, holder.get());

    auto &invalid = *reinterpret_cast<test<20>*>(42);
    EXPECT_THROW(inst->get_ptr(invalid), std::runtime_error);
}

TEST_F(reactor, contract)
{
    EXPECT_TRUE(inst->validate_contracts());
    EXPECT_EQ(0ul, inst->unsatisfied_contracts().size());

    // Simple
    iws::reactor::contract<test<23>> contract23(inst);

    EXPECT_FALSE(inst->validate_contracts());
    EXPECT_EQ(1ul, inst->unsatisfied_contracts().size());

    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<test<23>, test<23>, false>()));

    EXPECT_TRUE(inst->validate_contracts());
    EXPECT_EQ(0ul, inst->unsatisfied_contracts().size());

    // After unregister
    iws::reactor::contract<test<24>> contract24(inst);

    EXPECT_FALSE(inst->validate_contracts());
    EXPECT_EQ(1ul, inst->unsatisfied_contracts().size());

    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<test<24>, test<24>, false>()));

    EXPECT_TRUE(inst->validate_contracts());
    EXPECT_EQ(0ul, inst->unsatisfied_contracts().size());

    inst->unregister_factory(std::string(), iws::reactor::prio_normal, typeid(test<24>));

    EXPECT_FALSE(inst->validate_contracts());
    EXPECT_EQ(1ul, inst->unsatisfied_contracts().size());

    // Named
    iws::reactor::contract<test<25>> contract25(inst, "named");

    EXPECT_FALSE(inst->validate_contracts());
    EXPECT_EQ(2ul, inst->unsatisfied_contracts().size());

    inst->register_factory("other", iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<test<25>, test<25>, false>()));

    EXPECT_FALSE(inst->validate_contracts());
    EXPECT_EQ(2ul, inst->unsatisfied_contracts().size());

    inst->register_factory("named", iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<test<25>, test<25>, false>()));

    EXPECT_FALSE(inst->validate_contracts());
    EXPECT_EQ(1ul, inst->unsatisfied_contracts().size());
    
    inst->unregister_factory("named", iws::reactor::prio_normal, typeid(test<25>));

    EXPECT_FALSE(inst->validate_contracts());
    EXPECT_EQ(2ul, inst->unsatisfied_contracts().size());

    // Satisfy named by default factory
    inst->register_factory(std::string(), iws::reactor::prio_normal,
        std::unique_ptr<iws::reactor::factory_base>(new iws::reactor::factory<test<25>, test<25>, false>()));

    EXPECT_FALSE(inst->validate_contracts());
    EXPECT_EQ(1ul, inst->unsatisfied_contracts().size());
}

TEST_F(reactor, contract_global_registration)
{
    iws::reactor::contract<test<26>> contract26;

    EXPECT_FALSE(iws::r.validate_contracts());
    EXPECT_LE(1ul, iws::r.unsatisfied_contracts().size());

    iws::reactor::factory_registrator<test<26>, test<26>, false, true> registrator26(iws::reactor::prio_normal);

    auto &test26 = iws::r.get(contract26);

    EXPECT_EQ(26, test26.get_id());
}

TEST_F(reactor, addon_simple)
{
    mock_test_addon addon_mock;
    std::function<void(std::string)> addon_fun = std::bind(&mock_test_addon::doit, &addon_mock, sph::_1);

    EXPECT_CALL(addon_mock, doit("testing")).Times(1);

    inst->register_addon(std::string(), iws::reactor::prio_normal, std::make_unique<iws::reactor::addon<i_test::test_addon>>(std::move(addon_fun)));

    auto addons = inst->get_addons<i_test::test_addon>();
    for (auto& [prio, addon] : addons)
    {
        addon->addon_func("testing");
    }
}

TEST_F(reactor, addon_filter)
{
    mock_test_addon addon_mock_norm;
    EXPECT_CALL(addon_mock_norm, doit("testing")).Times(0);
    inst->register_addon(std::string(), iws::reactor::prio_normal, std::make_unique<iws::reactor::addon<i_test::test_addon>>(addon_mock_norm.doit_fun()));
    
    mock_test_addon addon_mock_ut;
    EXPECT_CALL(addon_mock_ut, doit("testing")).Times(1);
    inst->register_addon(std::string(), iws::reactor::prio_unittest, std::make_unique<iws::reactor::addon<i_test::test_addon>>(addon_mock_ut.doit_fun()));
    
    mock_test_addon_filter<i_test::test_addon> addon_filter_mock;
    typedef typename iws::reactor::addon_func_map<i_test::test_addon>::type addon_map_type;
    EXPECT_CALL(addon_filter_mock, mock_func(_)).WillOnce(Invoke([](addon_map_type &addons){
        iws::reactor_utils::erase_if(addons, [](auto &item) {
            return item.first != iws::reactor::prio_unittest;
        });
    }));
    inst->register_addon_filter(std::string(), iws::reactor::prio_unittest, std::make_unique<iws::reactor::addon_filter<i_test::test_addon>>(addon_filter_mock));
    
    auto addons = inst->get_addons<i_test::test_addon>();
    for (auto& [prio, addon] : addons)
    {
        addon->addon_func("testing");
    }
}
