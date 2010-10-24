#include "identifiers.h"

#include "browser.h"

namespace {

#define IDENTIFIER(symbol) const NPUTF8 STR_ ## symbol [] = #symbol;
#include "identifier_list.inc.h"
#undef IDENTIFIER

const NPUTF8 *strings[] = {
    #define IDENTIFIER(symbol) STR_ ## symbol ,
    #include "identifier_list.inc.h"
    #undef IDENTIFIER
};

}

namespace identifier {
NPIdentifier identifiers[NUM_SYMBOLS] = { 0 };
}

void initializeIdentifiers() {
    NPN_GetStringIdentifiers(strings,
                             sizeof(strings) / sizeof(strings[0]),
                             identifier::identifiers);
}
