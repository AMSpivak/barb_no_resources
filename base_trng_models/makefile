#-fprofile-generate -fprofile-use  
CC=g++
ENGINE=../gl_engine

ifeq ($(OS),Windows_NT)
	ICON=game.res
	CFLAGS=-c  -I"./includes" -I"../gl_engine/glm" -I"../gl_engine" -O3  -std=gnu++14 -D _GLFW_WIN32
	DELOPERATOR= del /q
	LDFLAGS=  -lglew32 -lSOIL -l:../libglfw3.a libirrKlang.a -lopengl32 -static-libgcc -static-libstdc++  -l:libgdi32.a -L"./gl"
else
	ICON= 
	CFLAGS=-c  -I"./includes" -I"../gl_engine/glm" -I"../gl_engine" -O3  -std=gnu++14 -D LINUX
	DELOPERATOR= rm
	LDFLAGS=  -lGLEW -lSOIL -lGL `pkg-config --cflags glfw3` `pkg-config --static --libs glfw3`
endif
ENGINE_SOURCES= $(wildcard $(ENGINE)/*.cpp)
APP_SOURCES= $(wildcard *.cpp)
SOURCES= $(ENGINE_SOURCES) $(APP_SOURCES)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=game


all: build

build:	$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(ICON)
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS) $(ICON)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
game.res:
	windres game.rc -O coff -o game.res

rebuild: clean build

clean: clean_engine clean_app

clean_app: 
	$(DELOPERATOR) *.o

ifeq ($(OS),Windows_NT)
clean_engine: 
	$(DELOPERATOR) ..\gl_engine\*.o
else
clean_engine: 
	$(DELOPERATOR) ../gl_engine/*.o
endif