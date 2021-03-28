
all: clean test

clean:
	rm -rf url-test

test: test.c
	$(CC) -std=c99 -Wall -I. url.c $< -o url-test
	./url-test

.PHONY: all clean test
