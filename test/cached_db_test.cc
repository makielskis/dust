#include <iostream>
#include <chrono>

#include "gtest/gtest.h"

#include "boost/filesystem.hpp"

#include "../src/storage/cached_db.h"

#define DB_PATH "test_db"

using namespace dust;

class cached_db_test : public testing::Test {
 public:
  cached_db_test() : db_(new cached_db(DB_PATH)) {
  }

 protected:
  virtual void TearDown() override {
    delete db_;
    boost::filesystem::remove_all(DB_PATH);
  }

  cached_db* db_;
};

/*
 * ### Test Put and Get ###
 */
TEST_F(cached_db_test, put_get) {
  db_->set("Hello", "World");

  boost::optional<std::string> v = db_->get("Hello");

  ASSERT_TRUE(v);
  EXPECT_EQ("World", v.get());
}

/*
 * ### Test Remove ###
 */
TEST_F(cached_db_test, remove) {
  db_->set("Hello", "World");
  boost::optional<std::string> v;

  v = db_->get("Hello");
  ASSERT_TRUE(v);
  EXPECT_EQ("World", v.get());

  db_->remove("Hello");
  v = db_->get("Hello");
  ASSERT_FALSE(v);
}

/*
 * ### Measure Time ###
 */
TEST_F(cached_db_test, time_measurement) {
  using std::chrono::duration_cast;
  using std::chrono::microseconds;
  using std::chrono::steady_clock;

  steady_clock::time_point start = steady_clock::now();

  db_->set("Hello", "World");

  boost::optional<std::string> v = db_->get("Hello");
  ASSERT_TRUE(v);
  EXPECT_EQ("World", v.get());

  steady_clock::time_point end = steady_clock::now();
  auto duration_in_us = duration_cast<microseconds>(end - start).count();
  ASSERT_LE(duration_in_us, 1000);
}
