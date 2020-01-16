#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>
#include <string>

#include <reactor/callback_holder.hpp>

namespace ph = std::placeholders;
using ::testing::_;
using ::testing::Invoke;

struct callback_holder : public ::testing::Test
{
   struct mock_callback_test
   {
      MOCK_METHOD0(simple, void());
      MOCK_METHOD0(simple2, void());
      MOCK_METHOD0(simple3, void());
      MOCK_METHOD1(simple_arg, void(int));
      MOCK_METHOD1(complex_arg, void(const std::string &));
      MOCK_METHOD1(moved_arg, void(std::unique_ptr<std::string> &&));
      MOCK_METHOD3(multiple_args, void(int, double, std::string));
   };

   mock_callback_test mock;

   std::function<void()> simple_cb;
   std::function<void()> simple2_cb;
   std::function<void()> simple3_cb;
   std::function<void(int)> simple_arg_cb;
   std::function<void(const std::string &arg)> complex_arg_cb;
   std::function<void(std::unique_ptr<std::string> &&)> moved_arg_cb;
   std::function<void(int, double, std::string)> multiple_args_cb;

   callback_holder()
         : simple_cb(std::bind(&mock_callback_test::simple, &mock))
         , simple2_cb(std::bind(&mock_callback_test::simple2, &mock))
         , simple3_cb(std::bind(&mock_callback_test::simple3, &mock))
         , simple_arg_cb(std::bind(&mock_callback_test::simple_arg, &mock, ph::_1))
         , complex_arg_cb(std::bind(&mock_callback_test::complex_arg, &mock, ph::_1))
         , moved_arg_cb(std::bind(&mock_callback_test::moved_arg, &mock, ph::_1))
         , multiple_args_cb(std::bind(&mock_callback_test::multiple_args, &mock, ph::_1, ph::_2, ph::_3))
   {
   }
};

TEST_F(callback_holder, simple)
{
   EXPECT_CALL(mock, simple()).Times(1);

   iws::callback_holder<> cb;

   // empty call
   cb();

   ASSERT_LE(0ul, cb.connect(simple_cb));

   cb();
}

TEST_F(callback_holder, complex)
{
   EXPECT_CALL(mock, simple_arg(42)).Times(2);
   EXPECT_CALL(mock, complex_arg("forty two")).Times(2);

   iws::callback_holder<int> cb1;
   iws::callback_holder<const std::string &> cb2;
   auto cb1_id = cb1.connect(simple_arg_cb);
   ASSERT_LE(0ul, cb1_id);
   ASSERT_LE(0ul, cb2.connect(complex_arg_cb));
   ASSERT_LE(0ul, cb2.connect(complex_arg_cb));

   cb1(42);
   cb2("forty two"); // 2*

   cb2.clear();

   cb1(42);
   cb2("forty two"); // 0*

   ASSERT_TRUE(cb1.disconnect(cb1_id));

   cb1(42);          // 0*
   cb2("forty two"); // 0*
}

TEST_F(callback_holder, count)
{
   EXPECT_CALL(mock, simple()).Times(4);
   EXPECT_CALL(mock, simple2()).Times(1);

   iws::callback_holder<> cb;

   EXPECT_FALSE((bool)cb);
   EXPECT_EQ(0ul, cb.callback_count());

   ASSERT_LE(0ul, cb.connect(simple_cb));

   EXPECT_TRUE((bool)cb);
   EXPECT_EQ(1ul, cb.callback_count());

   auto cb_id = cb.connect(simple2_cb);
   ASSERT_LE(0ul, cb_id);

   EXPECT_TRUE((bool)cb);
   EXPECT_EQ(2ul, cb.callback_count());

   ASSERT_LE(0ul, cb.connect(simple_cb));

   EXPECT_TRUE((bool)cb);
   EXPECT_EQ(3ul, cb.callback_count());

   cb();
   ASSERT_TRUE(cb.disconnect(cb_id));
   cb();

   EXPECT_TRUE((bool)cb);
   EXPECT_EQ(2ul, cb.callback_count());

   cb.clear();

   EXPECT_FALSE((bool)cb);
   EXPECT_EQ(0ul, cb.callback_count());
}

TEST_F(callback_holder, disconnect)
{
   EXPECT_CALL(mock, simple()).Times(1);
   EXPECT_CALL(mock, simple2()).Times(2);
   EXPECT_CALL(mock, simple3()).Times(1);

   iws::callback_holder<> cb;

   ASSERT_FALSE(cb.disconnect(42)); // Disconnect invalid

   auto cb_id1 = cb.connect(simple_cb);
   cb.connect(simple2_cb);
   auto cb_id3 = cb.connect(simple3_cb);

   cb();

   ASSERT_TRUE(cb.disconnect(cb_id1));
   ASSERT_FALSE(cb.disconnect(cb_id1)); // Double disconnect attempt
   ASSERT_TRUE(cb.disconnect(cb_id3));

   cb();
}

TEST_F(callback_holder, forward_correct)
{
   const std::string txt("here I am");
   std::unique_ptr<std::string> to_be_moved(new std::string(txt));
   std::string callback_arg;
   EXPECT_CALL(mock, moved_arg(_)).WillRepeatedly(Invoke([&](std::unique_ptr<std::string> &&arg){
      std::unique_ptr<std::string> moved(std::move(arg));
      callback_arg = *moved;
   }));
   
   iws::callback_holder<std::unique_ptr<std::string> &&> cb;
   cb.connect(moved_arg_cb);

   cb(std::move(to_be_moved));
   EXPECT_EQ(to_be_moved.get(), nullptr); // Should be empty as it was moved into the callback
   EXPECT_EQ(callback_arg, txt);

   to_be_moved.reset(new std::string(txt)); // Re-initialize the pointer
   cb.connect(moved_arg_cb); // Connect the callback second time

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
   EXPECT_EXIT((cb(std::move(to_be_moved)),exit(0)),::testing::KilledBySignal(SIGSEGV),".*");
#else
   EXPECT_DEATH_IF_SUPPORTED((cb(std::move(to_be_moved)),exit(0)),".*");
#endif
}

TEST_F(callback_holder, multiple_args)
{
   // TODO
}
