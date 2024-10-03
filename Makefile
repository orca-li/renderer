ifeq ($(OS), Windows_NT)
	MACROS_OS := -D"_WIN32"
else
	MACROS_OS :=
endif

all:
	gcc $(MACROS_OS) renderer.c -lm -o renderer