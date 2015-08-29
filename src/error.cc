// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "dust/error.h"

namespace dust {

namespace error {
  boost::system::error_code make_error_code(error_code_t e) noexcept {
    return boost::system::error_code(static_cast<int>(e), error_category());
  }

  boost::system::error_condition make_error_condition(error_condition_t e) noexcept {
    return boost::system::error_condition(static_cast<int>(e), error_category());
  }
}  // namespace error

const char* error_category_impl::name() const noexcept {
    return "dust";
}

std::string error_category_impl::message(int ev) const noexcept  {
  switch (ev) {
    case error::value_does_not_exist:
      return "Database value does not exist";

    case error::parent_does_not_exist:
      return "Inconsistent database: parent does not exist";

    case error::parent_not_a_composite:
      return "Inconsistent database: parent is not a composite";

    case error::child_not_in_parent:
      return "Inconsistent database: child not in parent";

    case error::override_with_composite:
      return "Can't override value with composite";

    case error::store_dead:
      return "Database problem: reference to database lost";

    case error::unsupported_value_type:
      return "Insertion problem: unsupported datatype (not String/Object)";

    case error::invalid_json:
      return "Insertion problem: invalid JSON string";

    default:
      return "Unknown error";
  }
}

boost::system::error_condition
error_category_impl::default_error_condition(int ev) const noexcept {
  switch (ev)
  {
    case error::parent_does_not_exist:
    case error::parent_not_a_composite:
    case error::child_not_in_parent:
      return boost::system::error_condition(error::corrupt_database, *this);

    default:
      return boost::system::error_condition(ev, *this);
  }
}

const boost::system::error_category& error_category() {
  static error_category_impl instance;
  return instance;
}

}  // namespace dust
