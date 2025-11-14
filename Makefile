CXX = g++
CXXFLAGS = -Wall -I$(SRC_DIR) -I$(LIB_DIR) -MMD -MP

# The next line is for debugging purposes, when using gdb
# DEBUG_FLAGS = -g -O0 
CXXFLAGS += $(DEBUG_FLAGS)
LDFLAGS = $(DEBUG_FLAGS)

BUILD_DIR = build

TARGET = $(BUILD_DIR)/output
RAYLIB_DIR := lib/lib/raylib/src
RAYLIB_LIB := $(RAYLIB_DIR)/libraylib.a

LDLIBS = $(RAYLIB_LIB) -lm -lpthread -ldl

SRC_DIR := src
LIB_DIR := lib
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')

OBJ_DIR := $(BUILD_DIR)/obj
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

DEPS := $(OBJS:.o=.d)

all: $(RAYLIB_LIB) $(TARGET)

$(RAYLIB_LIB):
	$(MAKE) -C $(RAYLIB_DIR)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR) 
	$(CXX) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean

-include $(DEPS)
