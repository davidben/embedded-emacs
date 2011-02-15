// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_PLUGIN_H_
#define INCLUDED_PLUGIN_H_

#include "npapi-headers/npapi.h"

class PluginInstance;

class Plugin {
  public:
    Plugin();
    virtual ~Plugin();

    // This stub is for the plugin implementer to implement.
    static Plugin* createPlugin();

    static Plugin* get();

    // Finish initializing the plugin; notably this bit can return an
    // error code.
    virtual NPError init();

    virtual NPError getValue(NPPVariable variable, void *value);
    virtual const char* getMIMEDescription() = 0;

    virtual PluginInstance* createInstance(NPMIMEType pluginType, NPP npp,
                                           uint16_t mode,
                                           int16_t argc,
                                           char *argn[], char *argv[],
                                           NPSavedData* saved,
                                           NPError* error) = 0;
};
#endif  // INCLUDED_PLUGIN_H_
