all:
	cc -g -O0 -DDEBUG *.c -o chess
	cc -g -O3 *.c -o ochess

clean:
	rm -r chess chess.dSYM ochess ochess.dSYM

