// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#include "npapi-cxx/browser.h"
#include "npapi-cxx/plugin.h"
#include "npapi-cxx/plugin_instance.h"
#include "npapi-cxx/script_object.h"
#include "npapi-cxx/scoped_npobject.h"
#include "util.h"

namespace {

NPIdentifier g_identifier_windowId = 0;

class XEmbedContainerPlugin;
class XEmbedContainerInstance;
class XEmbedContainerObject;

class XEmbedContainerPlugin : public npapi::Plugin {
 public:
  NPError Init();

  const char* GetMIMEDescription();
  NPError GetValue(NPPVariable variable, void *value);
  npapi::PluginInstance* CreateInstance(NPMIMEType plugin_type, NPP npp,
                                        uint16_t mode,
                                        int16_t argc,
                                        char *argn[], char *argv[],
                                        NPSavedData* saved,
                                        NPError* error);
};

class XEmbedContainerInstance : public npapi::PluginInstance {
 public:
  XEmbedContainerInstance(NPP npp);

  long window_id() { return window_id_; }

  NPError SetWindow(NPWindow* window);
  NPError GetValue(NPPVariable variable, void* value);
 private:
  long window_id_;
  npapi::scoped_npobject<XEmbedContainerObject> script_object_;
};

class XEmbedContainerObject : public npapi::ScriptObject<XEmbedContainerObject> {
 public:
  XEmbedContainerInstance* plugin_instance();
  bool HasProperty(NPIdentifier name);
  bool GetProperty(NPIdentifier name, NPVariant *result);
  bool Enumerate(NPIdentifier **identifiers, uint32_t *identifier_count);
 private:
  friend class npapi::ScriptObject<XEmbedContainerObject>;
  XEmbedContainerObject(NPP npp);
  ~XEmbedContainerObject();
};

//////////////////////////////////////
// XEmbedContainerPlugin
//////////////////////////////////////

NPError XEmbedContainerPlugin::Init() {
  int major_version, minor_version;
  NPN_Version(NULL, NULL, &major_version, &minor_version);

  // Require NPRuntime.
  if (minor_version < NPVERS_HAS_NPRUNTIME_SCRIPTING)
    return NPERR_INCOMPATIBLE_VERSION_ERROR;

  // Require XEmbed support.
  NPBool has_xembed = false;
  NPError err = NPN_GetValue(NULL, NPNVSupportsXEmbedBool, &has_xembed);
  if (err != NPERR_NO_ERROR || !has_xembed)
    return NPERR_INCOMPATIBLE_VERSION_ERROR;

  // Initialize one identifier I care about.
  g_identifier_windowId = NPN_GetStringIdentifier("windowId");

  return NPERR_NO_ERROR;
}

const char* XEmbedContainerPlugin::GetMIMEDescription() {
  return "application/x-embedded-emacs-container:"
      ":"
      "XEmbed container (Embedded Emacs)";
}

NPError XEmbedContainerPlugin::GetValue(NPPVariable variable, void *value) {
  NPError err = NPERR_NO_ERROR;
  switch (variable) {
    case NPPVpluginNameString:
      *reinterpret_cast<const char **>(value) =
          "Embedded Emacs - XEmbed container";
      break;
    case NPPVpluginDescriptionString:
      *reinterpret_cast<const char **>(value) =
          "Dummy plugin that exports X11 window id for a"
          " trusted plugin to embed Emacs into.";
      break;
    default:
      err = Plugin::GetValue(variable, value);
  }
  return err;
}

npapi::PluginInstance* XEmbedContainerPlugin::CreateInstance(
    NPMIMEType plugin_type, NPP npp, uint16_t mode,
    int16_t argc, char *argn[], char *argv[],
    NPSavedData* saved,
    NPError* error) {
  // TODO: Pass some of these arguments in??
  return new XEmbedContainerInstance(npp);
}


//////////////////////////////////////
// XEmbedContainerInstance
//////////////////////////////////////

XEmbedContainerInstance::XEmbedContainerInstance(NPP npp)
    : PluginInstance(npp),
      window_id_(0) {
}

NPError XEmbedContainerInstance::SetWindow(NPWindow* window) {
  long window_id = reinterpret_cast<long>(window->window);
  if (!window_id_) {
    window_id_ = window_id;
  } else {
    // This really shouldn't happen.
    if (window_id != window_id_)
      return NPERR_GENERIC_ERROR;
  }
  return NPERR_NO_ERROR;
}

NPError XEmbedContainerInstance::GetValue(NPPVariable variable, void* value) {
  NPError err = NPERR_NO_ERROR;
  switch (variable) {
    case NPPVpluginNeedsXEmbed:
      *reinterpret_cast<NPBool*>(value) = true;
      break;
    case NPPVpluginScriptableNPObject: {
      if (!script_object_.get()) {
        script_object_.reset(XEmbedContainerObject::Create(npp()));
      }
      *reinterpret_cast<NPObject**>(value) =
          static_cast<NPObject*>(script_object_.get());
      break;
    }
    default:
      err = PluginInstance::GetValue(variable, value);
  }
  return err;
}

//////////////////////////////////////
// XEmbedContainerObject
//////////////////////////////////////

XEmbedContainerInstance* XEmbedContainerObject::plugin_instance() {
  return static_cast<XEmbedContainerInstance*>(
      npapi::ScriptObject<XEmbedContainerObject>::plugin_instance());
}

bool XEmbedContainerObject::HasProperty(NPIdentifier name) {
  return (name == g_identifier_windowId);
}

bool XEmbedContainerObject::GetProperty(NPIdentifier name, NPVariant *result) {
  if (!HasProperty(name))
    return false;
  if (!plugin_instance())
    return false;
  INT32_TO_NPVARIANT(plugin_instance()->window_id(), *result);
  return true;
}

bool XEmbedContainerObject::Enumerate(NPIdentifier **identifiers,
                                      uint32_t *identifier_count) {
  NPIdentifier* properties = static_cast<NPIdentifier*>(
      NPN_MemAlloc(sizeof(NPIdentifier)));
  if (!properties) return false;

  properties[0] = g_identifier_windowId;

  *identifiers = properties;
  *identifier_count = 1;
  return true;
}

XEmbedContainerObject::XEmbedContainerObject(NPP npp)
    : npapi::ScriptObject<XEmbedContainerObject>(npp) {
}

XEmbedContainerObject::~XEmbedContainerObject() {
}

}  // namespace

namespace npapi {
Plugin* Plugin::CreatePlugin() {
  return new XEmbedContainerPlugin();
}
}  // namespace npapi
