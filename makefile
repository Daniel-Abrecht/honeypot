
all: bin/honeypot bin/log2json

bin/honeypot: honeypot.c | bin
	gcc -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -pedantic -Werror honeypot.c -o bin/honeypot

bin/log2json: log2json.c | bin
	gcc -Wall -Wextra -pedantic -Werror -std=c11 log2json.c -o bin/log2json

bin:
	mkdir bin || true

clean:
	rm bin/*
