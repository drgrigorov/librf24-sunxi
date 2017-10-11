RLCPPINC=../../cpp-readline/src
RLCPPLIB=../../cpp-readline/lib
LIBNAME=rf24-sunxi
LIBDIR=../lib/

CCFLAGS=-O0 -ggdb -mfpu=vfp -mfloat-abi=hard

all:pingtest scanner bipingtest registers gpio gpiodump

registers: registers.cpp
	g++ registers.cpp -DGPIO_SUN7I -I../src/ -L${LIBDIR} -l${LIBNAME} ${CCFLAGS} -o registers

pingtest: pingtest.cpp
	g++ pingtest.cpp -DGPIO_SUN7I -I../src/ -L${LIBDIR} -l${LIBNAME} ${CCFLAGS} -o pingtest

bipingtest: bipingtest.cpp
	g++ bipingtest.cpp -DGPIO_SUN7I -I../src/ -L${LIBDIR} -l${LIBNAME} ${CCFLAGS} -o bipingtest

scanner: scanner.cpp
	g++ scanner.cpp -DGPIO_SUN7I -I../src/ -L${LIBDIR} -l${LIBNAME} ${CCFLAGS} -o scanner

gpio: gpio.cpp
	g++ -std=c++11 gpio.cpp -DGPIO_SUN7I -I../src/ -I${RLCPPINC} -L${LIBDIR} -l${LIBNAME} -L${RLCPPLIB} -lcpprl -lreadline ${CCFLAGS} -o gpio

gpiodump: gpiodump.cpp
	g++ -std=c++11 gpiodump.cpp -DGPIO_SUN7I -I../src/ -I${RLCPPINC} -L${LIBDIR} -l${LIBNAME} -L${RLCPPLIB} -lcpprl -lreadline ${CCFLAGS} -o gpiodump
clean:
	rm -f scanner pingtest bipingtest registers gpio
