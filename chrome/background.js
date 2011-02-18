// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

// Initialize default settings:
if (localStorage['commandPattern'] === undefined)
    localStorage['commandPattern'] = "emacs --parent-id $WINDOW $PATH";
    // "gvim --nofork --socketid $WINDOW $PATH"

var nextId = 1;
var contentScripts = {};
var launcher = document.getElementById('launcher');

function ContentScriptHost(port) {
    this.id = nextId++;
    contentScripts[this.id] = this;

    var self = this;
    this.port = port;
    this.port.onMessage.addListener(function (msg) {
	self.onMessage(msg);
    });
    this.port.onDisconnect.addListener(function () {
	self.onDisconnect();
    });
}
ContentScriptHost.prototype = {
    onMessage: function (msg) {
	if (msg.type === "start_editor") {
	    launcher.setEditorCommand(localStorage['commandPattern']);
	    var self = this;
	    launcher.startEditor(
                msg.windowId, msg.text,
                function (contents, status) {
                    self.port.postMessage({
                        type: 'edit_done',
                        source: msg.id,
                        text: contents
                    });
            });
	} else {
	    console.log("WARNING: ContentScriptHost: Unknown message type '" +
			msg.type + "'");
	}
    },
    onDisconnect: function () {
	delete contentScripts[this.id];
    },
};

chrome.extension.onConnect.addListener(function (port) {
    if (port.name === "content_script") {
	new ContentScriptHost(port);
    }
});
