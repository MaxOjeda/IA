flags=-Wno-deprecated -g -ggdb
comp=g++
prog=VRPB+FC+CBJ

ins ?= GA1.txt

comp:$(prog)

exe:$(prog)
	./$(prog) $(ins)

$(prog): main.o
	$(comp) main.o -o $(prog) $(flags)

main.o: main.cpp includes.h global.h
	$(comp) -c main.cpp $(flags)

clean:
	rm -f *~
	rm -f *.o
	rm -f $(prog)
	rm -f core*
