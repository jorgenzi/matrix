CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror
LDFLAGS =
REPORT_DIR = report
TEST_BIN = test_suite
TEST_SRC = test/test_matrix.c  
SRC = matrix.c
OBJ = $(SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o) 

PKG_CONFIG_CHECK = $(shell pkg-config --exists check && echo yes)
ifeq ($(PKG_CONFIG_CHECK),yes)
	CHECK_CFLAGS = $(shell pkg-config --cflags check)
	CHECK_LIBS = $(shell pkg-config --libs check)
else
	CHECK_CFLAGS =
	CHECK_LIBS = -lcheck -lm -lpthread -lrt
endif

ifdef COVERAGE
	CFLAGS += --coverage
	LDFLAGS += --coverage
endif

all: matrix.a

matrix.a: $(OBJ)
	ar rcs $@ $^

%.o: %.c matrix.h
	$(CC) $(CFLAGS) -c $< -o $@

test/%.o: test/%.c matrix.h
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) -c $< -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_OBJ) matrix.a matrix.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< matrix.a $(CHECK_LIBS) -o $@

gcov_report:
	$(MAKE) clean
	$(MAKE) COVERAGE=1 $(TEST_BIN)
	./$(TEST_BIN)
	lcov --directory . --capture --output-file coverage.info
	genhtml coverage.info --output-directory $(REPORT_DIR)

clean:
	rm -f $(OBJ) $(TEST_OBJ) matrix.a $(TEST_BIN)
	rm -f *.gcno *.gcda test/*.gcno test/*.gcda
	rm -rf $(REPORT_DIR) coverage.info

rebuild: clean all test

.PHONY: all clean test gcov_report rebuild
