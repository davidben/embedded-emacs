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
    ScriptObject::invokeDefaultThunk,  // invokeDefault
    ScriptObject::hasPropertyThunk,  // hasProperty
    ScriptObject::getPropertyThunk,  // getProperty
    ScriptObject::setPropertyThunk,  // setProperty
    ScriptObject::removePropertyThunk,  // removeProperty
    ScriptObject::enumerateThunk,  // enumerate
    ScriptObject::constructThunk,  // construct
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
            name == identifier::setInitialText() ||
            name == identifier::setEditorCommand());
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

bool ScriptObject::invokeDefault(const NPVariant *args,
                                 uint32_t argCount,
                                 NPVariant *result)
{
    return false;
}

bool ScriptObject::hasProperty(NPIdentifier name)
{
    return false;
}

bool ScriptObject::getProperty(NPIdentifier name, NPVariant *result)
{
    return false;
}

bool ScriptObject::setProperty(NPIdentifier name, const NPVariant *value)
{
    return false;
}

bool ScriptObject::removeProperty(NPIdentifier name)
{
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

bool ScriptObject::construct(const NPVariant *args, uint32_t argCount,
                             NPVariant *result)
{
    return false;
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
bool ScriptObject::invokeDefaultThunk(NPObject *npobj,
                                      const NPVariant *args, uint32_t argCount,
                                      NPVariant *result)
{
    return static_cast<ScriptObject*>(npobj)->invokeDefault(
        args, argCount, result);
}

// static
bool ScriptObject::hasPropertyThunk(NPObject *npobj, NPIdentifier name)
{
    return static_cast<ScriptObject*>(npobj)->hasProperty(name);
}

// static
bool ScriptObject::getPropertyThunk(NPObject *npobj,
                                    NPIdentifier name,
                                    NPVariant *result)
{
    return static_cast<ScriptObject*>(npobj)->getProperty(name, result);
}

// static
bool ScriptObject::setPropertyThunk(NPObject *npobj,
				 NPIdentifier name,
				 const NPVariant *value)
{
    return static_cast<ScriptObject*>(npobj)->setProperty(name, value);
}

// static
bool ScriptObject::removePropertyThunk(NPObject *npobj, NPIdentifier name)
{
    return static_cast<ScriptObject*>(npobj)->removeProperty(name);
}

// static
bool ScriptObject::enumerateThunk(NPObject *npobj, NPIdentifier **identifiers,
                                  uint32_t *identifierCount)
{
    return static_cast<ScriptObject*>(npobj)->enumerate(identifiers,
                                                        identifierCount);
}

// static
bool ScriptObject::constructThunk(NPObject *npobj,
                                  const NPVariant *args, uint32_t argCount,
                                  NPVariant *result)
{
    return static_cast<ScriptObject*>(npobj)->construct(args, argCount, result);
}
