// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#include "npapi-cxx/script_object.h"

#include "npapi-cxx/plugin_instance.h"

namespace npapi {

ScriptObjectBase::ScriptObjectBase(NPP npp)
    : npp_(npp) {
}

ScriptObjectBase::~ScriptObjectBase() {
}

PluginInstance* ScriptObjectBase::plugin_instance() {
  return PluginInstance::from_npp(npp_);
}

void ScriptObjectBase::Invalidate() {
  npp_ = NULL;
}

bool ScriptObjectBase::HasMethod(NPIdentifier name) {
  return false;
}

bool ScriptObjectBase::Invoke(NPIdentifier name,
                              const NPVariant *args,
                              uint32_t arg_count,
                              NPVariant *result) {
  return false;
}

bool ScriptObjectBase::InvokeDefault(const NPVariant *args,
                                     uint32_t arg_count,
                                     NPVariant *result) {
    return false;
}

bool ScriptObjectBase::HasProperty(NPIdentifier name) {
  return false;
}

bool ScriptObjectBase::GetProperty(NPIdentifier name, NPVariant *result) {
  return false;
}

bool ScriptObjectBase::SetProperty(NPIdentifier name, const NPVariant *value) {
    return false;
}

bool ScriptObjectBase::RemoveProperty(NPIdentifier name) {
    return false;
}

bool ScriptObjectBase::Enumerate(NPIdentifier **identifiers,
                                 uint32_t *identifier_count) {
    return false;
}

bool ScriptObjectBase::Construct(const NPVariant *args, uint32_t arg_count,
                                 NPVariant *result) {
    return false;
}

// static
void ScriptObjectBase::DeallocateThunk(NPObject *npobj) {
  delete static_cast<ScriptObjectBase*>(npobj);
}

// static
void ScriptObjectBase::InvalidateThunk(NPObject *npobj) {
  static_cast<ScriptObjectBase*>(npobj)->Invalidate();
}

// static
bool ScriptObjectBase::HasMethodThunk(NPObject *npobj, NPIdentifier name) {
  return static_cast<ScriptObjectBase*>(npobj)->HasMethod(name);
}

// static
bool ScriptObjectBase::InvokeThunk(
    NPObject *npobj, NPIdentifier name,
    const NPVariant *args, uint32_t arg_count, NPVariant *result) {
  return static_cast<ScriptObjectBase*>(npobj)->Invoke(name, args, arg_count,
                                                       result);
}

// static
bool ScriptObjectBase::InvokeDefaultThunk(
    NPObject *npobj, const NPVariant *args, uint32_t arg_count,
    NPVariant *result) {
  return static_cast<ScriptObjectBase*>(npobj)->InvokeDefault(
      args, arg_count, result);
}

// static
bool ScriptObjectBase::HasPropertyThunk(NPObject *npobj, NPIdentifier name) {
  return static_cast<ScriptObjectBase*>(npobj)->HasProperty(name);
}

// static
bool ScriptObjectBase::GetPropertyThunk(NPObject *npobj,
                                        NPIdentifier name,
                                        NPVariant *result) {
  return static_cast<ScriptObjectBase*>(npobj)->GetProperty(name, result);
}

// static
bool ScriptObjectBase::SetPropertyThunk(NPObject *npobj,
                                        NPIdentifier name,
                                        const NPVariant *value) {
  return static_cast<ScriptObjectBase*>(npobj)->SetProperty(name, value);
}

// static
bool ScriptObjectBase::RemovePropertyThunk(NPObject *npobj, NPIdentifier name) {
  return static_cast<ScriptObjectBase*>(npobj)->RemoveProperty(name);
}

// static
bool ScriptObjectBase::EnumerateThunk(NPObject *npobj,
                                      NPIdentifier **identifiers,
                                      uint32_t *identifier_count) {
  return static_cast<ScriptObjectBase*>(npobj)->Enumerate(identifiers,
                                                          identifier_count);
}

// static
bool ScriptObjectBase::ConstructThunk(NPObject *npobj,
                                      const NPVariant *args, uint32_t arg_count,
                                      NPVariant *result) {
  return static_cast<ScriptObjectBase*>(npobj)->Construct(args, arg_count, result);
}

}  // namespace npapi
