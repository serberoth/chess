all:
	cc -g -O0 -DDEBUG *.c -o chess


clean:
	rm -r chess chess.dSYM
