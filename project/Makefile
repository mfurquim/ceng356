CC=g++
CFLAGS=-c -Wall
INCLUDES=-Iinc/ -I/usr/include/GL
LDFLAGS=-lSDL2 -lGL -lGLU -lwiiuse -L./
SOURCES=main.cpp game.cpp timer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ceng356.out
OBJ_DIR=obj

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(addprefix obj/,$(OBJECTS) ) -o $@ $(LDFLAGS) $(LIBS) $(INCLUDES)

.cpp.o: 
	mkdir -p $(OBJ_DIR) && $(CC) $(CFLAGS) $(INCLUDES) $< -o $(OBJ_DIR)/$@

clean:
	rm -rf $(OBJ_DIR)/*o  
	rm -rf $(EXECUTABLE)

