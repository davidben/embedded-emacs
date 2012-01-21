// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#ifndef INCLUDED_EMACS_OBJECT_H_
#define INCLUDED_EMACS_OBJECT_H_

#include "npapi-cxx/script_object.h"

class EmacsManager;

class EmacsObject : public npapi::ScriptObject<EmacsObject> {
 public:
  bool HasMethod(NPIdentifier name);
  bool Invoke(NPIdentifier name,
              const NPVariant *args,
              uint32_t arg_count,
              NPVariant *result);
  bool Enumerate(NPIdentifier **identifiers, uint32_t *identifier_count);

 private:
  friend class npapi::ScriptObject<EmacsObject>;
  EmacsObject(NPP npp);
  ~EmacsObject();

  EmacsManager* emacs_manager();

  DISALLOW_COPY_AND_ASSIGN(EmacsObject);
};

#endif  // INCLUDED_EMACS_OBJECT_H_
