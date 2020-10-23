all: test_thread.out test_infile_mult.out limit_of_file_pointer.out

test_thread.out: test_thread.cpp
	g++ -pthread -o test_thread.out test_thread.cpp

test_infile_mult.out: test_infile_mult.cpp
	g++ -pthread -o test_infile_mult.out test_infile_mult.cpp

limit_of_file_pointer.out: limit_of_file_pointer.cpp
	g++ -o limit_of_file_pointer.out limit_of_file_pointer.cpp

clean:
	rm *.out -v