// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

// Doesn't hurt. Not sure if relying on the background page can race.
initDefaultSettings();

var presets = [ {
    name: "Emacs",
    pattern: "emacs --parent-id $WINDOW -- $PATH",
    icon: "editors/emacs.png"
}, {
    name: "gVim",
    pattern: "gvim --nofork --socketid $WINDOW -- $PATH",
    icon: "editors/gvim.png"
}];

function makeButton(spec) {
    var img = document.createElement("img");
    var button = document.createElement("button");
    img.setAttribute("src", spec.icon);
    img.setAttribute("alt", "");
    button.setAttribute("class", "presetButton");
    button.appendChild(img);
    button.appendChild(document.createTextNode(spec.name));
    button.addEventListener("click", function (ev) {
        document.getElementById("commandPattern").value = spec.pattern;
        // This doesn't fire the change event.
        saveOptions();
    });
    return button;
}

function setup() {
    // Make the preset buttons.
    var list = document.getElementById("presetsList");
    for (var i = 0; i < presets.length; i++) {
        list.appendChild(makeButton(presets[i]));
    }

    loadOptions();

    // Hook up listeners.
    document.getElementById("commandPattern").addEventListener("change", saveOptions);
    document.getElementById("triggerAltX").addEventListener("change", saveOptions);
    document.getElementById("triggerDoubleClick").addEventListener("change", saveOptions);
}

function loadOptions() {
    document.getElementById("commandPattern").value = localStorage["commandPattern"];
    document.getElementById("triggerAltX").checked = (localStorage["triggerAltX"] == "true");
    document.getElementById("triggerDoubleClick").checked = (localStorage["triggerDoubleClick"] == "true");
}

function saveOptions() {
    localStorage["commandPattern"] = document.getElementById("commandPattern").value;
    localStorage["triggerAltX"] = (document.getElementById("triggerAltX").checked ? "true" : "false");
    localStorage["triggerDoubleClick"] = (document.getElementById("triggerDoubleClick").checked ? "true" : "false");
}

window.addEventListener("load", setup);
