TARGETDIR="target"
TARGET="${TARGETDIR}/Yarbil.exe"

all:
	mkdir -p ${TARGETDIR}
	gcc -o ${TARGET} src/main.c -Wall -Wextra -std=c99 -Wno-missing-braces -Wconversion -g -O0 C:/raylib/raylib/src/raylib.rc.data -I. -IC:/raylib/raylib/src -IC:/raylib/raylib/src/external -L. -LC:/raylib/raylib/src -LC:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm -DPLATFORM_DESKTOP

clean:
	rm -rf ${TARGETDIR}

