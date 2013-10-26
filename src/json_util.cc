// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "dust/json_util.h"

#include "rapidjson/rapidjson_with_exception.h"

#include "dust/document.h"
#include "dust/error.h"

namespace json = rapidjson;

namespace dust {

void insert_json_recursive(document& d, const json::Value& v) {
  typedef json::Value::ConstMemberIterator ConstJsonIt;

  if (v.IsString()) {
    d = v.GetString();
  } else if (v.IsObject()) {
    for (ConstJsonIt i = v.MemberBegin(); i != v.MemberEnd(); ++i) {
      document child_doc = d[i->name.GetString()];
      insert_json_recursive(child_doc, i->value);
    }
  } else {
    throw boost::system::system_error(error::unsupported_value_type);
  }
}

void insert_json(document& d, const std::string& json_string) {
  json::Document json_document;
  if (json_document.Parse<0>(json_string.c_str()).HasParseError()) {
    throw boost::system::system_error(error::invalid_json);
  }
  insert_json_recursive(d, json_document);
}

}  // namespace dust
