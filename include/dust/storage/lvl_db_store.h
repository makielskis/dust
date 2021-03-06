// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef DUST_STORAGE_LVL_DB_STORE_H_
#define DUST_STORAGE_LVL_DB_STORE_H_

#include "./key_value_store.h"

#include <memory>
#include <string>

namespace leveldb {
class DB;
}

namespace dust {

/// Stores data in a leveldb database.
class lvl_db_store : public key_value_store {
 public:
  lvl_db_store(const std::string& path);
  virtual ~lvl_db_store();

  virtual boost::optional<std::string> get(const std::string& key) const override;
  virtual void set(const std::string& key, const std::string& value) override;
  virtual void remove(const std::string& key) override;
  virtual std::map<std::string, std::string> all() const override;

 protected:
  static leveldb::DB* create_db(const std::string& path);

  std::unique_ptr<leveldb::DB> db_;
};

}  // namespace dust

#endif  // DUST_STORAGE_LVL_DB_STORE_H_
