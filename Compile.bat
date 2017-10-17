set ARNOLD_PATH=c:/path/to/arnold
cl /LD simple_shader.cpp /I %ARNOLD_PATH%/include %ARNOLD_PATH%/lib/ai.lib /link /out:simple_shader.dll