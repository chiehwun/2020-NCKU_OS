create_data: Clock.o create_data.o
	g++ -pthread -o create_data create_data.o Clock.o

Clock.o: Clock.cpp Clock.h
	g++ -c Clock.cpp

create_data.o: create_data.cpp Clock.h
	g++ -c create_data.cpp

clean:
	rm create_data *.o *.d -v