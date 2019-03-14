CC=g++
ENGINE=..\gl_engine
CFLAGS=-c  -I".\includes" -I".\glm" -I"$(ENGINE)" -O2  -std=gnu++14 -D _GLFW_WIN32
ifeq ($(OS),Windows_NT)
	DELOPERATOR= del /q
	LDFLAGS=  -lglew32 -lSOIL -l:..\libglfw3.a -lopengl32 -static-libgcc -static-libstdc++  -l:libgdi32.a -L".\gl"
else
	DELOPERATOR= rm
	LDFLAGS=  -lGLEW -lSOIL -lGL `pkg-config --cflags glfw3` `pkg-config --static --libs glfw3`
endif
ENGINE_SOURCES= $(ENGINE)\gl_material.cpp $(ENGINE)\gl2d_image.cpp $(ENGINE)\gl2d_progressbar.cpp $(ENGINE)\gl_font16x16.cpp $(ENGINE)\glresourses.cpp $(ENGINE)\glscene.cpp $(ENGINE)\collision.cpp $(ENGINE)\engine_settings.cpp $(ENGINE)\loader.cpp $(ENGINE)\gl_resources_manager.cpp
SOURCES= $(ENGINE_SOURCES) map_heightmap.cpp game_event_fabric.cpp animation_sequence.cpp game_status.cpp main.cpp map_event_general.cpp map_event_valhalla.cpp map_event_hero_action.cpp map_event_hero_strikes.cpp  gl_render_target.cpp gl_character.cpp gl_model.cpp gl_game_state_dungeon.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=game

all: build

build:	$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) game.res
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS) game.res

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
game.res:
	windres game.rc -O coff -o game.res

rebuild: clean build

clean:
	$(DELOPERATOR) $(OBJECTS)