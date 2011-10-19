var presets = [ {
    name: "Emacs",
    pattern: "emacs --parent-id $WINDOW --file $PATH",
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
        saveCommandPattern();
    });
    return button;
}

function setup() {
    // Make the preset buttons.
    var list = document.getElementById("presetsList");
    for (var i = 0; i < presets.length; i++) {
        list.appendChild(makeButton(presets[i]));
    }

    // Hook up a listener to commandPattern. TODO: Make this automatic somehow.
    document.getElementById("commandPattern").addEventListener("change", saveCommandPattern);
    loadOptions();
}

function loadOptions() {
    document.getElementById("commandPattern").value = localStorage["commandPattern"];
}

function saveCommandPattern() {
    localStorage["commandPattern"] = document.getElementById("commandPattern").value;
}

window.addEventListener("load", setup);
