#!/usr/bin/node

// Yes, I know I am a NODE Fan-boy.

const path = require('path');
const fs = require('fs');
const os = require('os');

function getFiles(thisPath) {
    const entries = fs.readdirSync(thisPath, { withFileTypes: true });

    // Get files within the current directory and add a path key to the file objects
    const files = entries
        .filter(file => !file.isDirectory())
        .map(file => ({ ...file, path: path.join(thisPath, file.name) }));

    // Get folders within the current directory
    const folders = entries.filter(folder => folder.isDirectory());

    for (const folder of folders)
        /*
          Add the found files within the subdirectory to the files array by calling the
          current function itself
        */
        files.push(...getFiles(path.join(thisPath, folder.name)));

    return files;
}

let arduinoUserDir;
let arduinoInstall;
let arduinoWorkspace;
let sdkVersions;

if (os.platform() === "win32") {
    arduinoUserDir = path.join(
        process.env.APPDATA,
        "../Local/Arduino15"
    );
    arduinoInstall = path.join(
        process.env['ProgramFiles(x86)'],
        "Arduino"
    );
    arduinoWorkspace = path.join(
        process.env.USERPROFILE,
        "Documents/Arduino"
    );
} else if (os.platform() === "darwin") {
    arduinoUserDir = path.join(
        process.env.HOME,
        "Library/Arduino15"
    );
    arduinoInstall = "/Applications/Arduino.app/Contents/Java";
    arduinoWorkspace = path.join(
        process.env.HOME,
        "Documents/Arduino"
    );
} else {
    console.error("Incompatible Platform! (Currently Working on Linux...)");
    return;
}


const esp32Path = path.join(
    arduinoUserDir,
    "packages/esp32/hardware/esp32"
);

const esp32ToolsPath = path.join(
    arduinoUserDir,
    "packages/esp32/tools"
);

const esp32ToolsFiles = getFiles(esp32ToolsPath);
const esp32GCCs = esp32ToolsFiles.filter(n => n.name.toLowerCase().endsWith("gcc"+(os.platform() === "win32" ? ".exe" : ""))).map(n => n.path);

if (esp32GCCs.length === 0) {
    console.error("NO ESP32 GCC was FOUND!");
    return;
}

const esp32GCC = esp32GCCs[0];

sdkVersions = fs.readdirSync(
    esp32Path
);

if (sdkVersions.length === 0) {
    console.error("NO ESP32 SDK was FOUND!");
    return;
}

const sdkVersion = sdkVersions[0];
const architecture = process.arch === "ia32" ? "x86" : process.arch;

fs.writeFileSync(
    './.vscode/c_cpp_properties.json',
    JSON.stringify({
        configurations: [
            {
                name: os.platform(),
                includePath: [
                    // arduino application library
                    path.join(
                        arduinoInstall,
                        "libraries"
                    )+path.sep+"**",

                    // workspace library
                    path.join(
                        arduinoWorkspace,
                        "libraries"
                    )+path.sep+"**",

                    // ArduinoJson
                    path.join(
                        arduinoWorkspace,
                        "libraries",
                        "ArduinoJson"
                    )+path.sep+"**",

                    //TJpgDecoder
                    path.join(
                        arduinoWorkspace,
                        "libraries",
                        "TJpg_Decoder"
                    )+path.sep+"**",

                    // ESP32 Specifics
                    path.join(
                        arduinoUserDir,
                        "packages/esp32/tools"
                    )+path.sep+"**",

                    // ESP32 Libraries
                    path.join(
                        esp32Path,
                        sdkVersion
                    )+path.sep+"**",

                    // current directory
                    path.join(
                        "${workspaceFolder}",
                        "main"
                    )+path.sep+"**",
                ],
                forcedInclude: [],
                defines: [
                    "USBCON"
                ],
                intelliSenseMode: "gcc-"+architecture,
                compilerPath: esp32GCC,
                cStandard: "c11",
                cppStandard: "c++11",
            }
        ],
        version: 4,
    }, null, 4)
);

