CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lSDL2 -lSDL_image -lGL -lGLU
SOURCES=SDL_and_opengl.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=SDL_and_opengl
OBJ_DIR=obj

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(addprefix obj/,$(OBJECTS) ) -o $@ $(LDFLAGS) 

.cpp.o: 
	mkdir -p $(OBJ_DIR) && $(CC) $(CFLAGS) $< -o $(OBJ_DIR)/$@

clean:
	rm -rf $(OBJ_DIR)/*o  
	rm -rf $(EXECUTABLE)

