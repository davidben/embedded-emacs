// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

// Initialize default settings:
function initDefaultSettings() {
    var defaultSettings = {
        commandPattern: 'emacs --parent-id $WINDOW -- $PATH',
        triggerAltX: true,
        triggerDoubleClick: false
    };
    for (var k in defaultSettings) {
        if (localStorage[k] === undefined)
            localStorage[k] = defaultSettings[k];
    }
}
