call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
set > environment.txt
set ARNOLD_PATH="C:/Workspace/arnold-4.2.11.3-windows"

cl /LD src/syToons.cpp src/syRamp.cpp src/syRemap.cpp src/shader_loader.cpp /I %ARNOLD_PATH%/include /I src/include %ARNOLD_PATH%/lib/ai.lib /link /out:bin/sytoons.dll
