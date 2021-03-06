// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef DUST_DOCUMENT_H_
#define DUST_DOCUMENT_H_

#include <string>
#include <memory>
#include <map>
#include <vector>

#include "./storage/key_value_store.h"
#include "./error.h"


/// This delimiter is used to represent hierarchies:
///
/// Example:
/// JSON value:
///    { 'a': { 'b': 'c' } }
/// Storage representation:
///    a   = '~[b]'
///    a#b = 'c'
#define DELIMITER "\x1F" // ASCII unit seperator

/// This prefix identifies composite values (JSON objects).
///
/// Example:
/// JSON value:
///    { 'a': { 'b': 'c', 'd': 'f' } }
/// Storage representation:
///    a   = '~[b,d]'
///    a#b = 'c'
///    a#d = 'f'
#define COMP_PREFIX "\x01" // ASCII start of heading

// This character marks the begin of a sub-items listing
#define LIST_START "\x02" // ASCII start of text

// This character marks the end of a sub-items listing
#define LIST_END "\x03" // ASCII end of text

// This character seperates items in a sub-items listing
#define LIST_SEP "\x1E" // ASCII record seperator

namespace dust {

class key_value_store;
class document_db;
class document_visitor;

/// A document that is stored in the database.
class document {
 public:
  /// Creates a document from a path.
  ///
  /// \param store      the store this document is located in
  /// \param full_path  the path of this document in the store
  document(
      std::weak_ptr<key_value_store> store,
      std::string full_path);

  /// Creates a document from another path adding an index to it.
  ///
  /// \param store  the store this document is located in
  /// \param path   the path prefix
  /// \param index  the index to append to the path prefix
  document(
      std::weak_ptr<key_value_store> store,
      std::string path,
      std::string index);

  /// \param index  the index to get the document for
  /// \return the document located at this index
  inline document operator[](const std::string& index) const {
    return document(store_, full_path(), index);
  }

  /// Sets the value of this document and stores it to the underlying database.
  ///
  /// \param rhs  the value to assign to
  /// \return this document
  document& operator=(const std::string& rhs);

  /// Compre two documents.
  ///
  /// \param rhs  another document
  /// \return true when this and the other documents are equal
  bool operator==(const document& other);

  /// Set the value of this document and store it in the database.
  ///
  /// \param val  the value to assign
  void assign(const std::string& val);

  /// \return the index of this document
  std::string index() const;

  /// \return the actual (raw) value of this document
  std::string val() const;

  /// \return JSON representation of this value
  std::string to_json() const;

  /// Load a JSON string into the document.
  ///
  /// \ param json  the json string to load
  void from_json(const std::string& json);

  /// \return whether this document exists in the store
  bool exists() const;

  /// Removes the entry and all entries belonging to this entry from the store.
  void remove();

  /// \return true if this is a composite document.
  bool is_composite() const;

  /// \return the children of this document.
  std::vector<document> children() const;

  /// Calls the visit methods of the visitor.
  void accept(document_visitor& visitor) const;

  /// \return the full path of this document
  std::string full_path() const;

 protected:
  /// Transaction helper class storing actions that can then be commited.
  class transaction {
   public:
    explicit transaction(std::shared_ptr<key_value_store> store)
        : store_(std::move(store)) {
    }

    // Disable copy, enable move.
    transaction(const transaction&) = delete;
    transaction& operator=(const transaction&) = delete;

    transaction(transaction&& o) {
      store_ = std::move(o.store_);
      actions_ = std::move(o.actions_);
    }

    transaction& operator=(transaction&& o) {
      store_ = std::move(o.store_);
      actions_ = std::move(o.actions_);
      return *this;
    }

    void add(const std::string& key, const std::string& value) {
      actions_[key] = value;
    }

    void commit() {
      for (const auto& v : actions_) {
        store_->set(v.first, v.second);
      }
    }

   private:
    std::shared_ptr<key_value_store> store_;
    std::map<std::string, std::string> actions_;
  };

  /// Allow the document_db to access the protected constructors.
  friend class document_db;

  /// Set helper method for document::operator=().
  ///
  /// This method stores all actions it would do into a transaction.
  /// If no exceptions were thrown while processing, this transaction can then
  /// be commited.
  ///
  /// \param rhs  the value this node should be set to
  /// \param tr   the transaction to write the actions to
  transaction set(const std::string& rhs, transaction tr);

  /// Remove helper method for document::remove().
  ///
  /// If the parent was a composite document containing only the removed doc,
  /// also the parent doc will be removed. This method does this recursively.
  void clear_parents();

  /// Remove helper method for document::remove().
  ///
  /// Removes all child values recusively.
  void clear_children();

  /// \return the child keys of this node if it is a composite document
  ///         otherwise an empty vector
  std::vector<std::string> child_keys() const;

  /// Aquires a lock for the document::store_ pointer.
  /// Throws an runtime error if this fails.
  std::shared_ptr<key_value_store> get_store() const;

  /// Joins the given vector to a string using the delimiter.
  ///
  /// \param vec    the strings to join
  /// \param delim  the delimiter to use
  /// \return the joined path string
  static std::string join(
      const std::vector<std::string>& vec,
      const char* delim = "#");

  std::weak_ptr<key_value_store> store_;
  std::string path_;
  std::string index_;
};

}  // namespace dust

#endif  // DUST_DOCUMENT_H_
