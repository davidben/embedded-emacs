// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

// Initialize default settings:
if (localStorage['commandPattern'] === undefined)
    // FIXME: This string is also in options.html
    localStorage['commandPattern'] = "emacs --parent-id $WINDOW --file $PATH";

var launcher = document.getElementById('launcher');

chrome.extension.onRequest.addListener(
    function (request, sender, sendResponse) {
	if (request.type === "start_editor") {
	    launcher.setEditorCommand(localStorage['commandPattern']);
	    launcher.startEditor(
                request.windowId, request.text,
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
