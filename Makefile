.PHONY: all clean url-test

all: url-test

clean:
	rm -rvf url-test *.o url_char_category_table.h

%.o: %.c
	$(CC) -g -std=c99 -Wall -c -o $@ $^

url.c: url.h url_char_category.h url_char_category_table.h

url_char_category_table.h: gen_char_category_table
	./gen_char_category_table > url_char_category_table.h

gen_char_category_table: gen_char_category_table.o
	$(CC) -g -std=c99 -Wall -o $@ $^

url-test: test.o url.o
	$(CC) -g -std=c99 -Wall -I. -o $@ $^
	./url-test

