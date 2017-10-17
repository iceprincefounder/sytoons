call C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/vcvarsall.bat

set ARNOLD_PATH=c:/path/to/arnold
rem cl /LD simple_shader.cpp /I %ARNOLD_PATH%/include %ARNOLD_PATH%/lib/ai.lib /link /out:simple_shader.dll