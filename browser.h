#ifndef INCLUDED_BROWSER_H_
#define INCLUDED_BROWSER_H_

// Provide the NPN_* prototypes.
#ifndef XP_UNIX
#define XP_UNIX 1
#endif
#include "npapi-headers/npapi.h"

typedef struct _NPNetscapeFuncs NPNetscapeFuncs;

NPError initializeBrowserFuncs(NPNetscapeFuncs* bFuncs);

#endif  // INCLUDED_BROWSER_H_
