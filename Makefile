all:
	cc -std=c17 -Wall -Werror -g -O0 -DDEBUG *.c -o chess
	cc -std=c17 -Wall -Werror -g -O3 *.c -o ochess

clean:
	rm -r chess chess.dSYM ochess ochess.dSYM
