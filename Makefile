CXX      := -gcc
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror -O3 $(shell pkg-config --cflags bullet)
#CXXFLAGS += -m64 -march=native
#CXXFLAGS += -fopt-info-vec-optimized #-fopt-info-vec-missed -ftree-vectorize
LDFLAGS  := -L/usr/lib -lstdc++ -lm $(shell pkg-config --libs bullet)
LDXX_FLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl

OBJ_DIR  := ./build
BIN_DIR  := ./bin
TARGET   := game
INCLUDE  := -Iinclude/
SRC      := $(wildcard src/*.cc)

OBJECTS  := $(SRC:%.cc=$(OBJ_DIR)/%.o)
DEPENDENCIES \
         := $(OBJECTS:.o=.d)

all: build $(BIN_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(BIN_DIR)/$(TARGET) $^ $(LDFLAGS) $(LDXX_FLAGS)

-include $(DEPENDENCIES)

.PHONY: all build clean debug release info

debug: CXXFLAGS:= $(filter-out -O3,$(CXXFLAGS)) -DDEBUG -g -fsanitize=address
debug: all

build:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)


exe: all
	$(BIN_DIR)/$(TARGET)

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(BIN_DIR)/*
