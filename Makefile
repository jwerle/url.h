.PHONY: all clean url-test
all: clean url-test


clean:
	rm -rf url-test

url.c: url_char_category.h

url_char_category_table.h: gen_char_category_table
	./gen_char_category_table > url_char_category_table.h

gen_char_category_table: gen_char_category_table.c
	$(CC) -g -std=c99 -Wall -o $@ $^

url-test: test.c
	$(CC) -g -std=c99 -Wall -I. url.c $< -o url-test
	./url-test

