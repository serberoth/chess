all:
	cc -g -O0 -DDEBUG *.c -o chess
	cc -O3 *.c -o ochess

clean:
	rm -r chess chess.dSYM ochess
