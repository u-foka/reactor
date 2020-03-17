#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <thread>
#include <vector>

#include <reactor/contract.hpp>
#include <reactor/factory.hpp>
#include <reactor/factory_registrator.hpp>
#include <reactor/factory_wrapper.hpp>
#include <reactor/factory_wrapper_registrator.hpp>
#include <reactor/lazy_pulley.hpp>
#include <reactor/make_unique_polyfil.hpp>
#include <reactor/pulley.hpp>
#include <reactor/r.hpp>
#include <reactor/reactor.hpp>

#include "i_ext_test.hpp"

namespace sph = std::placeholders;

using testing::_;
using testing::Invoke;

namespace re = iws::reactor;
namespace pf = iws::polyfil;

struct reactor : public ::testing::Test
{
   re::reactor *inst;

   reactor()
         : inst(nullptr)
   {
   }

   void SetUp()
   {
      re::r.reset_objects();
      inst = new re::reactor();
   }

   void TearDown() { delete inst; }

   class mock_test_addon
   {
    public:
      MOCK_METHOD1(doit, void(std::string));
      std::function<void(std::string)> doit_fun() { return std::bind(&mock_test_addon::doit, this, sph::_1); }
   };

   template<typename T>
   class mock_test_addon_filter : public re::addon_filter<T>
   {
    public:
      typedef typename re::addon_func_map<T>::type map_type;
      MOCK_METHOD1_T(mock_func, void(map_type &));
      mock_test_addon_filter()
            : re::addon_filter<T>(std::bind(&mock_test_addon_filter<T>::mock_func, this, sph::_1))
      {
      }
   };

   class i_test
   {
    public:
      virtual ~i_test() {}
      struct test_addon
      {
         typedef i_test interface;
         typedef std::function<void(std::string)> func;
      };
      virtual int get_id() = 0;
   };

   template<int id, typename... Args>
   class test : public i_test
   {
    public:
      const std::tuple<Args...> args;
      test(Args &&... args)
            : args(std::forward<Args>(args)...)
      {
      }

      int get_id() { return id; }
   };

   template<typename T>
   class test_contract : public re::typed_contract<T>
   {
    public:
      const re::index _index;

      test_contract(const std::string &instance = std::string())
            : re::typed_contract<T>(nullptr)
            , _index(typeid(T), instance)
      {
      }

      virtual const re::index &get_index() const override { return _index; }
      virtual void try_get() override { throw std::logic_error("Not implemented"); }
   };

   template<typename T>
   class mock_contract : public re::typed_contract<T>
   {
    public:
      const re::index _index;

      mock_contract(re::reactor *r_inst, const std::string &instance = std::string())
            : re::typed_contract<T>(r_inst)
            , _index(typeid(T), instance)
      {
      }

      virtual const re::index &get_index() const override { return _index; }

      MOCK_METHOD0(try_get, void());
   };

   class shutdown_checker
   {
    public:
      iws::callback_holder<> sig_dtor;

      virtual ~shutdown_checker() { sig_dtor(); }
   };

   template<typename T>
   struct dependant
   {
      T &_dependency;
      dependant()
            : _dependency(re::r.get(test_contract<T>()))
      {
      }
   };

   struct dependency
   {
   };

   struct recursive_dependency
   {
      typedef dependant<recursive_dependency> recursive;
      recursive &_dependency;
      recursive_dependency()
            : _dependency(re::r.get(test_contract<recursive>()))
      {
      }
   };
};

TEST_F(reactor, instantiate) {}

TEST_F(reactor, basic)
{
   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<test<0>, test<0>, false>>());

   EXPECT_EQ(0, inst->get(test_contract<test<0>>()).get_id());
}

TEST_F(reactor, missing)
{
   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<test<1>, test<1>, false>>());

   EXPECT_THROW(inst->get(test_contract<test<-1>>()).get_id(), re::factory_not_registred_exception);
}

TEST_F(reactor, interface)
{
   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<i_test, test<2>, false>>());

   EXPECT_EQ(2, inst->get(test_contract<i_test>()).get_id());
}

TEST_F(reactor, throw_reregister)
{
   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<i_test, test<3>, false>>());
   EXPECT_THROW(inst->register_factory(std::string(), re::prio_normal,
                      std::shared_ptr<re::factory_base>(new re::factory<i_test, test<4>, false>())),
         re::type_already_registred_exception);
}

TEST_F(reactor, priority)
{
   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<i_test, test<5>, false>>());
   inst->register_factory(std::string(), re::prio_test, std::make_shared<re::factory<i_test, test<6>, false>>());

   EXPECT_EQ(6, inst->get(test_contract<i_test>()).get_id());
}

TEST_F(reactor, object_cache)
{
   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<i_test, test<5>, false>>());

   EXPECT_EQ(5, inst->get(test_contract<i_test>()).get_id());

   inst->register_factory(std::string(), re::prio_test, std::make_shared<re::factory<i_test, test<6>, false>>());

   EXPECT_EQ(5, inst->get(test_contract<i_test>()).get_id());
}

TEST_F(reactor, unregister)
{
   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<i_test, test<5>, false>>());
   inst->register_factory(std::string(), re::prio_test, std::make_shared<re::factory<i_test, test<6>, false>>());

   EXPECT_THROW(inst->unregister_factory(std::string(), re::prio_override, typeid(i_test)),
         re::factory_not_registred_exception);
   EXPECT_THROW(
         inst->unregister_factory(std::string(), re::prio_test, typeid(test<6>)), re::factory_not_registred_exception);
   EXPECT_THROW(
         inst->unregister_factory("no_inst", re::prio_test, typeid(i_test)), re::factory_not_registred_exception);

   EXPECT_EQ(6, inst->get(test_contract<i_test>()).get_id());

   inst->unregister_factory(std::string(), re::prio_test, typeid(i_test));
   inst->reset_objects(); // So i_test will be recreated with the effective factory on the next get()

   EXPECT_EQ(5, inst->get(test_contract<i_test>()).get_id());
}

TEST_F(reactor, named_factory)
{
   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<i_test, test<7>, false>>());
   inst->register_factory("named", re::prio_normal, std::make_shared<re::factory<i_test, test<8>, false>>());

   EXPECT_EQ(7, inst->get(test_contract<i_test>()).get_id());
   EXPECT_EQ(8, inst->get(test_contract<i_test>("named")).get_id());

   // Fallback to default
   EXPECT_EQ(7, inst->get(test_contract<i_test>("other")).get_id());
}

TEST_F(reactor, named_objects_differ)
{
   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<i_test, test<9>, false>>());

   EXPECT_NE(&inst->get(test_contract<i_test>()), &inst->get(test_contract<i_test>("first")));
   EXPECT_NE(&inst->get(test_contract<i_test>("first")), &inst->get(test_contract<i_test>("second")));
}

TEST_F(reactor, instance_name)
{
   inst->register_factory(std::string(), re::prio_normal,
         std::shared_ptr<re::factory_base>(
               new re::factory<test<10, const std::string &>, test<10, const std::string &>, true>()));

   EXPECT_EQ("first", std::get<0>(inst->get(test_contract<test<10, const std::string &>>("first")).args));
   EXPECT_EQ("second", std::get<0>(inst->get(test_contract<test<10, const std::string &>>("second")).args));
}

TEST_F(reactor, passed_args)
{
   typedef test<11, const std::string &> test11;
   typedef test<12, const std::string &, const std::string &> test12;

   inst->register_factory(
         std::string(), re::prio_normal, std::make_shared<re::factory<test11, test11, false, std::string>>("arg1"));
   inst->register_factory(
         std::string(), re::prio_normal, std::make_shared<re::factory<test12, test12, true, std::string>>("arg2"));

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

   re::factory_registrator<test13, test13, false, true> reg1(re::prio_normal, std::string("arg1"));
   re::factory_registrator<test14, test14, true, true> reg2(re::prio_normal, std::string("arg2"));

   EXPECT_EQ("arg1", std::get<0>(re::r.get(test_contract<test13>()).args));
   EXPECT_EQ("", std::get<0>(re::r.get(test_contract<test14>()).args));
   EXPECT_EQ("arg2", std::get<1>(re::r.get(test_contract<test14>()).args));
   EXPECT_EQ("other", std::get<0>(re::r.get(test_contract<test14>("other")).args));
   EXPECT_EQ("arg2", std::get<1>(re::r.get(test_contract<test14>("other")).args));
}

TEST_F(reactor, factory_wrapper)
{
   inst->register_factory(std::string(), re::prio_normal,
         std::shared_ptr<re::factory_base>(
               new re::factory_wrapper<test<15, const std::string &, int>>([](const std::string &instance) {
                  return std::make_shared<test<15, const std::string &, int>>(instance, 42);
               })));

   EXPECT_EQ("", std::get<0>(inst->get(test_contract<test<15, const std::string &, int>>()).args));
   EXPECT_EQ(42, std::get<1>(inst->get(test_contract<test<15, const std::string &, int>>()).args));
}

TEST_F(reactor, registrator)
{
   try
   {
      {
         re::factory_registrator<i_test, test<16>, false, true> registrator(re::prio_normal);
         EXPECT_EQ(16, re::r.get(test_contract<i_test>()).get_id());
      }
      re::r.reset_objects();
      {
         re::factory_registrator<i_test, test<17>, false, false> registrator(re::prio_normal);
         EXPECT_EQ(17, re::r.get(test_contract<i_test>()).get_id());
      }

      auto problematic = []() { re::factory_registrator<i_test, test<18>, false, true> registrator(re::prio_normal); };
      EXPECT_THROW(problematic(), re::type_already_registred_exception);

      re::r.unregister_factory(std::string(), re::prio_normal, typeid(i_test));
   }
   catch (...)
   {
      re::r.unregister_factory(std::string(), re::prio_normal, typeid(i_test));
   }
}

TEST_F(reactor, concurrent_get)
{
   // Exacly the same object has to be received for each threads per run

   constexpr int rounds = 1000;
   constexpr int threads = 10;

   test_contract<test<19>> ct;

   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<test<19>, test<19>, false>>());

   auto fun = [&]() { return &inst->get(ct); };

   std::vector<std::future<test<19> *>> futures;
   futures.resize(threads);

   std::map<test<19> *, int> res;

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
   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<test<20>, test<20>, false>>());

   auto &obj = inst->get(test_contract<test<20>>());
   auto holder = inst->get_ptr(obj);

   EXPECT_EQ(&obj, holder.get());

   auto &invalid = *reinterpret_cast<test<20> *>(42);
   EXPECT_THROW(inst->get_ptr(invalid), std::runtime_error);
}

TEST_F(reactor, contract)
{
   EXPECT_TRUE(inst->validate_contracts());
   EXPECT_EQ(0ul, inst->unsatisfied_contracts().size());

   // Simple
   re::contract<test<23>> contract23(inst);

   EXPECT_FALSE(inst->validate_contracts());
   EXPECT_EQ(1ul, inst->unsatisfied_contracts().size());

   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<test<23>, test<23>, false>>());

   EXPECT_TRUE(inst->validate_contracts());
   EXPECT_EQ(0ul, inst->unsatisfied_contracts().size());

   // After unregister
   re::contract<test<24>> contract24(inst);

   EXPECT_FALSE(inst->validate_contracts());
   EXPECT_EQ(1ul, inst->unsatisfied_contracts().size());

   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<test<24>, test<24>, false>>());

   EXPECT_TRUE(inst->validate_contracts());
   EXPECT_EQ(0ul, inst->unsatisfied_contracts().size());

   inst->unregister_factory(std::string(), re::prio_normal, typeid(test<24>));

   EXPECT_FALSE(inst->validate_contracts());
   EXPECT_EQ(1ul, inst->unsatisfied_contracts().size());

   // Named
   re::contract<test<25>> contract25(inst, "named");

   EXPECT_FALSE(inst->validate_contracts());
   EXPECT_EQ(2ul, inst->unsatisfied_contracts().size());

   inst->register_factory("other", re::prio_normal, std::make_shared<re::factory<test<25>, test<25>, false>>());

   EXPECT_FALSE(inst->validate_contracts());
   EXPECT_EQ(2ul, inst->unsatisfied_contracts().size());

   inst->register_factory("named", re::prio_normal, std::make_shared<re::factory<test<25>, test<25>, false>>());

   EXPECT_FALSE(inst->validate_contracts());
   EXPECT_EQ(1ul, inst->unsatisfied_contracts().size());

   inst->unregister_factory("named", re::prio_normal, typeid(test<25>));

   EXPECT_FALSE(inst->validate_contracts());
   EXPECT_EQ(2ul, inst->unsatisfied_contracts().size());

   // Satisfy named by default factory
   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<test<25>, test<25>, false>>());

   EXPECT_FALSE(inst->validate_contracts());
   EXPECT_EQ(1ul, inst->unsatisfied_contracts().size());
}

TEST_F(reactor, contract_global_registration)
{
   re::contract<test<26>> contract26;

   EXPECT_FALSE(re::r.validate_contracts());
   EXPECT_LE(1ul, re::r.unsatisfied_contracts().size());

   re::factory_registrator<test<26>, test<26>, false, true> registrator26(re::prio_normal);

   auto &test26 = re::r.get(contract26);

   EXPECT_EQ(26, test26.get_id());
}

TEST_F(reactor, addon_simple)
{
   mock_test_addon addon_mock;
   std::function<void(std::string)> addon_fun = std::bind(&mock_test_addon::doit, &addon_mock, sph::_1);

   EXPECT_CALL(addon_mock, doit("testing")).Times(1);

   inst->register_addon(
         std::string(), re::prio_normal, pf::make_unique<re::addon<i_test::test_addon>>(std::move(addon_fun)));

   auto addons = inst->get_addons<i_test::test_addon>();
   EXPECT_EQ(addons.size(), 1ul);
   for (auto &item : addons)
   {
      EXPECT_EQ(std::type_index(item.second->get_type()), std::type_index(typeid(i_test::test_addon)));
      EXPECT_EQ(std::type_index(item.second->get_interface_type()), std::type_index(typeid(i_test)));

      item.second->addon_func("testing");
   }

   EXPECT_THROW(inst->unregister_addon(std::string(), re::prio_override, typeid(i_test::test_addon)),
         re::addon_not_registred_exception);
   EXPECT_THROW(inst->unregister_factory(std::string(), re::prio_normal, typeid(std::string)),
         re::factory_not_registred_exception);
   EXPECT_THROW(inst->unregister_factory("no_inst", re::prio_normal, typeid(i_test::test_addon)),
         re::factory_not_registred_exception);

   EXPECT_EQ(inst->get_addons<i_test::test_addon>().size(), 1ul);

   inst->unregister_addon(std::string(), re::prio_normal, typeid(i_test::test_addon));

   EXPECT_EQ(inst->get_addons<i_test::test_addon>().size(), 0ul);
}

TEST_F(reactor, addon_filter)
{
   mock_test_addon addon_mock_norm;
   EXPECT_CALL(addon_mock_norm, doit("testing")).Times(0);
   inst->register_addon(
         std::string(), re::prio_normal, pf::make_unique<re::addon<i_test::test_addon>>(addon_mock_norm.doit_fun()));

   mock_test_addon addon_mock_ut;
   EXPECT_CALL(addon_mock_ut, doit("testing")).Times(1);
   inst->register_addon(
         std::string(), re::prio_test, pf::make_unique<re::addon<i_test::test_addon>>(addon_mock_ut.doit_fun()));

   mock_test_addon_filter<i_test::test_addon> addon_filter_mock;
   typedef typename re::addon_func_map<i_test::test_addon>::type addon_map_type;
   EXPECT_CALL(addon_filter_mock, mock_func(_)).WillRepeatedly(Invoke([](addon_map_type &addons) {
      re::detail::erase_if(addons, [](addon_map_type::value_type &item) { return item.first != re::prio_test; });
   }));

   EXPECT_EQ(inst->get_addons<i_test::test_addon>().size(), 2ul);

   auto filter = pf::make_unique<re::addon_filter<i_test::test_addon>>(addon_filter_mock);
   EXPECT_EQ(std::type_index(filter->get_type()), std::type_index(typeid(i_test::test_addon)));
   EXPECT_EQ(std::type_index(filter->get_interface_type()), std::type_index(typeid(i_test)));

   inst->register_addon_filter(std::string(), re::prio_test, std::move(filter));

   auto addons = inst->get_addons<i_test::test_addon>();
   EXPECT_EQ(addons.size(), 1ul);
   for (auto &item : addons)
   {
      item.second->addon_func("testing");
   }

   EXPECT_THROW(inst->unregister_addon_filter(std::string(), re::prio_override, typeid(i_test::test_addon)),
         re::addon_filter_not_registred_exception);
   EXPECT_THROW(inst->unregister_addon_filter(std::string(), re::prio_test, typeid(std::string)),
         re::addon_filter_not_registred_exception);
   EXPECT_THROW(inst->unregister_addon_filter("no_inst", re::prio_test, typeid(i_test::test_addon)),
         re::addon_filter_not_registred_exception);

   EXPECT_EQ(inst->get_addons<i_test::test_addon>().size(), 1ul);

   inst->unregister_addon_filter(std::string(), re::prio_test, typeid(i_test::test_addon));

   EXPECT_EQ(inst->get_addons<i_test::test_addon>().size(), 2ul);
}

TEST_F(reactor, factory_result)
{
   auto val = std::make_shared<test<27>>();
   re::factory_result obj(val);

   EXPECT_EQ(obj.get<test<27>>().get(), val.get());
   EXPECT_THROW(obj.get<test<0>>(), std::logic_error);
}

TEST_F(reactor, factory_not_registred_exception_what)
{
   const std::string what("WHAT?!");
   const re::factory_not_registred_exception ex(typeid(reactor), what);

   EXPECT_GE(strlen(ex.what()), 0ul);
   EXPECT_GE(strlen(ex.what()), 0ul); // Also check cached
}

TEST_F(reactor, type_already_registred_exception_what)
{
   const std::string what("WHAT?!");
   const re::type_already_registred_exception ex(typeid(reactor), what, re::prio_normal);

   EXPECT_GE(strlen(ex.what()), 0ul);
   EXPECT_GE(strlen(ex.what()), 0ul); // Also check cached
}

TEST_F(reactor, is_shutting_down)
{
   EXPECT_EQ(inst->is_shutting_down(), false);

   inst->register_factory(
         std::string(), re::prio_normal, std::make_shared<re::factory<shutdown_checker, shutdown_checker, false>>());

   inst->get(test_contract<shutdown_checker>()).sig_dtor.connect([&] { EXPECT_EQ(inst->is_shutting_down(), true); });

   delete inst;
   inst = nullptr;
}

TEST_F(reactor, get_version)
{
   ASSERT_NE(inst->get_version(), std::string());
   std::cout << "Reactor version: " << inst->get_version() << std::endl;
}

TEST_F(reactor, recursion)
{
   re::factory_registrator<dependency, dependency, false, true> reg1(re::prio_normal);
   re::factory_registrator<dependant<dependency>, dependant<dependency>, false, true> reg2(re::prio_normal);
   re::factory_registrator<dependant<recursive_dependency>, dependant<recursive_dependency>, false, true> reg3(
         re::prio_normal);
   re::factory_registrator<recursive_dependency, recursive_dependency, false, true> reg4(re::prio_normal);

   EXPECT_NO_THROW(re::r.get(test_contract<dependant<dependency>>()));
   EXPECT_THROW(re::r.get(test_contract<recursive_dependency>()), std::runtime_error);
}

TEST_F(reactor, test_all_contracts)
{
   mock_contract<test<21>> ctr1(inst);
   mock_contract<test<22>> ctr2(inst);

   EXPECT_CALL(ctr1, try_get()).Times(1);
   EXPECT_CALL(ctr2, try_get()).Times(1);

   inst->test_all_contracts();
}

TEST_F(reactor, contract_try_get)
{
   bool done = false;

   const re::factory_wrapper_registrator<test<27>, true> reg(re::prio_normal, [&done](const std::string &) {
      done = true;
      return std::make_shared<test<27>>();
   });

   re::contract<test<27>> ctr;

   ctr.try_get();

   EXPECT_EQ(done, true);
}

TEST_F(reactor, pulley)
{
   re::factory_registrator<i_test, test<28>, false, true> reg28(re::prio_normal);
   re::factory_registrator<i_test, test<29>, false, true> reg29("named", re::prio_normal);

   re::pulley<i_test> p28;
   // re::pulley<i_test, "named"> p29;
   const re::pulley<i_test> p28c;

   EXPECT_TRUE(re::r.instance_exists(p28._contract));

   EXPECT_EQ(28, p28->get_id());
   // EXPECT_EQ(29, p29->get_id());
   EXPECT_EQ(28, p28c->get_id());
}

TEST_F(reactor, lazy_pulley)
{
   re::factory_registrator<i_test, test<28>, false, true> reg28(re::prio_normal);

   re::lazy_pulley<i_test> p28;
   const re::lazy_pulley<i_test> p28c;

   EXPECT_FALSE(re::r.instance_exists(p28._contract));

   EXPECT_EQ(28, p28->get_id());

   EXPECT_TRUE(re::r.instance_exists(p28._contract));

   EXPECT_EQ(28, p28c->get_id());
}

TEST_F(reactor, lazy_pulley_concurrency)
{
   constexpr int rounds = 1000;
   constexpr int threads = 10;

   re::factory_registrator<i_test, test<28>, false, true> reg28(re::prio_normal);

   std::vector<std::future<void *>> futures;
   futures.resize(threads);

   std::vector<void *> res;

   for (int i = 0; i < rounds; ++i)
   {
      inst->reset_objects();
      re::lazy_pulley<i_test> p28;

      res.clear();
      res.reserve(threads);

      for (auto it = futures.begin(); it != futures.end(); ++it)
      {
         *it = std::async([&]() {
            EXPECT_EQ(28, p28->get_id());
            return (void *)p28.operator->();
         });
      }

      for (auto it = futures.begin(); it != futures.end(); ++it)
      {
         res.push_back((*it).get());
      }

      const auto firstval = res.front();
      for (auto val : res)
      {
         EXPECT_EQ(firstval, val);
      }
   }
}

TEST_F(reactor, instance_exists)
{
   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<test<30>, test<30>, false>>());
   inst->register_factory(std::string(), re::prio_normal, std::make_shared<re::factory<test<31>, test<31>, false>>());

   test_contract<test<30>> ctr30;
   test_contract<test<31>> ctr31;

   EXPECT_FALSE(inst->instance_exists(ctr30));
   EXPECT_FALSE(inst->instance_exists(ctr31));

   EXPECT_EQ(30, inst->get(ctr30).get_id());
   EXPECT_TRUE(inst->instance_exists(ctr30));
   EXPECT_FALSE(inst->instance_exists(ctr31));

   EXPECT_EQ(31, inst->get(ctr31).get_id());
   EXPECT_TRUE(inst->instance_exists(ctr30));
   EXPECT_TRUE(inst->instance_exists(ctr31));
}

TEST_F(reactor, ext_impl)
{
   re::contract<iws::reactor_test::i_ext_test> ct;

   EXPECT_TRUE(re::r.get(ct).are_you_ext());
}
