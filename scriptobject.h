#ifndef INCLUDED_SCRIPT_OBJECT_H_
#define INCLUDED_SCRIPT_OBJECT_H_

#include "browser.h"
#include "npapi-headers/npruntime.h"

class EmacsInstance;

class ScriptObject : public NPObject {
public:
    static ScriptObject* create(NPP npp);

    void invalidate();
    bool hasMethod(NPIdentifier name);
    bool invoke(NPIdentifier name,
		const NPVariant *args,
		uint32_t argCount,
		NPVariant *result);
    bool enumerate(NPIdentifier **identifiers, uint32_t *identifierCount);


    static NPObject* allocateThunk(NPP npp, NPClass *aClass);
    static void deallocateThunk(NPObject *npobj);
    static void invalidateThunk(NPObject *npobj);
    static bool hasMethodThunk(NPObject *npobj, NPIdentifier name);
    static bool invokeThunk(NPObject *npobj, NPIdentifier name,
			    const NPVariant *args, uint32_t argCount,
			    NPVariant *result);
    static bool enumerateThunk(NPObject *npobj, NPIdentifier **identifiers,
			       uint32_t *identifierCount);

private:
    ScriptObject(NPP npp);
    ~ScriptObject();

    EmacsInstance* emacsInstance();
    
    NPP npp_;
};

#endif  // INCLUDED_SCRIPT_OBJECT_H_
