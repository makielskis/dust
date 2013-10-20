// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include "dust/storage/lvl_db_store.h"

#include <cassert>
#include <memory>

#include "leveldb/db.h"


namespace dust {

lvl_db_store::lvl_db_store(const std::string& path)
    : db_(create_db(path)) {
}

lvl_db_store::~lvl_db_store() {
}

boost::optional<std::string> lvl_db_store::get(const std::string& key) const {
  std::string value;
  leveldb::Status s = db_->Get(leveldb::ReadOptions(), key.c_str(), &value);
  if (!s.IsNotFound()) {
    return boost::optional<std::string>(value);
  } else {
    return boost::optional<std::string>();
  }
}

void lvl_db_store::set(const std::string& key, const std::string& value) {
  leveldb::Status s = db_->Put(leveldb::WriteOptions(), key, value);
  if (!s.ok()) {
    throw db_exception();
  }
}

void lvl_db_store::remove(const std::string& key) {
  leveldb::Status s = db_->Delete(leveldb::WriteOptions(), key);
  if (!s.ok()) {
    std::cout << s.ToString() << "\n";
    throw db_exception();
  }
}

std::map<std::string, std::string> lvl_db_store::all() const {
  std::map<std::string, std::string> entries;
  std::unique_ptr<leveldb::Iterator> i(db_->NewIterator(leveldb::ReadOptions()));
  for (i->SeekToFirst(); i->Valid(); i->Next()) {
    entries[i->key().ToString()] = i->value().ToString();
  }
  assert(i->status().ok());
  return entries;
}

leveldb::DB* lvl_db_store::create_db(const std::string& path) {
  leveldb::DB* db = nullptr;
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status s = leveldb::DB::Open(options, path, &db);
  if (!s.ok()) {
    throw db_exception();
  }
  return db;
}

}  // namespace dust
