#include <iostream>
#include <memory>

#include "gtest/gtest.h"

#include "boost/system/system_error.hpp"

#include "dust/document.h"
#include "dust/storage/mem_store.h"

using namespace dust;

class document_test : public testing::Test {
 public:
  document_test() : store_(std::make_shared<mem_store>()) {
  }

 protected:
  std::shared_ptr<key_value_store> store_;
};

TEST_F(document_test, delete_test) {
  store_->set("t", "~[q,s]");
  store_->set("t#q", "~[p]");
  store_->set("t#q#p", "~[a]");
  store_->set("t#q#p#a", "~[b,c,d]");
  store_->set("t#q#p#a#b", "1");
  store_->set("t#q#p#a#c", "2");
  store_->set("t#q#p#a#d", "~[e,f]");
  store_->set("t#q#p#a#d#e", "3");
  store_->set("t#q#p#a#d#f", "4");
  store_->set("t#s", "5");

  document a(store_, "t");
  a = a["q"]["p"]["a"];
  a.remove();

  ASSERT_TRUE(store_->get("t"));
  EXPECT_EQ("~[s]", store_->get("t").get());
  ASSERT_TRUE(store_->get("t#s"));
  EXPECT_EQ("5", store_->get("t#s").get());
  EXPECT_FALSE(store_->get("t#q"));
  EXPECT_FALSE(store_->get("t#q#p"));
  EXPECT_FALSE(store_->get("t#q#p#a"));
  EXPECT_FALSE(store_->get("t#q#p#a#b"));
  EXPECT_FALSE(store_->get("t#q#p#p#a#b"));
  EXPECT_FALSE(store_->get("t#q#p#a#d"));
  EXPECT_FALSE(store_->get("t#q#p#a#d#e"));
  EXPECT_FALSE(store_->get("t#q#p#a#d#f"));
}

TEST_F(document_test, set_test) {
  document c(store_, "a");
  c = c["b"]["c"];
  c = "1";

  ASSERT_TRUE(store_->get("a"));
  ASSERT_TRUE(store_->get("a#b"));
  ASSERT_TRUE(store_->get("a#b#c"));

  EXPECT_EQ("~[b]", store_->get("a").get());
  EXPECT_EQ("~[c]", store_->get("a#b").get());
  EXPECT_EQ("1", store_->get("a#b#c").get());
}

TEST_F(document_test, set_fail_test) {
  store_->set("p", "1");

  document p(store_, "p");
  EXPECT_THROW(p["a"]["d"]["f"] = "2", boost::system::system_error);
  EXPECT_FALSE(store_->get("p#a#d#f"));
  EXPECT_FALSE(store_->get("p#a#d"));
  EXPECT_FALSE(store_->get("p#a"));
  ASSERT_TRUE(store_->get("p"));
  EXPECT_EQ("1", store_->get("p").get());
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
