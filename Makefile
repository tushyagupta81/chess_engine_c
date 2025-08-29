CC=gcc
CFLAGS=-Wall -Wextra -Werror -I$(INCLUDE_DIR) -MMD -MP

BUILD_DIR=build
SRC_DIR=src
INCLUDE_DIR=include

SRCS=$(wildcard $(SRC_DIR)/*.c)
OBJS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS=$(OBJS:.o=.d)
TARGET=$(BUILD_DIR)/chess_engine

default: all

all: $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

clean:
	rm -r ./build

run: all
	@./$(TARGET)

-include $(DEPS)
