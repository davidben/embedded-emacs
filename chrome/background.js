// Copyright (c) 2010 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

// Initialize default settings:
if (localStorage['commandPattern'] === undefined)
    localStorage['commandPattern'] = "emacs --parent-id $WINDOW $PATH";
    // "gvim --nofork --socketid $WINDOW $PATH"

var nextId = 1;
var contentScripts = {};

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

    this.editors = {};
    this.pendingEditorMessages = {};

    // Tell the content script its id.
    this.port.postMessage({type: "init", id: this.id });
}
ContentScriptHost.prototype = {
    onMessage: function (msg) {
	if (msg.type === "editor_msg") {
	    if (msg.id === undefined || msg.message === undefined) {
		console.log("Invalid editor message");
	    } else {
		this.postEditorMessage(msg.id, msg.message);
	    }
	} else {
	    console.log("WARNING: ContentScriptHost: Unknown message type '" +
			msg.type + "'");
	}
    },
    onDisconnect: function () {
	delete contentScripts[this.id];
    },
    attachEditor: function (editor) {
	if (this.editors[editor.id]) {
	    console.log("WARNING: content script " + this.id +
			" reusing editor " + editor.id + "??");
	}
	this.editors[editor.id] = editor;

	// Send any pending messages.
	var pending = this.pendingEditorMessages[editor.id];
	delete this.pendingEditorMessages[editor.id];
	if (pending) {
	    for (var i = 0; i < pending.length; i++) {
		editor.port.postMessage(pending[i]);
	    }
	}
    },
    detachEditor: function (editor) {
	if (this.editors[editor.id] === editor) {
	    delete this.editors[editor.id];
	    delete this.pendingEditorMessages[editor.id];
	    this.port.postMessage({
		type: 'child_detached',
		id: editor.id
	    });
	}
    },
    postEditorMessage: function (id, msg) {
	if (this.editors[id]) {
	    this.editors[id].port.postMessage(msg);
	} else {
	    if (this.pendingEditorMessages[id] === undefined)
		this.pendingEditorMessages[id] = [];
	    this.pendingEditorMessages[id].push(msg);
	}
    }
};

function EditorHost(port) {
    var parts = port.name.split("/");
    if (parts.length != 3) {
	console.log("ERROR: Editor name '" + name +
		    "' is not of the form tab_id/editor_id");
	return;
    }
    var parent_id = parts[1];
    var id = parts[2];
    if (!contentScripts[parent_id]) {
	console.log("ERROR: Content script '" + parent_id +
		    "' does not exist");
	return;
    }
    // We're good.
    this.id = id;
    this.contentScript = contentScripts[parent_id];

    var self = this;
    this.port = port;
    this.port.onMessage.addListener(function (msg) {
	self.onMessage(msg);
    });
    this.port.onDisconnect.addListener(function () {
	self.onDisconnect();
    });

    // And when the port is known, attach.
    this.contentScript.attachEditor(this);
}
EditorHost.prototype = {
    onMessage: function (msg) {
	if (msg.type === "parent_msg") {
	    if (msg.message === undefined) {
		console.log("Invalid parent message");
	    } else {
		// Tell the parent who sent it.
		msg.message.source = this.id;
		// We don't need to queue up on this end; if the
		// content script knows its id, its been connected.
		this.contentScript.port.postMessage(msg.message);
	    }
	} else {
	    console.log("WARNING: ContentScriptHost: Unknown message type '" +
			msg.type + "'");
	}
    },
    onDisconnect: function () {
	this.contentScript.detachEditor(this);
    },
};

chrome.extension.onConnect.addListener(function (port) {
    if (port.name === "content_script") {
	new ContentScriptHost(port);
    } else {
	new EditorHost(port);
    }
});
