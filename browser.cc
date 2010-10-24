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

    // TODO: Assert support in minor_version for features we care
    // about.

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
