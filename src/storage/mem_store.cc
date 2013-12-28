// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "dust/storage/mem_store.h"

namespace dust {

mem_store::~mem_store() {
}

boost::optional<std::string> mem_store::get(const std::string& key) const {
  boost::optional<std::string> ret;

  const auto it = map_.find(key);
  if (it != map_.end()) {
    ret = it->second;
  }

  return ret;
}

void mem_store::set(const std::string& key, const std::string& value) {
  map_[key] = value;
}

void mem_store::remove(const std::string& key) {
  map_.erase(key);
}

std::map<std::string, std::string> mem_store::all() const {
  return std::map<std::string, std::string>(map_.begin(), map_.end());
}

}  // namespace dust
