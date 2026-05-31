# ESP32 Arduino build helper

BOARD = esp32:esp32:esp32
SRC = src
BUILD_DIR = build

all:
	arduino-cli compile --fqbn $(BOARD) --output-dir $(BUILD_DIR) $(SRC)

clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean