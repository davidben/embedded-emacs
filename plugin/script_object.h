// Copyright (c) 2010 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_SCRIPT_OBJECT_H_
#define INCLUDED_SCRIPT_OBJECT_H_

#include "browser.h"
#include "npapi-headers/npruntime.h"
#include "util.h"

class EmacsInstance;

class ScriptObject : public NPObject {
public:
    static ScriptObject* create(NPP npp);

    void invalidate();
    bool hasMethod(NPIdentifier name);
    bool invoke(NPIdentifier name,
		const NPVariant *args,
		uint32_t argCount,
		NPVariant *result);
    bool invokeDefault(const NPVariant *args, uint32_t argCount,
		       NPVariant *result);
    bool hasProperty(NPIdentifier name);
    bool getProperty(NPIdentifier name, NPVariant *result);
    bool setProperty(NPIdentifier name, const NPVariant *value);
    bool removeProperty(NPIdentifier name);
    bool enumerate(NPIdentifier **identifiers, uint32_t *identifierCount);
    bool construct(const NPVariant *args, uint32_t argCount, NPVariant *result);

    static NPObject* allocateThunk(NPP npp, NPClass *aClass);
    static void deallocateThunk(NPObject *npobj);
    static void invalidateThunk(NPObject *npobj);
    static bool hasMethodThunk(NPObject *npobj, NPIdentifier name);
    static bool invokeThunk(NPObject *npobj, NPIdentifier name,
			    const NPVariant *args, uint32_t argCount,
			    NPVariant *result);
    static bool invokeDefaultThunk(NPObject *npobj,
				   const NPVariant *args, uint32_t argCount,
				   NPVariant *result);
    static bool hasPropertyThunk(NPObject *npobj, NPIdentifier name);
    static bool getPropertyThunk(NPObject *npobj,
				 NPIdentifier name,
				 NPVariant *result);
    static bool setPropertyThunk(NPObject *npobj,
				 NPIdentifier name,
				 const NPVariant *value);
    static bool removePropertyThunk(NPObject *npobj, NPIdentifier name);
    static bool enumerateThunk(NPObject *npobj, NPIdentifier **identifiers,
			       uint32_t *identifierCount);
    static bool constructThunk(NPObject *npobj,
			       const NPVariant *args,
			       uint32_t argCount,
			       NPVariant *result);

private:
    ScriptObject(NPP npp);
    ~ScriptObject();

    EmacsInstance* emacsInstance();
    
    NPP npp_;

    DISALLOW_COPY_AND_ASSIGN(ScriptObject);
};

#endif  // INCLUDED_SCRIPT_OBJECT_H_
