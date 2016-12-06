
OPTIONS = -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -pedantic -Werror


all: bin/login_logger bin/log2json

bin/%: %.c | bin
	gcc $(OPTIONS) -Werror "$^" -o "$@"

bin:
	mkdir bin || true

clean:
	rm -f bin/*
