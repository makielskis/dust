#include <iostream>

#include "gtest/gtest.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson_with_exception.h"

#include "dust/storage/mem_store.h"
#include "dust/json_visitor.h"
#include "dust/json_util.h"

#define TEST_JSON "{\"test\":{\"yeah\":\"abc\"}}"

using namespace dust;

typedef rapidjson::Writer<rapidjson::StringBuffer> JsonStringWriter;

class json_ignore_test : public testing::Test {
 public:
  json_ignore_test() : store_(std::make_shared<mem_store>()) {
  }

 protected:
  std::shared_ptr<key_value_store> store_;
};

TEST_F(json_ignore_test, to_json_ignore_test) {
  document config = document(store_, "users")["foo"]["bots"]["pg_hh_bot"];

  config["username"] = "penner_name";
  config["password"] = "penner_password";
  document modules = config["modules"];
  document collect = modules["collect"];
  collect["active"] = "1";
  document sell = modules["sell"];
  sell["price"] = "20";
  sell["active"] = "0";

  rapidjson::StringBuffer buffer;
  JsonStringWriter writer(buffer);
  json_visitor<JsonStringWriter> json(writer);
  json.ignore("password");
  document(store_, "users").accept(json);

  rapidjson::Document d;
  ASSERT_FALSE(d.Parse<0>(buffer.GetString()).HasParseError());

  ASSERT_TRUE(d.IsObject());
  ASSERT_TRUE(d.HasMember("foo"));
  ASSERT_TRUE(d["foo"].IsObject());
  ASSERT_TRUE(d["foo"].HasMember("bots"));
  ASSERT_TRUE(d["foo"]["bots"].IsObject());
  ASSERT_TRUE(d["foo"]["bots"].HasMember("pg_hh_bot"));
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"].IsObject());
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"].HasMember("username"));
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"]["username"].IsString());
  EXPECT_FALSE(d["foo"]["bots"]["pg_hh_bot"].HasMember("password"));
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"].HasMember("modules"));
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"]["modules"].IsObject());
  ASSERT_TRUE(d["foo"]["bots"].IsObject());
}
