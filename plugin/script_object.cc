#include "script_object.h"

#include "emacs_instance.h"
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
    return (name == identifier::startEditor() ||
            name == identifier::setCallback() ||
            name == identifier::setInitialText());
}

bool ScriptObject::invoke(NPIdentifier name,
                          const NPVariant *args,
                          uint32_t argCount,
                          NPVariant *result)
{
    if (!hasMethod(name))
        return false;

    EmacsInstance* emacs = emacsInstance();
    if (!emacs) {
        // TODO: raise an exception if the emacs is missing?
        VOID_TO_NPVARIANT(*result);
        return true;
    }

    if (name == identifier::startEditor()) {
        emacs->startEditor();
        VOID_TO_NPVARIANT(*result);
        return true;
    } else if (name == identifier::setCallback()) {
        if (argCount >= 1 && NPVARIANT_IS_OBJECT(args[0])) {
            emacs->setCallback(NPVARIANT_TO_OBJECT(args[0]));
        } else {
            // They gave us an integer. Merp. Well, too bad for them.
            emacs->setCallback(NULL);
        }
        VOID_TO_NPVARIANT(*result);
        return true;
    } else if (name == identifier::setInitialText()) {
        if (argCount >= 1 && NPVARIANT_IS_STRING(args[0])) {
            emacs->setInitialText(NPVARIANT_TO_STRING(args[0]).UTF8Characters,
                                  NPVARIANT_TO_STRING(args[0]).UTF8Length);
        } else {
            emacs->setInitialText(NULL, 0);
        }
        VOID_TO_NPVARIANT(*result);
        return true;
    }
    // Shouldn't happen.
    return false;
}

bool ScriptObject::enumerate(NPIdentifier **identifiers,
                             uint32_t *identifierCount)
{
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
