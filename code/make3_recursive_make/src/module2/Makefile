CC = gcc
CFLAGS = -I../../include -MMD -MP

BUILD_DIR = ../../build/module2

.PHONY: all
all: $(BUILD_DIR)/bar.o

$(BUILD_DIR)/bar.o: bar.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

-include $(BUILD_DIR)/*.d

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
