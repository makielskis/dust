// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef DUST_JSON_VISITOR_H_
#define DUST_JSON_VISITOR_H_

#include <stack>

#include "rapidjson/rapidjson_with_exception.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "./document.h"
#include "./document_visitor.h"

namespace dust {

class document;

template <typename Writer>
class json_visitor : public document_visitor {
 public:
  json_visitor(Writer& writer)
      : writer_(writer) {
  }

  virtual void composite_start(document& doc) override {
    std::string index = doc.index();
		writer_.String(index.c_str(),
                   static_cast<rapidjson::SizeType>(index.length()));
    writer_.StartObject();
  }

  virtual void composite_end(document& doc) override {
    writer_.EndObject();
  }

  virtual void visit_value_document(document& doc) override {
    std::string index = doc.index(), value = doc.val();
		writer_.String(index.c_str(),
                   static_cast<rapidjson::SizeType>(index.length()));
		writer_.String(value.c_str(),
                   static_cast<rapidjson::SizeType>(value.length()));
  }

 private:
  Writer& writer_;
};

}  // namespace dust

#endif  // DUST_JSON_VISITOR_H_
