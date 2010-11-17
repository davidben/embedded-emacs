// Copyright (c) 2010 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_IDENTIFIERS_H_
#define INCLUDED_IDENTIFIERS_H_

#include "browser.h"
#include "npapi-headers/npruntime.h"

namespace identifier {

enum Idx {
    DUMMY = -1,
#define IDENTIFIER(symbol) IDX_ ## symbol,
#include "identifier_list.inc.h"
#undef IDENTIFIER
    NUM_SYMBOLS
};

extern NPIdentifier identifiers[];

#define IDENTIFIER(symbol) \
    inline NPIdentifier symbol () { return identifiers[ IDX_ ## symbol ]; }
#include "identifier_list.inc.h"
#undef IDENTIFIER

}

void initializeIdentifiers();

#endif  // INCLUDED_IDENTIFIERS_H_
