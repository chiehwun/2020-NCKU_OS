all: create_data converter

Clock.o: Clock.cpp Clock.h
	g++ -c Clock.cpp

converter.o: converter.cpp Clock.h
	g++ -c converter.cpp

converter: Clock.o converter.o
	g++ -pthread -o converter converter.o Clock.o -lstdc++fs

create_data.o: create_data.cpp Clock.h
	g++ -c create_data.cpp

create_data: Clock.o create_data.o
	g++ -pthread -o create_data create_data.o Clock.o

clean:
	rm create_data converter *.o *.thdout *.log output.txt -v

cleand:
	rm create_data converter *.o *.d -v