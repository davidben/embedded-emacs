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
    return g_browser_functions.releaseobject(npobj);
}
