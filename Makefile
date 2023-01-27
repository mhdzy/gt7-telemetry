# compiler and flags
CC := g++
CCFLAGS := -Wall -std=c++20

# need to set '-lstdc++fs' on linux, breaks macOS builds
OS_NAME := $(shell uname -s | tr A-Z a-z)
OS_FLAGS := -lzmq
ifeq ($(OS_NAME), "linux")
	OS_FLAGS += -lstdc++fs
endif

# source and install dirs
BUILD_DIR   := build
INCLUDE_DIR := include
SOURCE_DIR  := src

# target executable
TARGET := main

# some included libraries (spdlog)
INC := /usr/local/include /usr/local/bin/R/include
INC_LIBS := $(addprefix -I,$(INC))

# auto-detect source .cpp files and derive .o names
# assumes corresponding .hpp files exist in the $(INCLUDE_DIR)
SOURCES := $(wildcard $(SOURCE_DIR)/*.cpp)
OBJECTS := $(patsubst $(SOURCE_DIR)/%,%,$(SOURCES))
OBJECTS := $(patsubst %.cpp,%.o,$(OBJECTS))
OBJECTS := $(addprefix $(BUILD_DIR)/,$(OBJECTS))

# setup 'make run' args
ifeq (run, $(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2, $(words $(MAKECMDGOALS)), $(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) -g -o $@ $(INC_LIBS) $(OS_FLAGS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(INCLUDE_DIR)/%.hpp
	$(CC) $(CCFLAGS) -c -g $(INC_DIRS) $< -o $@

clean: trim
	@echo "cleaning build files and executables"
	rm -f $(BUILD_DIR)/* $(TARGET)

run:
	@echo "prog $(RUN_ARGS)"
	@chmod +x $(TARGET)
	./$(TARGET) $(RUN_ARGS)

.PHONY: clean run trim

