BUILD_DIR := build
TARGET := EverEngineEditor
BIN_DIR := $(BUILD_DIR)/bin
EXECUTABLE := $(BIN_DIR)/$(TARGET)

all: build

build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ..
	$(MAKE) -C $(BUILD_DIR)

rebuild: clean build

clean:
	rm -rf $(BUILD_DIR)

run: build
	$(EXECUTABLE)

.PHONY: all build rebuild clean run
