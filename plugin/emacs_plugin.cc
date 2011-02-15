// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#include "plugin.h"

#include "emacs_instance.h"

namespace {

class EmacsPlugin : public Plugin {
  public:
    NPError init() {
        return NPERR_NO_ERROR;
    }

    const char* getMIMEDescription() {
        return "application/x-emacs-npapi::Embed emacs with NPAPI";
    }

    PluginInstance* createInstance(NPMIMEType pluginType, NPP npp,
                                   uint16_t mode,
                                   int16_t argc,
                                   char *argn[], char *argv[],
                                   NPSavedData* saved,
                                   NPError* error) {
        // TODO
    }

};

}  // namespace

Plugin* Plugin::createPlugin() {
    return new EmacsPlugin();
}
