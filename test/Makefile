RLCPPINC=../../cpp-readline/src
RLCPPLIB=../../cpp-readline/lib
LIBNAME=rf24-sunxi
LIBDIR=../lib/

all:pingtest scanner bipingtest registers gpio

registers: registers.cpp
	g++ registers.cpp -DGPIO_SUN7I -I../src/ -L${LIBDIR} -l${LIBNAME} -o registers

pingtest: pingtest.cpp
	g++ pingtest.cpp -DGPIO_SUN7I -I../src/ -L${LIBDIR} -l${LIBNAME} -o pingtest

bipingtest: bipingtest.cpp
	g++ bipingtest.cpp -DGPIO_SUN7I -I../src/ -L${LIBDIR} -l${LIBNAME} -o bipingtest

scanner: scanner.cpp
	g++ scanner.cpp -DGPIO_SUN7I -I../src/ -L${LIBDIR} -l${LIBNAME} -o scanner

gpio: gpio.cpp
	g++ -std=c++11 gpio.cpp -DGPIO_SUN7I -I../src/ -I${RLCPPINC} -L${LIBDIR} -l${LIBNAME} -L${RLCPPLIB} -lcpprl -lreadline -o gpio

clean:
	rm -f scanner pingtest bipingtest registers gpio
