var port = chrome.extension.connect({name: "content_script"});
var id = -1;
var nextId = 1;

port.onMessage.addListener(function (msg) {
    if (msg.type === "init") {
	id = msg.id;
	var iframe = document.createElement("iframe");
	var editorId = nextId++;
	var path = "editor.html?parent_id=" + id + "&editor_id=" + editorId;
	iframe.src = chrome.extension.getURL(path);
	document.body.appendChild(iframe);
	port.postMessage({
	    type: "editor_msg",
	    id: editorId,
	    message: {
		type: "start_editor",
		text: "WHY HELLO THERE!"
	    }
	});
    } else if (msg.type === "edit_done") {
	alert("Editor is done! text = " + msg.text);
    }
});
