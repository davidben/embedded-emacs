// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#include "emacs_manager.h"
#include "npapi-cxx/browser.h"
#include "npapi-cxx/plugin.h"

namespace {

class EmacsPlugin : public npapi::Plugin {
 public:
  NPError Init() {
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

    return NPERR_NO_ERROR;
  }

  const char* GetMIMEDescription() {
    return "application/x-embedded-emacs-launcher::Embed emacs with NPAPI";
  }

  NPError GetValue(NPPVariable variable, void *value) {
    NPError err = NPERR_NO_ERROR;
    switch (variable) {
      case NPPVpluginNameString:
        *reinterpret_cast<const char **>(value) =
            "Embedded Emacs - Launcher";
        break;
      case NPPVpluginDescriptionString:
        *reinterpret_cast<const char **>(value) =
            "Private half of embedded emacs; takes a window id"
            " and launches your editor on behalf of the renderer.";
        break;
      default:
        err = Plugin::GetValue(variable, value);
    }
    return err;
  }

  npapi::PluginInstance* CreateInstance(NPMIMEType plugin_type, NPP npp,
                                        uint16_t mode,
                                        int16_t argc,
                                        char *argn[], char *argv[],
                                        NPSavedData* saved,
                                        NPError* error) {
    // TODO: Pass some of these arguments in??
    return new EmacsManager(npp);
  }

};

}  // namespace

namespace npapi {

Plugin* Plugin::CreatePlugin() {
  return new EmacsPlugin();
}

}  // namespace npapi
