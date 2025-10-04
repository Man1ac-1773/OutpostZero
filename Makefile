CXX = g++
CXXFLAGS = -Wall -I$(SRC_DIR)
LDLIBS = -lraylib

BUILD_DIR = build

TARGET = $(BUILD_DIR)/output

SRC_DIR := src
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')

OBJ_DIR := $(BUILD_DIR)/obj
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR) 
	$(CXX) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean