#include <iostream>
#include <chrono>

#include "gtest/gtest.h"

#include "boost/filesystem.hpp"

#include "dust/storage/cached_db.h"

#define DB_PATH "test_db"

using namespace dust;

class cached_db_test : public testing::Test {
 public:
  cached_db_test() : db_(new cached_db(DB_PATH)) {
  }

 protected:
  virtual void TearDown() override {
    if (db_ != nullptr) {
      delete db_;
    }
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

  boost::optional<std::string> v = db_->get("Hello");
  ASSERT_TRUE(v);
  EXPECT_EQ("World", v.get());

  db_->remove("Hello");
  v = db_->get("Hello");
  ASSERT_FALSE(v);
}

/*
 * ### Test get all ###
 */
TEST_F(cached_db_test, all) {
  db_->set("Hello", "World");
  EXPECT_EQ("World", db_->all()["Hello"]);
}

/*
 * ### Test get all ###
 */
TEST_F(cached_db_test, reload) {
  db_->set("Hello", "World");

  delete db_;
  db_ = new cached_db(DB_PATH);

  EXPECT_EQ("World", db_->all()["Hello"]);
}
