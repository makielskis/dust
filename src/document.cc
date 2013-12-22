// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "dust/document.h"

#include <utility>
#include <algorithm>

#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/system/system_error.hpp"

#include "dust/storage/key_value_store.h"
#include "dust/document_visitor.h"
#include "dust/error.h"

namespace dust {

document::document(std::weak_ptr<key_value_store> store,
                   std::string full_path)
    : store_(std::move(store)) {
  auto pos = full_path.rfind(DELIMITER);
  if (pos == std::string::npos) {
    index_ = full_path;
    return;
  }

  path_ = full_path.substr(0, pos);
  index_ = full_path.substr(pos + 1);
}

document::document(std::weak_ptr<key_value_store> store,
                   std::string path,
                   std::string index)
    : store_(std::move(store)),
      path_(std::move(path)),
      index_(std::move(index)) {
}

std::string document::index() const {
  return index_;
}

std::string document::val() const {
  auto opt_val = get_store()->get(full_path());
  if (!opt_val) {
    throw boost::system::system_error(error::value_does_not_exist);
  }
  return opt_val.get();
}

bool document::exists() const {
  return get_store()->get(full_path());
}

void document::remove() {
  if (!exists()) {
    return;
  }

  clear_parents();
  clear_children();
}

bool document::is_composite() const {
  if (!exists()) {
    return false;
  }

  std::string v = val();
  return (v.length() > 3 && v[0] == '~');
}

std::vector<document> document::children() const {
  if (!is_composite()) {
    return std::vector<document>();
  }

  // Split child index list.
  std::string v = val();
  std::string keys = v.substr(2, v.length() - 3);
  std::vector<std::string> split;
  boost::split(split, keys, boost::algorithm::is_any_of(","));

  // Create child documents using the document::operator[]() operator.
  std::vector<document> children;
  for (const std::string& key : split) {
    children.emplace_back(operator[](key));
  }

  return children;
}

void document::accept(document_visitor& visitor) const {
  if (is_composite()) {
    visitor.composite_start(*this);
    for (auto& child : children()) {
      visitor.visit_key(child);
      child.accept(visitor);
    }
    visitor.composite_end(*this);
  } else {
    visitor.visit_value(*this);
  }
}

void document::clear_children() {
  if (is_composite()) {
    for (auto child : children()) {
      child.clear_children();
    }
  }

  get_store()->remove(full_path());
}

void document::clear_parents() {
  // Check whether the are parents at all.
  if (path_.empty()) {
    return;
  }

  document parent(store_, path_);
  if (!parent.exists()) {
    throw boost::system::system_error(error::parent_does_not_exist);
  }

  if (!parent.is_composite()) {
    throw boost::system::system_error(error::parent_not_a_composite);
  }

  // Make sure that the parent composite document contains this document.
  std::vector<std::string> parents_childs = parent.child_keys();
  auto it = std::find(parents_childs.begin(), parents_childs.end(), index_);
  if (it == parents_childs.end()) {
    throw boost::system::system_error(error::child_not_in_parent);
  }

  // Remove this node from the parent node.
  parents_childs.erase(it);

  // Update parent node.
  if (parents_childs.empty()) {
    // No children left in the parent node.
    // Remove it completely.
    get_store()->remove(parent.full_path());
    parent.clear_parents();
  } else {
    // Set the remaining children.
    std::string new_val = std::string("~[") + join(parents_childs, ",") + "]";
    get_store()->set(parent.full_path(), new_val);
  }
}

document& document::operator=(const std::string& rhs) {
  auto store = get_store();
  transaction tr(store);
  tr = set(rhs, std::move(tr));
  tr.commit();
  return *this;
}

void document::assign(const std::string& val) {
  operator=(val);
}

document::transaction document::set(const std::string& rhs,
                                    document::transaction tr) {
  auto store = get_store();

  // If this is a composite value: allow only changing it to another
  // composite value.
  if (is_composite()) {
    if (rhs.length() < 3 || rhs[0] != '~') {
      throw boost::system::system_error(error::override_with_composite);
    } else {
      tr.add(full_path(), rhs);
      return tr;
    }
  }

  // Create reference to this node in parent nodes (recursive).
  if (!path_.empty()) {
    document parent(store_, path_);
    if (parent.exists()) {
      if (!parent.is_composite()) {
        throw boost::system::system_error(error::parent_not_a_composite);
      }
      std::vector<std::string> split = parent.child_keys();
      if (std::find(split.begin(), split.end(), index_) == split.end()) {
        split.push_back(index_);
        std::string new_parent_val = std::string("~[") + join(split, ",") + "]";
        tr = parent.set(new_parent_val, std::move(tr));
      }
    } else {
      tr = parent.set(std::string("~[") + index_ + "]", std::move(tr));
    }
  }

  tr.add(full_path(), rhs);
  return tr;
}

std::vector<std::string> document::child_keys() const {
  if (!is_composite() || !exists()) {
    return std::vector<std::string>();
  }

  std::string value = val();
  std::string keys = value.substr(2, value.length() - 3);
  std::vector<std::string> split;
  boost::split(split, keys, boost::algorithm::is_any_of(","));

  return split;
}

std::string document::full_path() const {
  return path_.empty() ? index_ : (path_ + DELIMITER + index_);
}

std::shared_ptr<key_value_store> document::get_store() const {
  std::shared_ptr<key_value_store> lock = store_.lock();
  if (lock == std::shared_ptr<key_value_store>()) {
    throw boost::system::system_error(error::store_dead);
  }
  return lock;
}

std::string document::join(const std::vector<std::string>& vec,
                           const char* delimiter) {
  std::string joined;
  for (const auto& s : vec) {
    joined = joined + s + delimiter;
  }
  return joined.substr(0, joined.length() - 1);
}

}  // namespace dust
