// Copyright (c) 2010 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

function getQueryValue(key) {
    // Is there seriously no better way to do this?
    var query = window.location.search.substring(1);
    var kvPairs = query.split("&");
    for (var i = 0; i < kvPairs.length; i++) {
	var kvPair = kvPairs[i].split("=");
	if (kvPair.length !== 2)
	    continue;
	if (kvPair[0] === key)
	    return kvPair[1];
    }
    return undefined;
}

var parent_id = getQueryValue("parent_id");
var editor_id = getQueryValue("editor_id");

var editor_started = false;

function startEditor(text) {
    if (editor_started)
	return;
    editor_started = true;
    var plugin = document.getElementById("plugin");
    // plugin.setEditorCommand("gvim --nofork --socketid $WINDOW $PATH");
    plugin.setEditorCommand("emacs --parent-id $WINDOW $PATH");
    plugin.setInitialText(text);
    plugin.setCallback(function(contents, status) {
	port.postMessage({
	    type: "parent_msg",
	    message: {
		type: "edit_done",
		text: contents
	    }
	});
    });
    plugin.startEditor();
    // FIXME: This doesn't work.
    plugin.focus();
}

if (parent_id !== undefined && editor_id !== undefined) {
    var name = "editor/" + parent_id + "/" + editor_id;
    var port = chrome.extension.connect({name: name});
    port.onMessage.addListener(function (msg) {
	if (msg.type === "start_editor") {
	    startEditor(msg.text);
	}
    });
}
