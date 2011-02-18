// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

var port;
var nextEditorId = 1;

var editorCallbacks = {}

function ensurePortConnected() {
    if (port)
	return;
    port = chrome.extension.connect({name: "content_script"});

    port.onMessage.addListener(function (msg) {
	if (msg.type === "edit_done") {
	    if (editorCallbacks[msg.source])
		editorCallbacks[msg.source](msg.text);
	}
    });
}

function positioned(elem) {
    var position = window.getComputedStyle(elem).position;
    return (position === "absolute" ||
	    position === "relative" ||
	    position === "fixed");
}

function hookTextArea(node) {
    var editorId = nextEditorId++;

    function attachEmacs() {
	if (!node.parentNode)
	    return;
	var container = document.createElement("embed");
	container.type = "application/x-embedded-emacs-container";
	function relayout() {
	    container.style.setProperty("width", node.offsetWidth + "px",
				     "important");
	    container.style.setProperty("height", node.offsetHeight + "px",
				     "important");
	    // Apparently offsetParent is a lie.
	    var parent = node.offsetParent;
	    var top = node.offsetTop;
	    var left = node.offsetLeft;
	    var body = node.ownerDocument.body;
	    var docElem = node.ownerDocument.documentElement;
	    while (parent &&
		   parent !== body && parent !== docElem &&
		   !positioned(parent)) {
		top += parent.offsetTop;
		left += parent.offsetLeft;
		parent = parent.offsetParent;
	    }
	    container.style.setProperty("top", top + "px", "important");
	    container.style.setProperty("left", left + "px", "important");
	    if (window.getComputedStyle(node).position === "fixed") {
		container.style.setProperty("position", "fixed", "important");
	    } else {
		container.style.setProperty("position", "absolute", "important");
	    }
	}
	relayout();
	node.parentNode.insertBefore(container, node);

	function onAttrModified(ev) {
	    if (ev.attrName !== "style")
		return;
	    relayout();
	}
	// FIXME: This doesn't actually catch CSS changes.
	node.addEventListener("DOMAttrModified", onAttrModified);
	window.addEventListener("resize", relayout);

	// FIXME: If another script decides to change this value
	// again, act accordingly.
	var oldVisibility = node.style.visibility;
	node.style.visibility = "hidden";
	port.postMessage({
	    type: "start_editor",
	    id: editorId,
            text: node.value,
            windowId: container.windowId
	});

	editorCallbacks[editorId] = function (text) {
	    node.removeEventListener("DOMAttrModified", onAttrModified);
	    window.removeEventListener("resize", relayout);

	    node.value = text;
	    node.style.visibility = oldVisibility;
	    if (container.parentNode)
		container.parentNode.removeChild(container);
	    delete editorCallbacks[editorId];
	}

	// FIXME: This doesn't work.
	container.focus();
    }
    function attachEmacsWrapper() {
	ensurePortConnected();
	attachEmacs();
    }

    node.addEventListener('dblclick', attachEmacsWrapper);
}

// Hook the current textareas.
var textareas = document.getElementsByTagName("textarea");
for (var i = 0; i < textareas.length; i++) {
    hookTextArea(textareas[i]);
}

// And hook any new ones that get created.
document.body.addEventListener('DOMNodeInserted', function (ev) {
    if (ev.target.nodeType != 1)
		return;
    var textareas = ev.target.getElementsByTagName("textarea");
    for (var i = 0; i < textareas.length; i++) {
	hookTextArea(textareas[i]);
    }
});
