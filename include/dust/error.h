// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef DUST_ERROR_H_
#define DUST_ERROR_H_

#include <iostream>

#include "boost/system/system_error.hpp"
#include "boost/type_traits.hpp"

namespace dust {

namespace error {
  // Specific errors
  enum error_code_t {
    value_does_not_exist = 100,

    parent_does_not_exist = 111,
    parent_not_a_composite = 112,
    child_not_in_parent = 113,

    override_with_composite = 120,

    store_dead = 130,

    unsupported_value_type =  140,
    invalid_json = 141
  };

  enum error_condition_t {
    corrupt_database = 1
  };

  boost::system::error_code make_error_code(error_code_t e) noexcept;
  boost::system::error_condition make_error_condition(error_condition_t e) noexcept;
}

class error_category_impl : public boost::system::error_category {
 public:
  virtual const char* name() const noexcept;
  virtual std::string message(int ev) const noexcept;
  virtual boost::system::error_condition default_error_condition(int ev) const noexcept;
};

const boost::system::error_category& error_category();

}  // namespace dust

namespace boost {
namespace system {
  template <>
  struct is_error_code_enum<dust::error::error_code_t>
    : public boost::true_type {};

  template <>
  struct is_error_condition_enum<dust::error::error_condition_t>
    : public boost::true_type {};
}  // namespace system
}  // namespace boost

#endif  // DUST_ERROR_H_
