// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef DUST_STORAGE_MEM_STORE_H_
#define DUST_STORAGE_MEM_STORE_H_

#include "./key_value_store.h"

#include <string>
#include <unordered_map>

namespace dust {

/// Stores data in a in-memory map.
class mem_store : public key_value_store {
 public:
  virtual ~mem_store();

  virtual boost::optional<std::string> get(const std::string& key) const override;
  virtual void set(const std::string& key, const std::string& value) override;
  virtual void remove(const std::string& key) override;

 protected:
  std::unordered_map<std::string, std::string> map_;
};

}  // namespace dust

#endif  // DUST_STORAGE_MEM_STORE_H_
