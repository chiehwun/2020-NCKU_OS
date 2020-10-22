all: test_thread test_infile_mult limit_of_file_pointer

test_thread: test_thread.cpp
	g++ -pthread -o test_thread test_thread.cpp

test_infile_mult: test_infile_mult.cpp
	g++ -pthread -o test_infile_mult test_infile_mult.cpp

limit_of_file_pointer: limit_of_file_pointer.cpp
	g++ -o limit_of_file_pointer limit_of_file_pointer.cpp

clean:
	rm test_thread test_infile_mult limit_of_file_pointer -v