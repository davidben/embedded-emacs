#ifndef INCLUDED_SCRIPT_OBJECT_H_
#define INCLUDED_SCRIPT_OBJECT_H_

#include "browser.h"
#include "npapi-headers/npruntime.h"

class EmacsInstance;

class ScriptObject : public NPObject {
public:
    static ScriptObject* create(NPP npp);
    void invalidate();

    static NPObject* allocateThunk(NPP npp, NPClass *aClass);
    static void deallocateThunk(NPObject *npobj);
    static void invalidateThunk(NPObject *npobj);

private:
    ScriptObject(NPP npp);
    ~ScriptObject();

    EmacsInstance* emacsInstance();
    
    NPP npp_;
};

#endif  // INCLUDED_SCRIPT_OBJECT_H_
