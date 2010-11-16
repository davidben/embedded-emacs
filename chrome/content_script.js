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
	    function relayout() {
		iframe.style.setProperty("width", node.offsetWidth + "px",
					 "important");
		iframe.style.setProperty("height", node.offsetHeight + "px",
					 "important");
		iframe.style.setProperty("top", node.offsetTop + "px",
					 "important");
		iframe.style.setProperty("left", node.offsetLeft + "px",
					 "important");
		if (node.style.position === "fixed") {
		    iframe.style.setProperty("position", "fixed", "important");
		} else {
		    iframe.style.setProperty("position", "absolute", "important");
		}
	    }
	    relayout();
	    node.parentNode.insertBefore(iframe, node);

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
		type: "editor_msg",
		id: editorId,
		message: {
		    type: "start_editor",
		    text: node.value
		}
	    });

	    editorCallbacks[editorId] = function (text) {
		node.removeEventListener("DOMAttrModified", onAttrModified);
		window.removeEventListener("resize", relayout);

		node.value = text;
		node.style.visibility = oldVisibility;
		if (iframe.parentNode)
		    iframe.parentNode.removeChild(iframe);
		delete editorCallbacks[editorId];
	    }

	    iframe.focus();
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
	document.body.addEventListener('DOMNodeInserted', function (ev) {
	    if (ev.target.nodeType != 1)
		return;
	    var textareas = ev.target.getElementsByTagName("textarea");
	    for (var i = 0; i < textareas.length; i++) {
		hookTextArea(textareas[i]);
	    }
	});
    } else if (msg.type === "edit_done") {
	if (editorCallbacks[msg.source])
	    editorCallbacks[msg.source](msg.text);
    } else if (msg.type === "child_detached") {
	delete editorCallbacks[msg.id];
    }
});
