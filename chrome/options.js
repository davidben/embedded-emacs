// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

// Doesn't hurt. Not sure if relying on the background page can race.
initDefaultSettings();

// Borrow Chromium WebUI-style i18n handling, or at least something
// similar.
function localizeElements() {
    var elements = document.querySelectorAll("[i18n-content]");
    for (var i = 0; i < elements.length; i++) {
	var element = elements[i];
	var key = element.getAttribute("i18n-content");
	// Ick. I'd prefer textContent, but there are <kbd> tags in
	// places.
	element.innerHTML = chrome.i18n.getMessage(key);
    }
}

function setup() {
    localizeElements();
    loadOptions();
    updateInstructions();

    // Hook up listeners.
    var editorOptions = document.getElementsByName("editor");
    for (var i = 0; i < editorOptions.length; i++) {
        editorOptions[i].addEventListener("change", saveOptions);
    }
    document.getElementById("triggerAltX").addEventListener("change", saveOptions);
    document.getElementById("triggerDoubleClick").addEventListener("change", saveOptions);
}

function updateInstructions() {
    var customInstructions = document.getElementById("customInstructions");
    var editorOptionCustom = document.getElementById("editorOptionCustom");
    customInstructions.hidden = !editorOptionCustom.checked;
}

function loadOptions() {
    var editorOptions = document.getElementsByName("editor");
    for (var i = 0; i < editorOptions.length; i++) {
        editorOptions[i].checked = (editorOptions[i].value == localStorage["editorType"]);
    }

    document.getElementById("triggerAltX").checked = (localStorage["triggerAltX"] == "true");
    document.getElementById("triggerDoubleClick").checked = (localStorage["triggerDoubleClick"] == "true");
}

function saveOptions() {
    var editorOptions = document.getElementsByName("editor");
    for (var i = 0; i < editorOptions.length; i++) {
        if (editorOptions[i].checked)
            localStorage["editorType"] = editorOptions[i].value;
    }

    localStorage["triggerAltX"] = (document.getElementById("triggerAltX").checked ? "true" : "false");
    localStorage["triggerDoubleClick"] = (document.getElementById("triggerDoubleClick").checked ? "true" : "false");

    updateInstructions();
}

window.addEventListener("load", setup);
