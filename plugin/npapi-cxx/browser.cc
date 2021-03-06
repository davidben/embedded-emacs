// Copyright (c) 2010 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#include "npapi-cxx/browser.h"

#include <algorithm>
#include <cstring>

#include "npapi-headers/npfunctions.h"

namespace {

NPNetscapeFuncs g_browser_functions = { 0 };

}  // namespace

namespace npapi {

NPError InitializeBrowserFuncs(NPNetscapeFuncs* bFuncs) {
    if (!bFuncs)
        return NPERR_INVALID_FUNCTABLE_ERROR;

    int major_version = bFuncs->version >> 8;

    if (major_version != NP_VERSION_MAJOR)
        return NPERR_INCOMPATIBLE_VERSION_ERROR;

    memcpy(&g_browser_functions, bFuncs,
           std::min(sizeof(g_browser_functions), (size_t)bFuncs->size));

    return NPERR_NO_ERROR;
}

}  // namespace npapi

void NPN_Version(int* plugin_major, int* plugin_minor,
                 int* netscape_major, int* netscape_minor) {
    if (netscape_major)
        *netscape_major = g_browser_functions.version >> 8;
    if (netscape_minor)
        *netscape_minor = g_browser_functions.version & 0xff;

    if (plugin_major)
        *plugin_major = NP_VERSION_MAJOR;
    if (plugin_minor)
        *plugin_minor = NP_VERSION_MINOR;
}

NPError NPN_GetValue(NPP instance, NPNVariable variable, void *value) {
    if (!g_browser_functions.getvalue)
        return NPERR_INVALID_FUNCTABLE_ERROR;
    return g_browser_functions.getvalue(instance, variable, value);
}

NPError NPN_SetValue(NPP instance, NPPVariable variable, void *value) {
    if (!g_browser_functions.setvalue)
        return NPERR_INVALID_FUNCTABLE_ERROR;
    return g_browser_functions.setvalue(instance, variable, value);
}

void* NPN_MemAlloc(uint32_t size) {
    if (!g_browser_functions.memalloc)
        return NULL;
    return g_browser_functions.memalloc(size);
}

NPObject *NPN_CreateObject(NPP npp, NPClass *aClass) {
    if (!g_browser_functions.createobject)
        return NULL;
    return g_browser_functions.createobject(npp, aClass);
}

NPObject *NPN_RetainObject(NPObject *npobj) {
    if (!g_browser_functions.retainobject)
        return NULL;
    return g_browser_functions.retainobject(npobj);
}

void NPN_ReleaseObject(NPObject *npobj) {
    if (!g_browser_functions.releaseobject)
        return;
    g_browser_functions.releaseobject(npobj);
}

NPIdentifier NPN_GetStringIdentifier(const NPUTF8 *name) {
    if (!g_browser_functions.getstringidentifier)
        return 0;
    return g_browser_functions.getstringidentifier(name);
}

void NPN_GetStringIdentifiers(const NPUTF8 **names,
                              int32_t nameCount,
                              NPIdentifier *identifiers) {
    if (!g_browser_functions.getstringidentifiers)
        return;
    g_browser_functions.getstringidentifiers(names, nameCount, identifiers);
}

bool NPN_InvokeDefault(NPP npp, NPObject *npobj, const NPVariant *args,
                       uint32_t argCount, NPVariant *result) {
    if (!g_browser_functions.invokeDefault)
        return false;
    return g_browser_functions.invokeDefault(npp, npobj, args, argCount, result);
}

void NPN_ReleaseVariantValue(NPVariant *variant) {
    if (!g_browser_functions.releasevariantvalue)
        return;
    g_browser_functions.releasevariantvalue(variant);
}

void NPN_SetException(NPObject *npobj, const NPUTF8 *message) {
    if (!g_browser_functions.setexception)
        return;
    g_browser_functions.setexception(npobj, message);
}

void NPN_PluginThreadAsyncCall(NPP plugin,
                               void (*func)(void *),
                               void *userData) {
    // FIXME: This will get called on another thread. Lock this guy?
    if (!g_browser_functions.pluginthreadasynccall)
        return;
    g_browser_functions.pluginthreadasynccall(plugin, func, userData);
}
