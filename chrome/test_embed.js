var port = chrome.extension.connect({name: "content_script"});
var id = -1;
var nextId = 1;

var editorCallbacks = {}

function iframePath(editorId) {
    return "editor.html?parent_id=" + id + "&editor_id=" + editorId;
}

function hookTextArea(node) {
    var editorId = nextId++;
    node.addEventListener('dblclick', function (ev) {
	if (node.parentNode) {
	    var iframe = document.createElement("iframe");
	    iframe.src = chrome.extension.getURL(iframePath(editorId));
	    node.parentNode.insertBefore(iframe, node);
	    port.postMessage({
		type: "editor_msg",
		id: editorId,
		message: {
		    type: "start_editor",
		    text: node.value
		}
	    });

	    // TODO: If child editor gets detached, then drop the
	    // callback to avoid leaking memory.
	    editorCallbacks[editorId] = function (text) {
		node.value = text;
		if (iframe.parentNode)
		    iframe.parentNode.removeChild(iframe);
		delete editorCallbacks[editorId];
	    }
	}
    });
}

port.onMessage.addListener(function (msg) {
    if (msg.type === "init") {
	id = msg.id;

	// Hook the current textareas.
	var textareas = document.getElementsByTagName("textarea");
	for (var i = 0; i < textareas.length; i++) {
	    hookTextArea(textareas[i]);
	}

	// And hook any new ones that get created.
	document.addEventListener('DOMNodeInserted', function (ev) {
	    if (ev.target.tagName.toLowerCase() === "textarea") {
		hookTextArea(ev.target);
	    }
	});
    } else if (msg.type === "edit_done") {
	if (editorCallbacks[msg.source])
	    editorCallbacks[msg.source](msg.text);
    }
});
