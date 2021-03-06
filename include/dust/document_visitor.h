// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#ifndef DUST_DOCUMENT_VISITOR_H_
#define DUST_DOCUMENT_VISITOR_H_

namespace dust {

class document;

class document_visitor {
 public:
  virtual ~document_visitor() {
  }

  virtual void composite_start(const document& doc) = 0;
  virtual void composite_end(const document& doc) = 0;
  virtual void visit_key(const document& doc) = 0;
  virtual void visit_value(const document& doc) = 0;
};

}  // namespace dust

#endif  // DUST_DOCUMENT_VISITOR_H_
