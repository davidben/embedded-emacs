// Copyright (c) 2012 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#include "emacs_object.h"

#include "editor_type.h"
#include "emacs_manager.h"

EmacsObject::EmacsObject(NPP npp) : npapi::ScriptObject<EmacsObject>(npp) {
}

EmacsObject::~EmacsObject() {
}

EmacsManager* EmacsObject::emacs_manager() {
  return static_cast<EmacsManager*>(plugin_instance());
}

bool EmacsObject::HasMethod(NPIdentifier name) {
  return (name == NPN_GetStringIdentifier("startEditor"));
}

bool EmacsObject::Invoke(NPIdentifier name,
                         const NPVariant *args,
                         uint32_t arg_count,
                         NPVariant *result) {
  EmacsManager* emacs = emacs_manager();
  if (!emacs) {
    // TODO: raise an exception if the emacs is missing?
    VOID_TO_NPVARIANT(*result);
    return true;
  }
  if (name == NPN_GetStringIdentifier("startEditor")) {
    // Argument count.
    if (arg_count < 4) {
      NPN_SetException(this, "startEditor takes four arguments");
      return true;
    }

    // Window ID.
    int window_id;
    if (NPVARIANT_IS_INT32(args[0])) {
      window_id = NPVARIANT_TO_INT32(args[0]);
    } else if (NPVARIANT_IS_DOUBLE(args[0])) {
      // WHAT???
      window_id = static_cast<int>(NPVARIANT_TO_DOUBLE(args[0]));
    } else {
      NPN_SetException(this, "window argument is not a number");
      return true;
    }

    // Editor type.
    if (!NPVARIANT_IS_STRING(args[1])) {
      NPN_SetException(this, "editor argument is not a string");
      return true;
    }
    std::string editor_string = std::string(
        NPVARIANT_TO_STRING(args[1]).UTF8Characters,
        NPVARIANT_TO_STRING(args[1]).UTF8Length);
    EditorType editor = StringToEditorType(editor_string);
    if (editor == EDITOR_TYPE_INVALID) {
      NPN_SetException(this, "invalid editor type");
      return true;
    }

    // Initial text.
    if (!NPVARIANT_IS_STRING(args[2])) {
      NPN_SetException(this, "text argument is not a string");
      return true;
    }
    if (!NPVARIANT_IS_OBJECT(args[3])) {
      NPN_SetException(this, "callback argument is not an object");
      return true;
    }
    std::string initial_text = std::string(
        NPVARIANT_TO_STRING(args[2]).UTF8Characters,
        NPVARIANT_TO_STRING(args[2]).UTF8Length);

    // Finally run the function.
    std::string error;
    int job_id = emacs->StartEditor(
        window_id,
        editor,
        initial_text,
        NPVARIANT_TO_OBJECT(args[3]),
        &error);
    if (job_id == 0)
      NPN_SetException(this, error.c_str());
    INT32_TO_NPVARIANT(job_id, *result);
    return true;
  }

  return false;
}

bool EmacsObject::Enumerate(NPIdentifier **identifiers,
			    uint32_t *identifier_count) {
  const int kPropertyCount = 1;
  NPIdentifier* properties = static_cast<NPIdentifier*>(
      NPN_MemAlloc(sizeof(NPIdentifier) * kPropertyCount));
  if (!properties) return false;

  properties[0] = NPN_GetStringIdentifier("startEditor");

  *identifiers = properties;
  *identifier_count = kPropertyCount;
  return true;
}
