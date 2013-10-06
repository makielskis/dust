#include <iostream>

#include "gtest/gtest.h"

#include "rapidjson/rapidjson_with_exception.h"

#include "../src/storage/mem_store.h"
#include "../src/json_visitor.h"


using namespace dust;

typedef rapidjson::Writer<rapidjson::StringBuffer> JsonStringWriter;

class json_test : public testing::Test {
 public:
  json_test() : store_(std::make_shared<mem_store>()) {
  }

 protected:
  std::shared_ptr<key_value_store> store_;
};

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

  rapidjson::StringBuffer buffer;
  JsonStringWriter writer(buffer);
  json_visitor<JsonStringWriter> json(writer);
  writer.StartObject();
  document(store_, "users").accept(json);
  writer.EndObject();

  rapidjson::Document d;
  ASSERT_FALSE(d.Parse<0>(buffer.GetString()).HasParseError());

  ASSERT_TRUE(d.IsObject());
  ASSERT_TRUE(d.HasMember("users"));
  ASSERT_TRUE(d["users"].IsObject());
  ASSERT_TRUE(d["users"].HasMember("foo"));
  ASSERT_TRUE(d["users"]["foo"].IsObject());
  ASSERT_TRUE(d["users"]["foo"].HasMember("bots"));
  ASSERT_TRUE(d["users"]["foo"]["bots"].IsObject());
  ASSERT_TRUE(d["users"]["foo"]["bots"].HasMember("pg_hh_bot"));
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"].IsObject());
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"].HasMember("username"));
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"]["username"].IsString());
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"].HasMember("password"));
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"]["password"].IsString());
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"].HasMember("modules"));
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"]["modules"].IsObject());
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"]["modules"].HasMember("collect"));
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"]["modules"]["collect"].IsObject());
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"]["modules"].HasMember("sell"));
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"]["modules"]["sell"].IsObject());
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"]["modules"]["collect"].HasMember("active"));
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"]["modules"]["collect"]["active"].IsString());
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"]["modules"]["sell"].HasMember("active"));
  ASSERT_TRUE(d["users"]["foo"]["bots"]["pg_hh_bot"]["modules"]["sell"]["active"].IsString());
  ASSERT_STREQ("0", d["users"]["foo"]["bots"]["pg_hh_bot"]["modules"]["sell"]["active"].GetString());
  ASSERT_STREQ("1", d["users"]["foo"]["bots"]["pg_hh_bot"]["modules"]["collect"]["active"].GetString());
  ASSERT_STREQ("penner_name", d["users"]["foo"]["bots"]["pg_hh_bot"]["username"].GetString());
  ASSERT_STREQ("penner_password", d["users"]["foo"]["bots"]["pg_hh_bot"]["password"].GetString());
}
