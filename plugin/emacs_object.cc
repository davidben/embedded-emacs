// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#include "emacs_object.h"

#include "emacs_instance.h"
#include "identifiers.h"

EmacsObject::EmacsObject(NPP npp) : npapi::ScriptObject<EmacsObject>(npp) {
}

EmacsObject::~EmacsObject() {
}

EmacsInstance* EmacsObject::emacsInstance() {
    return static_cast<EmacsInstance*>(pluginInstance());
}

bool EmacsObject::hasMethod(NPIdentifier name) {
    return (name == identifier::startEditor() ||
            name == identifier::setCallback() ||
            name == identifier::setInitialText() ||
            name == identifier::setEditorCommand());
}

bool EmacsObject::invoke(NPIdentifier name,
                          const NPVariant *args,
                          uint32_t argCount,
                          NPVariant *result) {
    if (!hasMethod(name))
        return false;

    EmacsInstance* emacs = emacsInstance();
    if (!emacs) {
        // TODO: raise an exception if the emacs is missing?
        VOID_TO_NPVARIANT(*result);
        return true;
    }

    if (name == identifier::startEditor()) {
        std::string error;
        if (!emacs->startEditor(&error))
            NPN_SetException(this, error.c_str());
        VOID_TO_NPVARIANT(*result);
        return true;
    } else if (name == identifier::setCallback()) {
        if (argCount < 1) {
            NPN_SetException(NULL, "setCallback takes one argument");
        } else if (NPVARIANT_IS_OBJECT(args[0])) {
            emacs->setCallback(NPVARIANT_TO_OBJECT(args[0]));
        } else {
            // Passed null, void, or some value. Throw the callback away.
            emacs->setCallback(NULL);
        }
        VOID_TO_NPVARIANT(*result);
        return true;
    } else if (name == identifier::setInitialText()) {
        if (argCount < 1) {
            NPN_SetException(this, "setInitialText takes one argument");
        } else if (!NPVARIANT_IS_STRING(args[0])) {
            NPN_SetException(this, "argument to setInitialText is not a string");
        } else {
            emacs->setInitialText(NPVARIANT_TO_STRING(args[0]).UTF8Characters,
                                  NPVARIANT_TO_STRING(args[0]).UTF8Length);
        }
        VOID_TO_NPVARIANT(*result);
        return true;
    } else if (name == identifier::setEditorCommand()) {
        if (argCount < 1) {
            NPN_SetException(this, "setEditorCommand takes one argument");
        } else if (!NPVARIANT_IS_STRING(args[0])) {
            NPN_SetException(
                this, "argument to setEditorCommand is not a string");
        } else {
            emacs->setEditorCommand(NPVARIANT_TO_STRING(args[0]).UTF8Characters,
                                    NPVARIANT_TO_STRING(args[0]).UTF8Length);
        }
        VOID_TO_NPVARIANT(*result);
        return true;
    }

    return false;
}

bool EmacsObject::enumerate(NPIdentifier **identifiers,
			    uint32_t *identifierCount) {
    const int NUM_PROPS = 3;
    NPIdentifier* properties = static_cast<NPIdentifier*>(
        NPN_MemAlloc(sizeof(NPIdentifier) * NUM_PROPS));
    if (!properties) return false;

    properties[0] = identifier::startEditor();
    properties[1] = identifier::setCallback();
    properties[2] = identifier::setInitialText();

    *identifiers = properties;
    *identifierCount = NUM_PROPS;
    return true;
}
