// Copyright (c) 2012 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#include "emacs_object.h"

#include "emacs_manager.h"
#include "identifiers.h"

EmacsObject::EmacsObject(NPP npp) : npapi::ScriptObject<EmacsObject>(npp) {
}

EmacsObject::~EmacsObject() {
}

EmacsManager* EmacsObject::emacsManager() {
    return static_cast<EmacsManager*>(pluginInstance());
}

bool EmacsObject::hasMethod(NPIdentifier name) {
    return (name == identifier::startEditor());
}

bool EmacsObject::invoke(NPIdentifier name,
                         const NPVariant *args,
                         uint32_t argCount,
                         NPVariant *result) {
    EmacsManager* emacs = emacsManager();
    if (!emacs) {
        // TODO: raise an exception if the emacs is missing?
        VOID_TO_NPVARIANT(*result);
        return true;
    }
    if (name == identifier::startEditor()) {
        if (argCount < 4) {
            NPN_SetException(this, "startEditor takes four arguments");
            return true;
        }
        int windowId;
        if (NPVARIANT_IS_INT32(args[0])) {
            windowId = NPVARIANT_TO_INT32(args[0]);
        } else if (NPVARIANT_IS_DOUBLE(args[0])) {
            // WHAT???
            windowId = static_cast<int>(NPVARIANT_TO_DOUBLE(args[0]));
        } else {
            NPN_SetException(this, "window argument is not a number");
            return true;
	}
        if (!NPVARIANT_IS_STRING(args[1])) {
            NPN_SetException(this, "command argument is not a string");
            return true;
        }
        if (!NPVARIANT_IS_STRING(args[2])) {
            NPN_SetException(this, "text argument is not a string");
            return true;
        }
        if (!NPVARIANT_IS_OBJECT(args[3])) {
            NPN_SetException(this, "callback argument is not an object");
            return true;
        }
        std::string error;
        std::string editorCommand = std::string(
            NPVARIANT_TO_STRING(args[1]).UTF8Characters,
            NPVARIANT_TO_STRING(args[1]).UTF8Length);
        int jobId = emacs->startEditor(
            windowId,
            editorCommand,
            NPVARIANT_TO_STRING(args[2]).UTF8Characters,
            NPVARIANT_TO_STRING(args[2]).UTF8Length,
            NPVARIANT_TO_OBJECT(args[3]),
            &error);
        if (jobId == 0)
            NPN_SetException(this, error.c_str());
        INT32_TO_NPVARIANT(jobId, *result);
	return true;
    }

    return false;
}

bool EmacsObject::enumerate(NPIdentifier **identifiers,
			    uint32_t *identifierCount) {
    const int NUM_PROPS = 1;
    NPIdentifier* properties = static_cast<NPIdentifier*>(
        NPN_MemAlloc(sizeof(NPIdentifier) * NUM_PROPS));
    if (!properties) return false;

    properties[0] = identifier::startEditor();

    *identifiers = properties;
    *identifierCount = NUM_PROPS;
    return true;
}
