// Copyright (c) 2012 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

initDefaultSettings();

var launcher = document.getElementById('launcher');

chrome.extension.onRequest.addListener(
    function (request, sender, sendResponse) {
        if (request.type === 'get_config') {
            // TODO: send other config options when we get them.
            var enabled = launcher.hasOwnProperty('startEditor');
            sendResponse({
                enabled: enabled,
                triggerAltX: (localStorage['triggerAltX'] == 'true'),
                triggerDoubleClick: (localStorage['triggerDoubleClick'] == 'true')
            });
        } else if (request.type === 'start_editor') {
            launcher.startEditor(
                request.windowId, localStorage['editorType'], request.text,
                function (contents, status) { sendResponse(contents);});
        } else {
            console.log("WARNING: Unknown request type '" +
                        request.type + "'");
            console.log(request);
            console.log(sender);
            // Clear the request.
            sendResponse({});
        }
    }
);
