// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#include "script_object.h"

#include "plugin_instance.h"

ScriptObjectBase::ScriptObjectBase(NPP npp)
        : npp_(npp) {
}

ScriptObjectBase::~ScriptObjectBase() {
}

PluginInstance* ScriptObjectBase::pluginInstance() {
    return PluginInstance::fromNPP(npp_);
}

void ScriptObjectBase::invalidate() {
    npp_ = NULL;
}

bool ScriptObjectBase::hasMethod(NPIdentifier name) {
    return false;
}

bool ScriptObjectBase::invoke(NPIdentifier name,
                          const NPVariant *args,
                          uint32_t argCount,
                          NPVariant *result) {
    return false;
}

bool ScriptObjectBase::invokeDefault(const NPVariant *args,
                                 uint32_t argCount,
                                 NPVariant *result) {
    return false;
}

bool ScriptObjectBase::hasProperty(NPIdentifier name) {
    return false;
}

bool ScriptObjectBase::getProperty(NPIdentifier name, NPVariant *result) {
    return false;
}

bool ScriptObjectBase::setProperty(NPIdentifier name, const NPVariant *value) {
    return false;
}

bool ScriptObjectBase::removeProperty(NPIdentifier name) {
    return false;
}

bool ScriptObjectBase::enumerate(NPIdentifier **identifiers,
                             uint32_t *identifierCount) {
    return false;
}

bool ScriptObjectBase::construct(const NPVariant *args, uint32_t argCount,
                             NPVariant *result) {
    return false;
}

// static
void ScriptObjectBase::deallocateThunk(NPObject *npobj) {
    delete static_cast<ScriptObjectBase*>(npobj);
}

// static
void ScriptObjectBase::invalidateThunk(NPObject *npobj) {
    static_cast<ScriptObjectBase*>(npobj)->invalidate();
}

// static
bool ScriptObjectBase::hasMethodThunk(NPObject *npobj, NPIdentifier name) {
    return static_cast<ScriptObjectBase*>(npobj)->hasMethod(name);
}

// static
bool ScriptObjectBase::invokeThunk(NPObject *npobj, NPIdentifier name,
                               const NPVariant *args, uint32_t argCount,
                               NPVariant *result) {
    return static_cast<ScriptObjectBase*>(npobj)->invoke(name, args, argCount,
                                                         result);
}

// static
bool ScriptObjectBase::invokeDefaultThunk(NPObject *npobj,
                                          const NPVariant *args, uint32_t argCount,
                                          NPVariant *result) {
    return static_cast<ScriptObjectBase*>(npobj)->invokeDefault(
        args, argCount, result);
}

// static
bool ScriptObjectBase::hasPropertyThunk(NPObject *npobj, NPIdentifier name) {
    return static_cast<ScriptObjectBase*>(npobj)->hasProperty(name);
}

// static
bool ScriptObjectBase::getPropertyThunk(NPObject *npobj,
                                        NPIdentifier name,
                                        NPVariant *result) {
    return static_cast<ScriptObjectBase*>(npobj)->getProperty(name, result);
}

// static
bool ScriptObjectBase::setPropertyThunk(NPObject *npobj,
                                        NPIdentifier name,
                                        const NPVariant *value) {
    return static_cast<ScriptObjectBase*>(npobj)->setProperty(name, value);
}

// static
bool ScriptObjectBase::removePropertyThunk(NPObject *npobj, NPIdentifier name) {
    return static_cast<ScriptObjectBase*>(npobj)->removeProperty(name);
}

// static
bool ScriptObjectBase::enumerateThunk(NPObject *npobj, NPIdentifier **identifiers,
                                  uint32_t *identifierCount) {
    return static_cast<ScriptObjectBase*>(npobj)->enumerate(identifiers,
                                                        identifierCount);
}

// static
bool ScriptObjectBase::constructThunk(NPObject *npobj,
                                  const NPVariant *args, uint32_t argCount,
                                  NPVariant *result) {
    return static_cast<ScriptObjectBase*>(npobj)->construct(args, argCount, result);
}
