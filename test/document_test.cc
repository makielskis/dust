#include <memory>
#include <cstring>

#include "gtest/gtest.h"

#include "boost/system/system_error.hpp"

#include "dust/document.h"
#include "dust/storage/mem_store.h"

using namespace dust;

class document_test: public testing::Test {
 public:
  document_test()
      : store_(std::make_shared<mem_store>()) {
  }

 protected:
  std::shared_ptr<key_value_store> store_;
};

TEST_F(document_test, delete_test) {
  store_->set("t", COMP_PREFIX LIST_START "q" LIST_SEP "s" LIST_END);
  store_->set("t" DELIMITER "q", COMP_PREFIX LIST_START "p" LIST_END);
  store_->set("t" DELIMITER "q" DELIMITER "p", COMP_PREFIX LIST_START "a" LIST_END);
  store_->set("t" DELIMITER "q" DELIMITER "p" DELIMITER "a", COMP_PREFIX LIST_START "b" LIST_SEP "c" LIST_SEP "d" LIST_END);
  store_->set("t" DELIMITER "q" DELIMITER "p" DELIMITER "a" DELIMITER "b", "1");
  store_->set("t" DELIMITER "q" DELIMITER "p" DELIMITER "a" DELIMITER "c", "2");
  store_->set("t" DELIMITER "q" DELIMITER "p" DELIMITER "a" DELIMITER "d", COMP_PREFIX LIST_START "e" LIST_SEP "f" LIST_END);
  store_->set("t" DELIMITER "q" DELIMITER "p" DELIMITER "a" DELIMITER "d" DELIMITER "e", "3");
  store_->set("t" DELIMITER "q" DELIMITER "p" DELIMITER "a" DELIMITER "d" DELIMITER "f", "4");
  store_->set("t" DELIMITER "s", "5");

  document a(store_, "t");
  a = a["q"]["p"]["a"];
  a.remove();

  ASSERT_TRUE(store_->get("t"));
  EXPECT_EQ(COMP_PREFIX LIST_START "s" LIST_END, store_->get("t").get());
  ASSERT_TRUE(store_->get("t" DELIMITER "s"));
  EXPECT_EQ("5", store_->get("t" DELIMITER "s").get());
  EXPECT_FALSE(store_->get("t" DELIMITER "q"));
  EXPECT_FALSE(store_->get("t" DELIMITER "q" DELIMITER "p"));
  EXPECT_FALSE(store_->get("t" DELIMITER "q" DELIMITER "p" DELIMITER "a"));
  EXPECT_FALSE(store_->get("t" DELIMITER "q" DELIMITER "p" DELIMITER "a" DELIMITER "b"));
  EXPECT_FALSE(store_->get("t" DELIMITER "q" DELIMITER "p" DELIMITER "p" DELIMITER "a" DELIMITER "b"));
  EXPECT_FALSE(store_->get("t" DELIMITER "q" DELIMITER "p" DELIMITER "a" DELIMITER "d"));
  EXPECT_FALSE(store_->get("t" DELIMITER "q" DELIMITER "p" DELIMITER "a" DELIMITER "d" DELIMITER "e"));
  EXPECT_FALSE(store_->get("t" DELIMITER "q" DELIMITER "p" DELIMITER "a" DELIMITER "d" DELIMITER "f"));
}

TEST_F(document_test, set_test) {
  document c(store_, "a");
  c = c["b"]["c"];
  c = "1";

  ASSERT_TRUE(store_->get("a"));
  ASSERT_TRUE(store_->get("a" DELIMITER "b"));
  ASSERT_TRUE(store_->get("a" DELIMITER "b" DELIMITER "c"));

  EXPECT_EQ(COMP_PREFIX LIST_START "b" LIST_END, store_->get("a").get());
  EXPECT_EQ(COMP_PREFIX LIST_START "c" LIST_END, store_->get("a" DELIMITER "b").get());
  EXPECT_EQ("1", store_->get("a" DELIMITER "b" DELIMITER "c").get());
}

TEST_F(document_test, override_value_fail_test) {
  store_->set("p", "1");

  document p(store_, "p");
  EXPECT_THROW(p["a"]["d"]["f"] = "2", boost::system::system_error);
  EXPECT_FALSE(store_->get("p" DELIMITER "a" DELIMITER "d" DELIMITER "f"));
  EXPECT_FALSE(store_->get("p" DELIMITER "a" DELIMITER "d"));
  EXPECT_FALSE(store_->get("p" DELIMITER "a"));
  ASSERT_TRUE(store_->get("p"));
  EXPECT_EQ("1", store_->get("p").get());
}

TEST_F(document_test, override_value_error_message_test) {
  store_->set("p", "1");

  document p(store_, "p");
  bool thrown = false;
  try {
    p["a"]["d"]["f"] = "2";
  } catch (const boost::system::system_error& e) {
    thrown = true;
    EXPECT_TRUE(std::strstr(e.what(), "composite") != 0
                || std::strstr(e.what(), "Composite") != 0);
  }
  EXPECT_TRUE(thrown);
}

TEST_F(document_test, override_composite_fail_test) {
  store_->set("p", COMP_PREFIX LIST_START "a" LIST_END);
  store_->set("p" DELIMITER "a", "1");

  document p(store_, "p");
  EXPECT_THROW({
    p = "b";
  }, boost::system::system_error);
}

TEST_F(document_test, override_composite_error_message_test) {
  store_->set("p", COMP_PREFIX LIST_START "a" LIST_END);
  store_->set("p" DELIMITER "a", "1");

  document p(store_, "p");
  bool thrown = false;
  try {
    p = "b";
  } catch (const boost::system::system_error& e) {
    thrown = true;
    EXPECT_TRUE(std::strstr(e.what(), "composite") != 0
                || std::strstr(e.what(), "Composite") != 0);
  }
  EXPECT_TRUE(thrown);
}

TEST_F(document_test, is_composite_test) {
  document c(store_, "a");
  c = c["b"]["c"];
  c = "1";

  document a(store_, "a");
  document b = document(store_, "a")["b"];

  EXPECT_TRUE(a.is_composite());
  EXPECT_TRUE(b.is_composite());
  EXPECT_FALSE(c.is_composite());
}

TEST_F(document_test, get_children) {
  document c(store_, "a");
  c = c["b"]["c"];
  c = "1";

  document a(store_, "a");
  document b = document(store_, "a")["b"];

  EXPECT_EQ(1, a.children().size());
  EXPECT_EQ(1, b.children().size());
  EXPECT_EQ(0, c.children().size());
}
