flags=-Wno-deprecated -g -ggdb
comp=g++
prog=VRPB+FC+CBJ

#El problema
entrada=Instancias/GM1.txt
salida=GA1.txt
debug=1

comp:$(prog)

exe:$(prog)
	#valgrind --leak-check=full --show-reachable=yes 
	./$(prog) $(entrada) $(salida) $(debug)

$(prog): main.o
	$(comp) main.o -o $(prog) $(flags)

main.o: main.cpp includes.h global.h
	$(comp) -c main.cpp $(flags)

clean:
	rm -f *~
	rm -f *.o
	rm -f $(prog)
	rm -f core*
