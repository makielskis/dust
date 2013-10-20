// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef DUST_STORAGE_CACHED_DB_H_
#define DUST_STORAGE_CACHED_DB_H_

#include <string>
#include <unordered_map>
#include <memory>

#include "./key_value_store.h"

namespace dust {

class key_value_store;

/// Stores data in a in-memory map.
class cached_db : public key_value_store {
 public:
  cached_db(const std::string& path);
  virtual ~cached_db();

  virtual boost::optional<std::string> get(const std::string& key) const override;
  virtual void set(const std::string& key, const std::string& value) override;
  virtual void remove(const std::string& key) override;
  virtual std::map<std::string, std::string> all() const override;

 protected:
  std::unique_ptr<key_value_store> db_;
  std::unique_ptr<key_value_store> cache_;
};

}  // namespace dust

#endif  // DUST_STORAGE_CACHED_DB_H_
