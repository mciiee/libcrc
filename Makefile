CC=clang
CFLAGS=-std=c23 -Wall -Werror -Wpedantic
COMPILE=$(CC) $(CFLAGS)

SRC_DIR=src
BUILD_DIR=build
TEST_DIR=test


$(BUILD_DIR)/tokenizer.o: $(SRC_DIR)/tokenizer.c $(SRC_DIR)/tokenizer.h
	$(COMPILE) -c $< -o $@

