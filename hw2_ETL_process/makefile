all: create_data.out converter.out

Clock.o: Clock.cpp Clock.h
	g++ -c Clock.cpp

converter.o: converter.cpp Clock.h
	g++ -c converter.cpp

converter.out: Clock.o converter.o
	g++ -pthread -o converter.out converter.o Clock.o -lstdc++fs

create_data.o: create_data.cpp Clock.h
	g++ -c create_data.cpp

create_data.out: Clock.o create_data.o
	g++ -pthread -o create_data.out create_data.o Clock.o

clean:
	rm *.out *.o *.thdout *.log output.txt -v

cleanw:
	rm *.exe *.o *.d *.thdout *.log output.txt -v

cleanmem:
	sudo sh -c 'free -m && echo 1 > /proc/sys/vm/drop_caches && free -m'