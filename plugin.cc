#include "plugin.h"

#include <prtypes.h>

#include <algorithm>
#include <cstring>

#include "emacsinstance.h"
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
    pFuncs->version = (NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR;
    pFuncs->newp = NPP_New;
    pFuncs->destroy = NPP_Destroy;
    pFuncs->setwindow = NPP_SetWindow;
    pFuncs->getvalue = NPP_GetValue;
    // TODO: implement the functions I need.
    return NPERR_NO_ERROR;
}

NP_EXPORT(NPError) OSCALL NP_Initialize(NPNetscapeFuncs* bFuncs,
					NPPluginFuncs* pFuncs)
{
    NPError err = initializeBrowserFuncs(bFuncs);
    if (err != NPERR_NO_ERROR)
        return err;

    // Require XEmbed support.
    PRBool has_xembed = PR_FALSE;
    if (!g_browser_functions.getvalue)
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    err = g_browser_functions.getvalue(NULL, NPNVSupportsXEmbedBool, &has_xembed);
    if (err != NPERR_NO_ERROR || !has_xembed)
        return NPERR_INCOMPATIBLE_VERSION_ERROR;

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
            err = NPERR_INVALID_PARAM;
    }
    return err;
}

NP_EXPORT(NPError) OSCALL NP_Shutdown(void)
{
    return NPERR_NO_ERROR;
}

// This probably should be const char *, but NPAPI messed up.
NP_EXPORT(char*) OSCALL NP_GetMIMEDescription(void)
{
    return const_cast<char*>("application/x-emacs-npapi::Embed emacs with NPAPI");
}

/*****************************/
/* NPP Functions             */
/*****************************/

NPError NPP_New(NPMIMEType pluginType, NPP instance,
                uint16_t mode, int16_t argc, char* argn[],
                char* argv[], NPSavedData* saved)
{
    // TODO: Pass some of these arguments in??
    EmacsInstance* emacs = new EmacsInstance();
    instance->pdata = emacs;
    return NPERR_NO_ERROR;
}

NPError NPP_Destroy(NPP instance, NPSavedData** save)
{
    if (!instance->pdata)
        return NPERR_INVALID_INSTANCE_ERROR;
    delete static_cast<EmacsInstance*>(instance->pdata);
    instance->pdata = NULL;
    return NPERR_NO_ERROR;
}

NPError NPP_SetWindow(NPP instance, NPWindow* window)
{
    if (!instance->pdata)
        return NPERR_INVALID_INSTANCE_ERROR;
    EmacsInstance* emacs = static_cast<EmacsInstance*>(instance->pdata);
    return emacs->setWindow(window);
}

NPError NPP_GetValue(NPP instance, NPPVariable variable, void* value)
{
    NPError err = NPERR_NO_ERROR;
    switch (variable) {
        case NPPVpluginNeedsXEmbed:
            *reinterpret_cast<PRBool*>(value) = PR_TRUE;
            break;
        default:
            err = NPERR_INVALID_PARAM;
    }
    return err;
}
