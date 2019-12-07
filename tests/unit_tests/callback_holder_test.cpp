#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>
#include <string>

#include <reactor/callback_holder.hpp>

namespace ph = std::placeholders;

struct callback_holder : public ::testing::Test
{
   class i_callback_test
   {
    public:
      virtual ~i_callback_test() {}

      virtual void simple() = 0;
      virtual void simple2() = 0;
      virtual void simple3() = 0;
      virtual void simple_arg(int arg) = 0;
      virtual void complex_arg(const std::string &arg) = 0;
   };

   class mock_callback_test : public i_callback_test
   {
    public:
      MOCK_METHOD0(simple, void());
      MOCK_METHOD0(simple2, void());
      MOCK_METHOD0(simple3, void());
      MOCK_METHOD1(simple_arg, void(int));
      MOCK_METHOD1(complex_arg, void(const std::string &));
   };

   mock_callback_test mock;

   std::function<void()> simple_cb;
   std::function<void()> simple2_cb;
   std::function<void()> simple3_cb;
   std::function<void(int)> simple_arg_cb;
   std::function<void(const std::string &arg)> complex_arg_cb;

   callback_holder()
         : simple_cb(std::bind(&i_callback_test::simple, &mock))
         , simple2_cb(std::bind(&i_callback_test::simple2, &mock))
         , simple3_cb(std::bind(&i_callback_test::simple3, &mock))
         , simple_arg_cb(std::bind(&i_callback_test::simple_arg, &mock, ph::_1))
         , complex_arg_cb(std::bind(&i_callback_test::complex_arg, &mock, ph::_1))
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
