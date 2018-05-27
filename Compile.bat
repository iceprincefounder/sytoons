call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
set > environment.txt
set ARNOLD_PATH="D:/Workspace/arnold-5.0.2.3-windows"
rem src/syToons.cpp
cl /LD src/syOutline.cpp src/shader_loader.cpp /I %ARNOLD_PATH%/include /I src/include %ARNOLD_PATH%/lib/ai.lib /link /out:bin/sytoons.dll
