#include <iostream>

#include "gtest/gtest.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson_with_exception.h"

#include "dust/storage/mem_store.h"
#include "dust/json_visitor.h"
#include "dust/json_util.h"

using namespace dust;

#define TEST_JSON "{\"test\":{\"yeah\":\"abc\"}}"

const char* config_with_shared_state = R"({
  "guest":{
    "password":"5f4dcc3b5aa765d61d8327deb882cf99",
    "bots":{
       "pg_b_bot":{
          "username":"oclife",
          "modules":{
             "base":{
                "proxy":"",
                "wait_time_factor":"1.00"
             },
             "shared":{
                "loot_from":"-,Textilfetzen,Murmel",
                "active":"0"
             }
          },
          "inactive":"1"
       }
    }
  }
})";

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

TEST_F(json_ignore_test, to_json_ignore_composite_test) {
  dust::document config = document(store_, "users");
  insert_json(config, config_with_shared_state);

  std::string json = document(store_, "users").to_json();

  rapidjson::StringBuffer buffer;
  JsonStringWriter writer(buffer);
  dust::json_visitor<JsonStringWriter> visitor(writer);
  visitor.ignore("password");
  visitor.ignore("shared");
  config.accept(visitor);

  rapidjson::Document d;
  ASSERT_FALSE(d.Parse<0>(buffer.GetString()).HasParseError());

  ASSERT_TRUE(d.IsObject());
  ASSERT_TRUE(d.HasMember("guest"));
  ASSERT_TRUE(d["guest"].IsObject());
  ASSERT_TRUE(d["guest"].HasMember("bots"));
  ASSERT_TRUE(d["guest"]["bots"].IsObject());
  ASSERT_TRUE(d["guest"]["bots"].HasMember("pg_b_bot"));
  ASSERT_TRUE(d["guest"]["bots"]["pg_b_bot"].IsObject());
  ASSERT_TRUE(d["guest"]["bots"]["pg_b_bot"].HasMember("username"));
  ASSERT_TRUE(d["guest"]["bots"]["pg_b_bot"]["username"].IsString());
  EXPECT_FALSE(d["guest"]["bots"]["pg_b_bot"].HasMember("password"));
  ASSERT_TRUE(d["guest"]["bots"]["pg_b_bot"].HasMember("modules"));
  ASSERT_TRUE(d["guest"]["bots"]["pg_b_bot"]["modules"].IsObject());
  ASSERT_TRUE(d["guest"]["bots"]["pg_b_bot"]["modules"].HasMember("base"));
  ASSERT_FALSE(d["guest"]["bots"]["pg_b_bot"]["modules"].HasMember("shared"));
  ASSERT_TRUE(d["guest"]["bots"].IsObject());

}
