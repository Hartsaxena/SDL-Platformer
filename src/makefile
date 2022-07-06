CC=gcc
STD=-std=c17
STD_OPT=-g -Wall
MAIN=main.c
INC_SRC=game_config.c front.c front_tools.c player.c level.c calc.c
INC_DIR=-Iinclude
OUTPUT_DIR=bin
OUTPUT_EXEC=$(OUTPUT_DIR)\\main
SDL_LIB=-Ilibraries\SDL\Include -Llibraries\SDL\lib
SDL_OPT=-lmingw32 -lSDL2main -lSDL2


all: compile

run: all
	$(OUTPUT_EXEC)

compile:
	cls
	$(CC) $(STD) $(INC_DIR) $(SDL_LIB) $(STD_OPT) -o $(OUTPUT_EXEC) $(MAIN) $(INC_SRC) $(SDL_OPT)

clean:
	del $(OUTPUT_DIR)\\**.exe