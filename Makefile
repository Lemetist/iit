CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic
CPPFLAGS += $(shell pkg-config --cflags cairo x11)
LDLIBS += $(shell pkg-config --libs cairo x11)

.PHONY: all test
all: histogram

histogram: main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

tests: tests.c main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -Werror tests.c -o $@ $(LDFLAGS) $(LDLIBS)

test: histogram tests
	./tests
	python3 tests_integration.py
