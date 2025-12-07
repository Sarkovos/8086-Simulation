##################################
# Declare Makefile Variables
##################################

CC = g++
LANG_STD = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors
SRC_FILES = src/*.cpp
OBJ_NAME = 8086

##################################
# Declare Makefile Rules
##################################
build:
	$(CC) $(COMPILER_FLAGS) $(LANG_STD) $(SRC_FILES) -o $(OBJ_NAME)
run:
	./$(OBJ_NAME)

clean:
	rm $(OBJ_NAME)