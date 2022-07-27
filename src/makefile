CC=gcc
STD=-std=c17
STD_OPT=-g -Wall
MAIN=main.c
LEVEL_EDITOR_MAIN=level_editor.c
INC_SRC=game_config.c front.c player.c parse.c calc.c render.c update.c weather.c
INC_DIR=-Iinclude
OUTPUT_DIR=bin
OUTPUT_EXEC=$(OUTPUT_DIR)\\main
OUTPUT_LEVEL_EDITOR=$(OUTPUT_DIR)\\level_editor
SDL_LIB=-Ilibraries\SDL\Include -Llibraries\SDL\lib
SDL_OPT=-lmingw32 -lSDL2main -lSDL2


all: compile

run: compile
	$(OUTPUT_EXEC)

run_editor: compile_editor
	$(OUTPUT_LEVEL_EDITOR)

compile:
	cls
	$(CC) $(STD) $(INC_DIR) $(SDL_LIB) $(STD_OPT) -o $(OUTPUT_EXEC) $(MAIN) $(INC_SRC) $(SDL_OPT)

compile_editor:
	cls
	$(CC) $(STD) $(INC_DIR) $(SDL_LIB) $(STD_OPT) -o $(OUTPUT_LEVEL_EDITOR) $(LEVEL_EDITOR_MAIN) $(INC_SRC) $(SDL_OPT)

clean:
	del $(OUTPUT_DIR)\\**.exe