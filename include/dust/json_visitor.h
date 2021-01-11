// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef DUST_JSON_VISITOR_H_
#define DUST_JSON_VISITOR_H_

#include <set>

#include "rapidjson/rapidjson.h"

#include "./document.h"
#include "./document_visitor.h"

namespace dust {

class document;

template <typename Writer>
class json_visitor : public document_visitor {
 public:
  enum class ignore_type {
    OFF,
    UNKOWN_VALUE,
    COMPOSITE_VALUE
  };

  json_visitor(Writer& writer)
      : writer_(writer),
        ignore_(ignore_type::OFF) {
  }

  virtual void composite_start(const document& /* doc */) override {
    if (ignore_ == ignore_type::OFF) {
      writer_.StartObject();
    } else if (ignore_ == ignore_type::UNKOWN_VALUE) {
      ignore_ = ignore_type::COMPOSITE_VALUE;
    }
  }

  virtual void composite_end(const document& /* doc */) override {
    if (ignore_ != ignore_type::COMPOSITE_VALUE) {
      writer_.EndObject();
    } else {
      ignore_ = ignore_type::OFF;
    }
  }

  virtual void visit_key(const document& doc) override {
    if (ignore_ != ignore_type::OFF) {
      return;
    }

    std::string index = doc.index();
    if (ignored_attrs_.find(index) == ignored_attrs_.end()) {
      writer_.String(index.c_str(),
                     static_cast<rapidjson::SizeType>(index.length()));
    } else {
      ignore_ = ignore_type::UNKOWN_VALUE;
    }
  }

  virtual void visit_value(const document& doc) override {
    if (ignore_ == ignore_type::OFF) {
      std::string value = doc.val();
      writer_.String(value.c_str(),
                     static_cast<rapidjson::SizeType>(value.length()));
    } else if (ignore_ == ignore_type::UNKOWN_VALUE) {
      ignore_ = ignore_type::OFF;
    }
  }

  void ignore(const std::string& key) {
    ignored_attrs_.insert(key);
  }

  void unignore(const std::string& key) {
    ignored_attrs_.erase(key);
  }

 private:
  Writer& writer_;
  std::set<std::string> ignored_attrs_;
  ignore_type ignore_;
};

}  // namespace dust

#endif  // DUST_JSON_VISITOR_H_
