// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_EMACS_OBJECT_H_
#define INCLUDED_EMACS_OBJECT_H_

#include "script_object.h"

class EmacsInstance;

class EmacsObject : public npapi::ScriptObject<EmacsObject> {
public:
    bool hasMethod(NPIdentifier name);
    bool invoke(NPIdentifier name,
		const NPVariant *args,
		uint32_t argCount,
		NPVariant *result);
    bool enumerate(NPIdentifier **identifiers, uint32_t *identifierCount);

private:
    friend class npapi::ScriptObject<EmacsObject>;
    EmacsObject(NPP npp);
    ~EmacsObject();

    EmacsInstance* emacsInstance();

    DISALLOW_COPY_AND_ASSIGN(EmacsObject);
};

#endif  // INCLUDED_EMACS_OBJECT_H_
