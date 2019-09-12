CC = g++-8
CC_FLAGS = -O3 -Wall -std=c++17
LD_FLAGS =
NAME = sqlite_wrapper
SOURCE_DIR = ./src
SOURCE_FILES = $(shell find -L $(SOURCE_DIR) -type f,l -iregex '.*\.\(c\|i\|ii\|cc\|cp\|cxx\|cpp\|CPP\|c++\|C\|s\|S\|sx\)' )
OUTPUT_DIR = ./output

all: $(OUTPUT_DIR) $(SOURCE_FILES) 
	$(CC) $(CC_FLAGS) $(SOURCE_FILES) -o "$(OUTPUT_DIR)/$(NAME)" $(LD_FLAGS)

.PHONY: clean
clean: 
	rm -rf $(OUTPUT_DIR)

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR)