
CC = gcc
CFLAGS = -I../include -MMD -MP

BUILD_DIR = ../build
BIN_DIR = ../bin

TARGET   := myprog

.PHONY: all
all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(BUILD_DIR)/main.o $(BUILD_DIR)/module1/foo.o $(BUILD_DIR)/module2/bar.o
	$(CC) -o $(BIN_DIR)/$(TARGET) $(BUILD_DIR)/main.o $(BUILD_DIR)/module1/foo.o $(BUILD_DIR)/module2/bar.o

$(BUILD_DIR)/main.o: main.c | $(BUILD_DIR) $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/module1/foo.o:
	$(MAKE) -C module1

$(BUILD_DIR)/module2/bar.o:
	$(MAKE) -C module2

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

-include $(BUILD_DIR)/*.d

.PHONY: clean
clean:
	$(MAKE) -C module1 clean
	$(MAKE) -C module2 clean
	rm -f $(BUILD_DIR)/main.o
	rm -f $(BIN_DIR)/$(TARGET)
