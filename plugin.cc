#include "plugin.h"

#include <algorithm>
#include <cstring>

#include "npapi-headers/npfunctions.h"

NPNetscapeFuncs g_browser_functions = { 0 };

namespace {

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

}  // namespace

NP_EXPORT(NPError) OSCALL NP_GetEntryPoints(NPPluginFuncs* pFuncs)
{
    pFuncs->size = sizeof(*pFuncs);
    pFuncs->version = 0; // My version?
    // TODO: implement the functions I need.
    return NPERR_NO_ERROR;
}

NP_EXPORT(NPError) OSCALL NP_Initialize(NPNetscapeFuncs* bFuncs,
					NPPluginFuncs* pFuncs)
{
    NPError err = initializeBrowserFuncs(bFuncs);
    if (err != NPERR_NO_ERROR)
        return err;

    // On Unix, it looks like NP_GetEntryPoints isn't called directly?
    // The prototype for NP_Initialize is different from everyone
    // else. Meh.
    return NP_GetEntryPoints(pFuncs);
}

NP_EXPORT(NPError) OSCALL NP_GetValue(void *instance,
                                      NPPVariable variable,
                                      void *value)
{
    NPError err = NPERR_NO_ERROR;
    switch (variable) {
        case NPPVpluginNameString:
            *reinterpret_cast<const char **>(value) = "Embedded Emacs";
            break;
        case NPPVpluginDescriptionString:
            *reinterpret_cast<const char **>(value) =
                    "Embeds emacs into your browser window with XEmbed.";
            break;
        default:
            err = NPERR_GENERIC_ERROR;
    }
    return err;
}

// This probably should be const char *, but NPAPI messed up.
NP_EXPORT(char*) OSCALL NP_GetMIMEDescription(void)
{
    return const_cast<char*>("application/x-emacs-npapi::Embed emacs with NPAPI");
}