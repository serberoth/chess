all:
	cc -g -O0 -DDEBUG *.c -o chess
	cc -g -O0 *.c -o ochess

clean:
	rm -r chess chess.dSYM ochess
