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
    NPError init();

    const char* getMIMEDescription();
    NPError getValue(NPPVariable variable, void *value);
    npapi::PluginInstance* createInstance(NPMIMEType pluginType, NPP npp,
                                          uint16_t mode,
                                          int16_t argc,
                                          char *argn[], char *argv[],
                                          NPSavedData* saved,
                                          NPError* error);
};

class XEmbedContainerInstance : public npapi::PluginInstance {
  public:
    XEmbedContainerInstance(NPP npp);

    long windowId() { return window_id_; }

    NPError setWindow(NPWindow* window);
    NPError getValue(NPPVariable variable, void* value);
  private:
    long window_id_;
    npapi::scoped_npobject<XEmbedContainerObject> script_object_;
};

class XEmbedContainerObject : public npapi::ScriptObject<XEmbedContainerObject> {
  public:
    XEmbedContainerInstance* pluginInstance();
    bool hasProperty(NPIdentifier name);
    bool getProperty(NPIdentifier name, NPVariant *result);
  private:
    friend class npapi::ScriptObject<XEmbedContainerObject>;
    XEmbedContainerObject(NPP npp);
    ~XEmbedContainerObject();
};

//////////////////////////////////////
// XEmbedContainerPlugin
//////////////////////////////////////

NPError XEmbedContainerPlugin::init() {
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

const char* XEmbedContainerPlugin::getMIMEDescription() {
    return "application/x-embedded-emacs-container:"
            ":"
            "XEmbed container (Embedded Emacs)";
}

NPError XEmbedContainerPlugin::getValue(NPPVariable variable, void *value) {
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
            err = Plugin::getValue(variable, value);
    }
    return err;
}

npapi::PluginInstance* XEmbedContainerPlugin::createInstance(
    NPMIMEType pluginType, NPP npp, uint16_t mode,
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

NPError XEmbedContainerInstance::setWindow(NPWindow* window) {
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

NPError XEmbedContainerInstance::getValue(NPPVariable variable, void* value) {
    NPError err = NPERR_NO_ERROR;
    switch (variable) {
        case NPPVpluginNeedsXEmbed:
            *reinterpret_cast<NPBool*>(value) = true;
            break;
        case NPPVpluginScriptableNPObject: {
            if (!script_object_.get()) {
                script_object_.reset(XEmbedContainerObject::create(npp()));
            }
            *reinterpret_cast<NPObject**>(value) =
                    static_cast<NPObject*>(script_object_.get());
            break;
        }
        default:
            err = PluginInstance::getValue(variable, value);
    }
    return err;
}

//////////////////////////////////////
// XEmbedContainerObject
//////////////////////////////////////

XEmbedContainerInstance* XEmbedContainerObject::pluginInstance() {
    return static_cast<XEmbedContainerInstance*>(
        npapi::ScriptObject<XEmbedContainerObject>::pluginInstance());
}

bool XEmbedContainerObject::hasProperty(NPIdentifier name) {
    return (name == g_identifier_windowId);
}

bool XEmbedContainerObject::getProperty(NPIdentifier name, NPVariant *result) {
    if (!hasProperty(name))
        return false;
    if (!pluginInstance())
        return false;
    INT32_TO_NPVARIANT(pluginInstance()->windowId(), *result);
    return true;
}

XEmbedContainerObject::XEmbedContainerObject(NPP npp)
    : npapi::ScriptObject<XEmbedContainerObject>(npp) {
}

XEmbedContainerObject::~XEmbedContainerObject() {
}

}  // namespace

namespace npapi {
Plugin* Plugin::createPlugin() {
    return new XEmbedContainerPlugin();
}
}  // namespace npapi
