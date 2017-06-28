LIBNAME=rf24-sunxi
LIBDIR=../lib/

all:pingtest scanner bipingtest registers

registers: registers.cpp
	g++ registers.cpp -DGPIO_SUN7I -I../src/ -L${LIBDIR} -l${LIBNAME} -o registers

pingtest: pingtest.cpp
	g++ pingtest.cpp -DGPIO_SUN7I -I../src/ -L${LIBDIR} -l${LIBNAME} -o pingtest

bipingtest: bipingtest.cpp
	g++ bipingtest.cpp -DGPIO_SUN7I -I../src/ -L${LIBDIR} -l${LIBNAME} -o bipingtest

scanner: scanner.cpp
	g++ scanner.cpp -DGPIO_SUN7I -I../src/ -L${LIBDIR} -l${LIBNAME} -o scanner

clean:
	rm -f scanner pingtest bipingtest registers
