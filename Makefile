CC=g++
CFLAGS=-c -Wall -Iinc
LDFLAGS=-lSDL2 -lGL -lGLU
SOURCES=game.cpp main.cpp timer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ceng356.out
OBJ_DIR=obj

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(addprefix obj/,$(OBJECTS) ) -o $@ $(LDFLAGS) 

.cpp.o: 
	mkdir -p $(OBJ_DIR) && $(CC) $(CFLAGS) $< -o $(OBJ_DIR)/$@

clean:
	rm -rf $(OBJ_DIR)/*o  
	rm -rf $(EXECUTABLE)

