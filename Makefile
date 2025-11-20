CC=clang

BASE_CFLAGS=-std=c23 -Wall -Werror -Wpedantic -I$(SRC_DIR) -D_POSIX_C_SOURCE=200809L

ifneq (DEBUG,)
CFLAGS=$(BASE_CFLAGS) $(ACFLAGS) -g3
else
CFLAGS=$(BASE_CFLAGS) $(ACFLAGS)
endif

ifneq (DEBUG,)
FINAL_CFLAGS=$(CFLAGS) -fsanitize=address
else
FINAL_CFLAGS=$(CFLAGS)
endif

COMPILE=$(CC) $(CFLAGS)

SRC_DIR=src
BUILD_DIR=build
TEST_DIR=test

.PHONY: test

test: $(BUILD_DIR)/test

$(BUILD_DIR)/tokenizer.o: $(SRC_DIR)/tokenizer.c $(SRC_DIR)/tokenizer.h
	$(COMPILE) -c $< -o $@

$(BUILD_DIR)/utils.o: $(SRC_DIR)/utils.c $(SRC_DIR)/utils.h
	$(COMPILE) -c $< -o $@

$(BUILD_DIR)/test.o: $(TEST_DIR)/test.c 
	$(COMPILE) -c $< -o $@

$(BUILD_DIR)/test: $(BUILD_DIR)/test.o $(BUILD_DIR)/utils.o $(BUILD_DIR)/tokenizer.o
	$(CC) $(FINAL_CFLAGS) $^ -o $@

