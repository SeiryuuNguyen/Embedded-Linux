CURRENT_DIR := .
BIN_DIR := $(CURRENT_DIR)/bin
INC_DIR := $(CURRENT_DIR)/inc
OBJ_DIR := $(CURRENT_DIR)/obj
SRC_DIR := $(CURRENT_DIR)/src

build:
	gcc -c $(CURRENT_DIR)/main.c -o $(OBJ_DIR)/main.o -I $(INC_DIR)
	gcc -c $(SRC_DIR)/commands.c -o $(OBJ_DIR)/commands.o -I $(INC_DIR)
	gcc -c $(SRC_DIR)/connection.c -o $(OBJ_DIR)/connection.o -I $(INC_DIR)
	gcc -c $(SRC_DIR)/network.c -o $(OBJ_DIR)/network.o -I $(INC_DIR)

linker:
	gcc -pthread $(OBJ_DIR)/* -o $(BIN_DIR)/chatp2p
