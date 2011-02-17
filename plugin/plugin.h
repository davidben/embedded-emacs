// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_PLUGIN_H_
#define INCLUDED_PLUGIN_H_

#include "npapi-headers/npapi.h"

namespace npapi {

class PluginInstance;

class Plugin {
  public:
    // Constructors and destructors should be cheap. It seems that
    // Chrome will call NP_GetMIMEDescription before NP_Initialize
    // when probing, so it probably never even calls the latter. TODO:
    // Check if this is the case and if this means dtor needs to work
    // without init being called.
    Plugin();
    virtual ~Plugin();

    // This stub is for the plugin implementer to implement.
    static Plugin* createPlugin();

    static Plugin* get();

    // Finish initializing the plugin; notably this bit can return an
    // error code.
    virtual NPError init();

    virtual NPError getValue(NPPVariable variable, void *value);
    // NOTE: This function must be safe to call before init() is
    // called.
    virtual const char* getMIMEDescription() = 0;

    virtual PluginInstance* createInstance(NPMIMEType pluginType, NPP npp,
                                           uint16_t mode,
                                           int16_t argc,
                                           char *argn[], char *argv[],
                                           NPSavedData* saved,
                                           NPError* error) = 0;
};

}  // namespace npapi

#endif  // INCLUDED_PLUGIN_H_
