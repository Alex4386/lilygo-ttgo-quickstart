#!/bin/sh
compilerPath=`find $HOME/Library/Arduino15/packages/esp32/tools -name "*gcc" | grep bin | head -n 1`
currentVersion=`ls $HOME/Library/Arduino15/packages/esp32/hardware/esp32/`
cat > .vscode/c_cpp_properties.json <<EOF
{
    "configurations": [
        {
            "name": "Mac",
            "includePath": [
                "$HOME/Library/Arduino15/packages/esp32/tools/**",
                "$HOME/Documents/Arduino/libraries/**",
                "/Applications/Arduino.app/Contents/Java/libraries/**",
                "$HOME/Library/Arduino15/packages/esp32/hardware/esp32/$currentVersion/**",
                "\${workspaceFolder}/**"
            ],
            "forcedInclude": [],
            "defines": [
                "USBCON"
            ],
            "macFrameworkPath": [
                "/System/Library/Frameworks",
                "/Library/Frameworks"
            ],
            "intelliSenseMode": "gcc-x64",
            "compilerPath": "$compilerPath",
            "cStandard": "c11",
            "cppStandard": "c++11"
        }
    ],
    "version": 4
}
EOF

cat > .vscode/settings.json <<EOF
{
    "esp8266fs.esptool.executable": "$HOME/Library/Arduino15/packages/esp32/hardware/esp32/$currentVersion/tools/esptool.py",
    "esp8266fs.espota.py": "$HOME/Library/Arduino15/packages/esp32/hardware/esp32/$currentVersion/tools/espota.py",
}
EOF