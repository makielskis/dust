// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef DUST_STORAGE_KEY_VALUE_STORE_H_
#define DUST_STORAGE_KEY_VALUE_STORE_H_

#include <string>
#include <map>
#include <stdexcept>

#include "boost/optional.hpp"

namespace dust {

/// Generic database exception class
/// without any additional information.
class db_exception : std::runtime_error {
 public:
  db_exception()
    : std::runtime_error("db exception") {
  }
};

/// Simple interface to retrieve and set data.
class key_value_store {
 public:
  /// Enable child classes to have their destructor called too.
  virtual ~key_value_store() {
  }

  /// \param key  the key to retrieve
  /// \return an optional that is set to the value if it exists
  virtual boost::optional<std::string> get(const std::string& key) const = 0;

  /// \param key    the key to set the value for
  /// \param value  the value to set for the key
  virtual void set(const std::string& key, const std::string& value) = 0;

  /// \param key  the key to remove
  virtual void remove(const std::string& key) = 0;

  /// \return all key/value pairs
  virtual std::map<std::string, std::string> all() const = 0;
};

}  // namespace dust

#endif  // DUST_STORAGE_KEY_VALUE_STORE_H_
