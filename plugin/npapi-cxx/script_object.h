// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#ifndef INCLUDED_SCRIPT_OBJECT_H_
#define INCLUDED_SCRIPT_OBJECT_H_

#include "npapi-cxx/browser.h"
#include "npapi-headers/npruntime.h"
#include "util.h"

namespace npapi {

class PluginInstance;

class ScriptObjectBase : public NPObject {
 public:
  virtual void Invalidate();
  virtual bool HasMethod(NPIdentifier name);
  virtual bool Invoke(NPIdentifier name,
                      const NPVariant *args,
                      uint32_t arg_count,
                      NPVariant *result);
  virtual bool InvokeDefault(const NPVariant *args, uint32_t arg_count,
                             NPVariant *result);
  virtual bool HasProperty(NPIdentifier name);
  virtual bool GetProperty(NPIdentifier name, NPVariant *result);
  virtual bool SetProperty(NPIdentifier name, const NPVariant *value);
  virtual bool RemoveProperty(NPIdentifier name);
  virtual bool Enumerate(NPIdentifier **identifiers,
                         uint32_t *identifier_count);
  virtual bool Construct(const NPVariant *args, uint32_t arg_count,
                         NPVariant *result);

  PluginInstance* plugin_instance();

 protected:
  static void DeallocateThunk(NPObject *npobj);
  static void InvalidateThunk(NPObject *npobj);
  static bool HasMethodThunk(NPObject *npobj, NPIdentifier name);
  static bool InvokeThunk(NPObject *npobj, NPIdentifier name,
                          const NPVariant *args, uint32_t arg_count,
                          NPVariant *result);
  static bool InvokeDefaultThunk(NPObject *npobj,
                                 const NPVariant *args, uint32_t arg_count,
                                 NPVariant *result);
  static bool HasPropertyThunk(NPObject *npobj, NPIdentifier name);
  static bool GetPropertyThunk(NPObject *npobj,
                               NPIdentifier name,
                               NPVariant *result);
  static bool SetPropertyThunk(NPObject *npobj,
                               NPIdentifier name,
                               const NPVariant *value);
  static bool RemovePropertyThunk(NPObject *npobj, NPIdentifier name);
  static bool EnumerateThunk(NPObject *npobj, NPIdentifier **identifiers,
                             uint32_t *identifier_count);
  static bool ConstructThunk(NPObject *npobj,
                             const NPVariant *args,
                             uint32_t arg_count,
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
  static T* Create(NPP npp) {
    return static_cast<T*>(NPN_CreateObject(npp, &npclass));
  }

 protected:
  static NPClass npclass;

  static NPObject* AllocateThunk(NPP npp, NPClass *aClass) {
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
  ScriptObject::AllocateThunk,  // allocate
  ScriptObjectBase::DeallocateThunk,  // deallocate
  ScriptObjectBase::InvalidateThunk,  // invalidate
  ScriptObjectBase::HasMethodThunk,  // hasMethod
  ScriptObjectBase::InvokeThunk,  // invoke
  ScriptObjectBase::InvokeDefaultThunk,  // invokeDefault
  ScriptObjectBase::HasPropertyThunk,  // hasProperty
  ScriptObjectBase::GetPropertyThunk,  // getProperty
  ScriptObjectBase::SetPropertyThunk,  // setProperty
  ScriptObjectBase::RemovePropertyThunk,  // removeProperty
  ScriptObjectBase::EnumerateThunk,  // enumerate
  ScriptObjectBase::ConstructThunk,  // construct
};

}  // namespace npapi


#endif  // INCLUDED_SCRIPT_OBJECT_H_
