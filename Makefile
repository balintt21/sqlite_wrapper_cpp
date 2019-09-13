CC = gcc-8
CC_FLAGS = -O3 -Wall
CXX = g++-8
CXX_FLAGS = -O3 -Wall -Wextra -Wshadow -std=c++17 -isystem ./third-party
LD_FLAGS = -lpthread -ldl
NAME = sqlite_wrapper
SOURCE_DIR = ./src
SOURCE_FILES = $(shell find -L $(SOURCE_DIR) -type f,l -iregex '.*\.\(c\|i\|ii\|cc\|cp\|cxx\|cpp\|CPP\|c++\|C\|s\|S\|sx\)' )
SQLITE_SRC = ./third-party/sqlite3.c
SQLITE_OBJ = $(OUTPUT_DIR)/sqlite3.o
OUTPUT_DIR = ./output

all: $(OUTPUT_DIR) $(SOURCE_FILES) $(SQLITE_OBJ)
	$(CXX) $(CXX_FLAGS) $(SOURCE_FILES) $(SQLITE_OBJ) -o "$(OUTPUT_DIR)/$(NAME)" $(LD_FLAGS)

$(SQLITE_OBJ): $(SQLITE_SRC)
	$(CC) $(CC_FLAGS) $(SQLITE_SRC) -c -o $(SQLITE_OBJ)

.PHONY: clean
clean: 
	rm -rf $(OUTPUT_DIR)

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR)