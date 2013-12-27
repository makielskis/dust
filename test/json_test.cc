#include <iostream>

#include "gtest/gtest.h"

#include "rapidjson/rapidjson_with_exception.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "dust/storage/mem_store.h"
#include "dust/json_visitor.h"
#include "dust/json_util.h"

#define TEST_JSON "{\"username\":\"penner_name\",\"password\":\"penner_password\",\"modules\":{\"collect\":{\"active\":\"1\"},\"sell\":{\"price\":\"20\",\"active\":\"0\"}}}"

using namespace dust;

typedef rapidjson::Writer<rapidjson::StringBuffer> JsonStringWriter;

class json_test : public testing::Test {
 public:
  json_test() : store_(std::make_shared<mem_store>()) {
  }

 protected:
  std::shared_ptr<key_value_store> store_;
};

TEST_F(json_test, from_to_test) {
  std::string json_str = TEST_JSON;

  document config = document(store_, "users")["foo"]["bots"]["pg_hh_bot"];
  insert_json(config, json_str);

  std::string json = config.to_json();

  EXPECT_EQ(json, TEST_JSON);
}

TEST_F(json_test, to_json_test) {
  document config = document(store_, "users")["foo"]["bots"]["pg_hh_bot"];

  config["username"] = "penner_name";
  config["password"] = "penner_password";
  document modules = config["modules"];
  document collect = modules["collect"];
  collect["active"] = "1";
  document sell = modules["sell"];
  sell["price"] = "20";
  sell["active"] = "0";

  std::string json = document(store_, "users").to_json();
  rapidjson::Document d;
  ASSERT_FALSE(d.Parse<0>(json.c_str()).HasParseError());

  ASSERT_TRUE(d.IsObject());
  ASSERT_TRUE(d.HasMember("foo"));
  ASSERT_TRUE(d["foo"].IsObject());
  ASSERT_TRUE(d["foo"].HasMember("bots"));
  ASSERT_TRUE(d["foo"]["bots"].IsObject());
  ASSERT_TRUE(d["foo"]["bots"].HasMember("pg_hh_bot"));
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"].IsObject());
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"].HasMember("username"));
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"]["username"].IsString());
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"].HasMember("password"));
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"]["password"].IsString());
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"].HasMember("modules"));
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"]["modules"].IsObject());
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"]["modules"].HasMember("collect"));
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"]["modules"]["collect"].IsObject());
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"]["modules"].HasMember("sell"));
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"]["modules"]["sell"].IsObject());
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"]["modules"]["collect"].HasMember("active"));
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"]["modules"]["collect"]["active"].IsString());
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"]["modules"]["sell"].HasMember("active"));
  ASSERT_TRUE(d["foo"]["bots"]["pg_hh_bot"]["modules"]["sell"]["active"].IsString());
  ASSERT_STREQ("0", d["foo"]["bots"]["pg_hh_bot"]["modules"]["sell"]["active"].GetString());
  ASSERT_STREQ("1", d["foo"]["bots"]["pg_hh_bot"]["modules"]["collect"]["active"].GetString());
  ASSERT_STREQ("penner_name", d["foo"]["bots"]["pg_hh_bot"]["username"].GetString());
  ASSERT_STREQ("penner_password", d["foo"]["bots"]["pg_hh_bot"]["password"].GetString());
}
