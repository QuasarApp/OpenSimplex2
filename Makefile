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

all:	OpenSimplex2F.o OpenSimplex2F_1D.o test_OpenSimplex2F test_OpenSimplex2F_1D

OpenSimplex2F.o:	OpenSimplex2F.c OpenSimplex2F.h Makefile
	$(CC) ${CFLAGS} -c OpenSimplex2F.c -lm

OpenSimplex2F_1D.o:	OpenSimplex2F_1D.c OpenSimplex2F_1D.h Makefile
	$(CC) ${CFLAGS} -c OpenSimplex2F_1D.c -lm

test_OpenSimplex2F:	tests/test_OpenSimplex2F.c OpenSimplex2F.o Makefile
	$(CC) ${CFLAGS} -I. -o test_OpenSimplex2F tests/test_OpenSimplex2F.c OpenSimplex2F.o -lm -lpng

test_OpenSimplex2F_1D:	tests/test_OpenSimplex2F_1D.c OpenSimplex2F.o OpenSimplex2F_1D.o Makefile
	$(CC) ${CFLAGS} -I. -o test_OpenSimplex2F_1D tests/test_OpenSimplex2F_1D.c OpenSimplex2F.o OpenSimplex2F_1D.o -lm

debug_1d:	debug_1d.c OpenSimplex2F_1D.c OpenSimplex2F_1D.h OpenSimplex2F.h Makefile
	$(CC) ${CFLAGS} -I. -o debug_1d debug_1d.c OpenSimplex2F_1D.c -lm

debug_floor:	debug_floor.c debug_floor2.c OpenSimplex2F_1D.c OpenSimplex2F_1D.h OpenSimplex2F.h Makefile
	$(CC) ${CFLAGS} -I. -o debug_floor debug_floor.c debug_floor2.c OpenSimplex2F_1D.c -lm

debug_floor2:	debug_floor2.c OpenSimplex2F_1D.c OpenSimplex2F_1D.h OpenSimplex2F.h Makefile
	$(CC) ${CFLAGS} -I. -o debug_floor2 debug_floor2.c OpenSimplex2F_1D.c -lm

debug_floor3:	debug_floor3.c OpenSimplex2F_1D.c OpenSimplex2F_1D.h OpenSimplex2F.h Makefile
	$(CC) ${CFLAGS} -I. -o debug_floor3 debug_floor3.c OpenSimplex2F_1D.c -lm

debug_real:	debug_real.c OpenSimplex2F_1D.c OpenSimplex2F_1D.h OpenSimplex2F.h Makefile
	$(CC) ${CFLAGS} -I. -o debug_real debug_real.c OpenSimplex2F_1D.c -lm

debug_xs:	debug_xs.c OpenSimplex2F_1D.c OpenSimplex2F_1D.h OpenSimplex2F.h Makefile
	$(CC) ${CFLAGS} -I. -o debug_xs debug_xs.c OpenSimplex2F_1D.c -lm

debug_xs2:	debug_xs2.c OpenSimplex2F_1D.c OpenSimplex2F_1D.h OpenSimplex2F.h Makefile
	$(CC) ${CFLAGS} -I. -o debug_xs2 debug_xs2.c OpenSimplex2F_1D.c -lm

clean:
	rm -f *.o test_OpenSimplex2F test_OpenSimplex2F_1D debug_1d debug_floor debug_floor2 debug_floor3 debug_real debug_xs debug_xs2 test2d.png test3d.png test4d.png


