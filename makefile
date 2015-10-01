OPT = -g3 -O0
SOURCES =  voxblit.c ../iron/linmath.c ../iron/utils.c ../iron/mem.c ../iron/fileio.c ../iron/array.c ../iron/math.c ../iron/time.c ../iron/hashtable.c ../iron/log.c oct_node.c vec3i.c octree.c event.c renderer.c sdl_event.c image.c
CC = gcc
TARGET = run.exe
OBJECTS =$(SOURCES:.c=.o)
LDFLAGS=-ldl -L. -L../libconcurrency-read-only/  $(OPT) -Wextra #-lmcheck #-ftlo  #setrlimit on linux 
LIBS= -ldl -lm -lpthread -lSDL2  -lSDL2_gfx

CFLAGS =  -I.. -std=c11 -c $(OPT) -Wall -Wextra -Werror=implicit-function-declaration -Wformat=0  -D_GNU_SOURCE -fdiagnostics-color -Werror -Wwrite-strings

all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -ldl -o $@

.c.o: $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@ -MMD -MF $@.depends
depend: h-depend
clean:
	rm $(OBJECTS) $(TARGET) *.o.depends
-include $(OBJECTS:.o=.o.depends)
