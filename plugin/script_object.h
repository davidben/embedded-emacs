// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_SCRIPT_OBJECT_H_
#define INCLUDED_SCRIPT_OBJECT_H_

#include "browser.h"
#include "npapi-headers/npruntime.h"
#include "util.h"

namespace npapi {

class PluginInstance;

class ScriptObjectBase : public NPObject {
public:
    virtual void invalidate();
    virtual bool hasMethod(NPIdentifier name);
    virtual bool invoke(NPIdentifier name,
                        const NPVariant *args,
                        uint32_t argCount,
                        NPVariant *result);
    virtual bool invokeDefault(const NPVariant *args, uint32_t argCount,
                               NPVariant *result);
    virtual bool hasProperty(NPIdentifier name);
    virtual bool getProperty(NPIdentifier name, NPVariant *result);
    virtual bool setProperty(NPIdentifier name, const NPVariant *value);
    virtual bool removeProperty(NPIdentifier name);
    virtual bool enumerate(NPIdentifier **identifiers,
                           uint32_t *identifierCount);
    virtual bool construct(const NPVariant *args, uint32_t argCount,
                           NPVariant *result);

    PluginInstance* pluginInstance();

protected:
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
    ScriptObjectBase(NPP npp);
    ~ScriptObjectBase();

    NPP npp_;

  private:
    DISALLOW_COPY_AND_ASSIGN(ScriptObjectBase);
};

template <class T>
class ScriptObject : public ScriptObjectBase {
  public:
    static T* create(NPP npp) {
        return static_cast<T*>(NPN_CreateObject(npp, &npclass));
    }

  protected:
    static NPClass npclass;

    static NPObject* allocateThunk(NPP npp, NPClass *aClass) {
        return new T(npp);
    }

    ScriptObject(NPP npp) : ScriptObjectBase(npp) {}

  private:
    DISALLOW_COPY_AND_ASSIGN(ScriptObject<T>);
};

// Apparently I'm not allowed to define this inline. Okay, then.
template <class T>
NPClass ScriptObject<T>::npclass = {
    NP_CLASS_STRUCT_VERSION,
    ScriptObject::allocateThunk,  // allocate
    ScriptObjectBase::deallocateThunk,  // deallocate
    ScriptObjectBase::invalidateThunk,  // invalidate
    ScriptObjectBase::hasMethodThunk,  // hasMethod
    ScriptObjectBase::invokeThunk,  // invoke
    ScriptObjectBase::invokeDefaultThunk,  // invokeDefault
    ScriptObjectBase::hasPropertyThunk,  // hasProperty
    ScriptObjectBase::getPropertyThunk,  // getProperty
    ScriptObjectBase::setPropertyThunk,  // setProperty
    ScriptObjectBase::removePropertyThunk,  // removeProperty
    ScriptObjectBase::enumerateThunk,  // enumerate
    ScriptObjectBase::constructThunk,  // construct
};

}  // namespace npapi


#endif  // INCLUDED_SCRIPT_OBJECT_H_
