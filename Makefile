## Shader Compile Makefile
## Re-point ARNOLD_PATH to your own SDK path

OUT_PATH = bin/
INCLUDE_PATH=src/include
ARNOLD_PATH = /mnt/work/software/arnold/arnold-4.2.11.3
TEMP_PATH = ${OUT_PATH}tempShader1.os ${OUT_PATH}tempShader2.os ${OUT_PATH}tempShader3.os ${OUT_PATH}tempShader4.os ${OUT_PATH}loader.os

all:loader shader1 shader2 shader3 shader4
	g++ -o ${OUT_PATH}sytoons.so -shared ${TEMP_PATH} -I${INCLUDE_PATH} -L${ARNOLD_PATH}/bin -lai
	@echo -e '\033[0;33m'"Done!"'\033[0m'
loader:
	@echo -e '\033[0;33m'"Start Compiling..."'\033[0m'
	g++ -o ${OUT_PATH}loader.os -c -fPIC -D_LINUX -I${ARNOLD_PATH}/include -I${INCLUDE_PATH} src/shader_loader.cpp
shader1:
	g++ -o ${OUT_PATH}tempShader1.os -c -fPIC -D_LINUX -I${ARNOLD_PATH}/include -I${INCLUDE_PATH} src/syToons.cpp
shader2:
	g++ -o ${OUT_PATH}tempShader2.os -c -fPIC -D_LINUX -I${ARNOLD_PATH}/include -I${INCLUDE_PATH} src/syRamp.cpp
shader3:
	g++ -o ${OUT_PATH}tempShader3.os -c -fPIC -D_LINUX -I${ARNOLD_PATH}/include -I${INCLUDE_PATH} src/syRemap.cpp
shader4:
	g++ -o ${OUT_PATH}tempShader4.os -c -fPIC -D_LINUX -I${ARNOLD_PATH}/include -I${INCLUDE_PATH} src/syOutline.cpp

