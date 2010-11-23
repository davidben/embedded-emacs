// Copyright (c) 2010 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#include "browser.h"

#include <algorithm>
#include <cstring>

#include "npapi-headers/npfunctions.h"

namespace {

NPNetscapeFuncs g_browser_functions = { 0 };

}  // namespace

NPError initializeBrowserFuncs(NPNetscapeFuncs* bFuncs)
{
    if (!bFuncs)
        return NPERR_INVALID_FUNCTABLE_ERROR;

    int major_version = bFuncs->version >> 8;
    int minor_version = bFuncs->version & 0xff;

    if (major_version != NP_VERSION_MAJOR)
        return NPERR_INCOMPATIBLE_VERSION_ERROR;

    // Require NPRuntime.
    if (minor_version < NPVERS_HAS_NPRUNTIME_SCRIPTING)
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    // Require sane threading.
    if (minor_version < NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL)
        return NPERR_INCOMPATIBLE_VERSION_ERROR;

    memcpy(&g_browser_functions, bFuncs,
           std::min(sizeof(g_browser_functions), (size_t)bFuncs->size));

    return NPERR_NO_ERROR;
}

NPError NP_LOADDS NPN_GetValue(NPP instance, NPNVariable variable,
                               void *value)
{
    if (!g_browser_functions.getvalue)
        return NPERR_INVALID_FUNCTABLE_ERROR;
    return g_browser_functions.getvalue(instance, variable, value);
}

void* NP_LOADDS NPN_MemAlloc(uint32_t size)
{
    if (!g_browser_functions.memalloc)
        return NULL;
    return g_browser_functions.memalloc(size);
}

NPObject *NPN_CreateObject(NPP npp, NPClass *aClass)
{
    if (!g_browser_functions.createobject)
        return NULL;
    return g_browser_functions.createobject(npp, aClass);
}

NPObject *NPN_RetainObject(NPObject *npobj)
{
    if (!g_browser_functions.retainobject)
        return NULL;
    return g_browser_functions.retainobject(npobj);
}

void NPN_ReleaseObject(NPObject *npobj)
{
    if (!g_browser_functions.releaseobject)
        return;
    g_browser_functions.releaseobject(npobj);
}

void NPN_GetStringIdentifiers(const NPUTF8 **names,
                              int32_t nameCount,
                              NPIdentifier *identifiers)
{
    if (!g_browser_functions.getstringidentifiers)
        return;
    g_browser_functions.getstringidentifiers(names, nameCount, identifiers);
}

bool NPN_InvokeDefault(NPP npp, NPObject *npobj, const NPVariant *args,
                       uint32_t argCount, NPVariant *result)
{
    if (!g_browser_functions.invokeDefault)
        return false;
    return g_browser_functions.invokeDefault(npp, npobj, args, argCount, result);
}

void NPN_ReleaseVariantValue(NPVariant *variant)
{
    if (!g_browser_functions.releasevariantvalue)
        return;
    g_browser_functions.releasevariantvalue(variant);
}

void NPN_SetException(NPObject *npobj, const NPUTF8 *message)
{
    if (!g_browser_functions.setexception)
        return;
    g_browser_functions.setexception(npobj, message);
}

void NPN_PluginThreadAsyncCall(NPP plugin,
                               void (*func)(void *),
                               void *userData)
{
    // FIXME: This will get called on another thread. Lock this guy?
    if (!g_browser_functions.pluginthreadasynccall)
        return;
    g_browser_functions.pluginthreadasynccall(plugin, func, userData);
}
