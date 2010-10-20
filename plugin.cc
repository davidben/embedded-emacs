#define XP_UNIX 1
#include "npapi-headers/npapi.h"
#include "npapi-headers/npfunctions.h"

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
    NPError err;

    // TODO: save bFuncs

    // On Unix, it looks like NP_GetEntryPoints isn't called directly?
    // The prototype for NP_Initialize is different from everyone
    // else. Meh.
    return NP_GetEntryPoints(pFuncs);
}
