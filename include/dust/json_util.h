// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef DUST_JSON_UTIL_H_
#define DUST_JSON_UTIL_H_

#include <string>

namespace dust {

class document;

void insert_json(document& d, const std::string& json_string);

}  // namespace dust

#endif  // DUST_JSON_UTIL_H_
