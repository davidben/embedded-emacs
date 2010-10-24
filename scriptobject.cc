#include "scriptobject.h"

#include "emacsinstance.h"

namespace {

NPClass npclass = {
    NP_CLASS_STRUCT_VERSION,
    ScriptObject::allocateThunk,  // allocate
    ScriptObject::deallocateThunk,  // deallocate
    NULL,  // invalidate
    NULL,  // hasMethod
    NULL,  // invoke
    NULL,  // invokeDefault
    NULL,  // hasProperty
    NULL,  // getProperty
    NULL,  // setProperty
    NULL,  // removeProperty
    NULL,  // enumerate
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
