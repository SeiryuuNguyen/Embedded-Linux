CURRENT_DIR := .
BIN_DIR := $(CURRENT_DIR)/bin
INC_DIR := $(CURRENT_DIR)/inc
LIB_DIR := $(CURRENT_DIR)/lib
OBJ_DIR := $(CURRENT_DIR)/obj
SRC_DIR := $(CURRENT_DIR)/src

STATIC_LIB := $(LIB_DIR)/static
SHARED_LIB := $(LIB_DIR)/shared

LIB_NAME := strutils
PROGRAM_STATIC := main_static
PROGRAM_SHARED := main_shared

all: static shared
static:
	gcc -c $(CURRENT_DIR)/main.c -o $(OBJ_DIR)/main.o -I $(CURRENT_DIR)/inc
	gcc -c $(SRC_DIR)/bstrutils.c -o $(OBJ_DIR)/bstrutils.o -I $(CURRENT_DIR)/inc
	ar rcs $(STATIC_LIB)/libstrutils.a $(OBJ_DIR)/bstrutils.o
	gcc $(OBJ_DIR)/main.o -L $(STATIC_LIB) -l $(LIB_NAME) -o $(BIN_DIR)/$(PROGRAM_STATIC)
shared:
	gcc -c $(CURRENT_DIR)/main.c -o $(OBJ_DIR)/main.o -I $(CURRENT_DIR)/inc
	gcc -c -fPIC $(SRC_DIR)/bstrutils.c -o $(OBJ_DIR)/bstrutils_shared.o -I $(CURRENT_DIR)/inc
	gcc -shared $(OBJ_DIR)/bstrutils_shared.o -o $(SHARED_LIB)/libstrutils.so
	gcc $(OBJ_DIR)/main.o -L $(SHARED_LIB) -l $(LIB_NAME) -o $(BIN_DIR)/$(PROGRAM_SHARED)
clean:
	rm -rf $(SHARED_LIB)/* $(STATIC_LIB)/* $(OBJ_DIR)/* $(BIN_DIR)/*
