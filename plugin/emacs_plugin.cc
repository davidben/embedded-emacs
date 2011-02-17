// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.


#include "identifiers.h"
#include "emacs_instance.h"
#include "npapi-cxx/browser.h"
#include "npapi-cxx/plugin.h"
#include "process_watcher.h"

namespace {

class EmacsPlugin : public npapi::Plugin {
  public:
    ~EmacsPlugin() {
        process_watcher::killAndJoinThread();
    }

    NPError init() {
        int major_version, minor_version;
        NPN_Version(NULL, NULL, &major_version, &minor_version);

        // Require NPRuntime.
        if (minor_version < NPVERS_HAS_NPRUNTIME_SCRIPTING)
            return NPERR_INCOMPATIBLE_VERSION_ERROR;

        // Require sane threading.
        if (minor_version < NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL)
            return NPERR_INCOMPATIBLE_VERSION_ERROR;

        // Require XEmbed support.
        NPBool has_xembed = false;
        NPError err = NPN_GetValue(NULL, NPNVSupportsXEmbedBool, &has_xembed);
        if (err != NPERR_NO_ERROR || !has_xembed)
            return NPERR_INCOMPATIBLE_VERSION_ERROR;

        initializeIdentifiers();

        return NPERR_NO_ERROR;
    }

    const char* getMIMEDescription() {
        return "application/x-emacs-npapi::Embed emacs with NPAPI";
    }

    NPError getValue(NPPVariable variable, void *value) {
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
                err = Plugin::getValue(variable, value);
        }
        return err;
    }

    npapi::PluginInstance* createInstance(NPMIMEType pluginType, NPP npp,
                                          uint16_t mode,
                                          int16_t argc,
                                          char *argn[], char *argv[],
                                          NPSavedData* saved,
                                          NPError* error) {
        // TODO: Pass some of these arguments in??
        return new EmacsInstance(npp);
    }

};

}  // namespace

namespace npapi {
Plugin* Plugin::createPlugin() {
    return new EmacsPlugin();
}
}  // namespace npapi
