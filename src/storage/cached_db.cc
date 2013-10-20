// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "dust/storage/cached_db.h"

#include "dust/storage/mem_store.h"
#include "dust/storage/lvl_db_store.h"

namespace dust {

cached_db::cached_db(const std::string& path)
    : db_(new lvl_db_store(path)),
      cache_(new mem_store()) {
  for (const auto& el : db_->all()) {
    cache_->set(el.first, el.second);
  }
}

cached_db::~cached_db() {
}

boost::optional<std::string> cached_db::get(const std::string& key) const {
  return cache_->get(key);
}

void cached_db::set(const std::string& key, const std::string& value) {
  cache_->set(key, value);
  db_->set(key, value);
}

void cached_db::remove(const std::string& key) {
  cache_->remove(key);
  db_->remove(key);
}

std::map<std::string, std::string> cached_db::all() const {
  return cache_->all();
}

}  // namespace dust
