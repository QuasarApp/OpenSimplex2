#
# Copyright (C) 2019-2020 KdotJPG (original work, previous license:
# CC0 1.0 Universal, public domain dedication).
#
# Copyright (C) 2026 QuasarApp Group DOO
# Licensed under the GNU Lesser General Public License v3.0 (LGPL-3.0),
# see the accompanying LICENSE file.
#

CFLAGS=-Wall -Wextra --pedantic -O3
# CFLAGS=-Wall -Wextra --pedantic -g -fsanitize=undefined
# CFLAGS=-Wall -Wextra --pedantic -g -fsanitize=address

all:	OpenSimplex2F.o test_OpenSimplex2F test_1d_simple

OpenSimplex2F.o:	OpenSimplex2F.c OpenSimplex2F.h Makefile
	$(CC) ${CFLAGS} -c OpenSimplex2F.c -lm

test_OpenSimplex2F:	tests/test_OpenSimplex2F.c OpenSimplex2F.o Makefile
	$(CC) ${CFLAGS} -I. -o test_OpenSimplex2F tests/test_OpenSimplex2F.c OpenSimplex2F.o -lm -lpng

test_1d_simple:	test_1d_simple.c OpenSimplex2F.o Makefile
	$(CC) ${CFLAGS} -I. -o test_1d_simple test_1d_simple.c OpenSimplex2F.o -lm

clean:
	rm -f *.o test_OpenSimplex2F test2d.png test3d.png test4d.png test_1d_simple


