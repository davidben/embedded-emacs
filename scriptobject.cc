#include "scriptobject.h"

#include "emacsinstance.h"
#include "identifiers.h"

namespace {

NPClass npclass = {
    NP_CLASS_STRUCT_VERSION,
    ScriptObject::allocateThunk,  // allocate
    ScriptObject::deallocateThunk,  // deallocate
    ScriptObject::invalidateThunk,  // invalidate
    ScriptObject::hasMethodThunk,  // hasMethod
    ScriptObject::invokeThunk,  // invoke
    NULL,  // invokeDefault
    NULL,  // hasProperty
    NULL,  // getProperty
    NULL,  // setProperty
    NULL,  // removeProperty
    ScriptObject::enumerateThunk,  // enumerate
    NULL,  // construct
};

}  // namespace

ScriptObject* ScriptObject::create(NPP npp)
{
    return static_cast<ScriptObject*>(NPN_CreateObject(npp, &npclass));
}

ScriptObject::ScriptObject(NPP npp)
        : npp_(npp)
{
}

ScriptObject::~ScriptObject()
{
}

EmacsInstance* ScriptObject::emacsInstance()
{
    if (!npp_) return NULL;
    return static_cast<EmacsInstance*>(npp_->pdata);
}

void ScriptObject::invalidate()
{
    npp_ = NULL;
}

bool ScriptObject::hasMethod(NPIdentifier name)
{
    return (name == identifier::startEditor());
}

bool ScriptObject::invoke(NPIdentifier name,
                          const NPVariant *args,
                          uint32_t argCount,
                          NPVariant *result)
{
    EmacsInstance* emacs = emacsInstance();
    if (name == identifier::startEditor()) {
        if (emacs) {
            emacs->startEditor();
        }
        // TODO: raise an exception if the emacs is missing?
        VOID_TO_NPVARIANT(*result);
        return true;
    }
    return false;
}

bool ScriptObject::enumerate(NPIdentifier **identifiers,
                             uint32_t *identifierCount)
{
    NPIdentifier* properties =
            static_cast<NPIdentifier*>(NPN_MemAlloc(sizeof(NPIdentifier) * 1));
    if (!properties) return false;

    properties[0] = identifier::startEditor();

    *identifiers = properties;
    *identifierCount = 1;
    return true;
}

// static
NPObject* ScriptObject::allocateThunk(NPP npp, NPClass *aClass)
{
    return new ScriptObject(npp);
}

// static
void ScriptObject::deallocateThunk(NPObject *npobj)
{
    delete static_cast<ScriptObject*>(npobj);
}

// static
void ScriptObject::invalidateThunk(NPObject *npobj)
{
    static_cast<ScriptObject*>(npobj)->invalidate();
}

// static
bool ScriptObject::hasMethodThunk(NPObject *npobj, NPIdentifier name)
{
    return static_cast<ScriptObject*>(npobj)->hasMethod(name);
}

// static
bool ScriptObject::invokeThunk(NPObject *npobj, NPIdentifier name,
                               const NPVariant *args, uint32_t argCount,
                               NPVariant *result)
{
    return static_cast<ScriptObject*>(npobj)->invoke(name, args, argCount,
                                                     result);
}

// static
bool ScriptObject::enumerateThunk(NPObject *npobj, NPIdentifier **identifiers,
                                  uint32_t *identifierCount)
{
    return static_cast<ScriptObject*>(npobj)->enumerate(identifiers,
                                                        identifierCount);
}
