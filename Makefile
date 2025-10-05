# -----------------------------
# Compiler settings
# -----------------------------
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -Werror -I$(INCLUDE_DIR) -I$(UNITY_DIR) -MMD -MP
CXXFLAGS = -std=c++17 -O2 -I$(INCLUDE_DIR) -I$(LIBTORCH_PATH)/include -I$(LIBTORCH_PATH)/include/torch/csrc/api/include -MMD -MP

# Libraries and paths
LIBTORCH_PATH = $(CURDIR)/libtorch
LDFLAGS = -L$(LIBTORCH_PATH)/lib -ltorch -ltorch_cpu -lc10 -Wl,-rpath,$(LIBTORCH_PATH)/lib

# -----------------------------
# Directories
# -----------------------------
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
UNITY_DIR = unity
LIBTORCH_PATH = $(CURDIR)/libtorch

# -----------------------------
# Sources and objects
# -----------------------------
C_SRCS = $(wildcard $(SRC_DIR)/*.c)
C_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRCS))

CPP_SRCS = $(wildcard $(SRC_DIR)/*.cpp)
CPP_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CPP_SRCS))

TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/%.test.o,$(TEST_SRCS))

UNITY_SRC = $(UNITY_DIR)/unity.c
UNITY_OBJ = $(BUILD_DIR)/unity.o

TARGET = $(BUILD_DIR)/chess_engine
TEST_TARGET = $(BUILD_DIR)/tests

LIB_OBJS = $(filter-out $(BUILD_DIR)/main.o,$(C_OBJS))

# -----------------------------
# Default
# -----------------------------
default: all
all: $(TARGET)

# -----------------------------
# Build directories
# -----------------------------
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# -----------------------------
# Compile C source files
# -----------------------------
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test C files
$(BUILD_DIR)/%.test.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Unity
$(UNITY_OBJ): $(UNITY_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(UNITY_SRC) -o $(UNITY_OBJ)

# -----------------------------
# Link main target (engine)
# -----------------------------
$(TARGET): $(C_OBJS) $(CPP_OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Link test target
$(TEST_TARGET): $(LIB_OBJS) $(TEST_OBJS) $(CPP_OBJS) $(UNITY_OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)

# -----------------------------
# Clean
# -----------------------------
clean:
	rm -rf $(BUILD_DIR)

# -----------------------------
# Run
# -----------------------------
run: $(TARGET)
	@./$(TARGET)

test: $(TEST_TARGET)
	@./$(TEST_TARGET)

# Include dependency files
-include $(C_OBJS:.o=.d)
-include $(CPP_OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)
