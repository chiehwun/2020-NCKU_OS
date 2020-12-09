all: gen.out hw3.out test_mem.out

db.o: db.cpp db.h
	g++ -c db.cpp -g

BPTree.o: BPTree.cpp BPTree.h
	g++ -c BPTree.cpp -g

Clock.o: Clock.cpp Clock.h
	g++ -c Clock.cpp -g

hw3.o: hw3.cpp db.h BPTree.h
	g++ -c hw3.cpp -g

hw3.out: hw3.o db.o BPTree.o
	g++ -o hw3.out hw3.o db.o BPTree.o -Wall -g

test_mem.o: test_mem.cpp BPTree.h Clock.h
	g++ -c test_mem.cpp -g

test_mem.out: test_mem.o BPTree.o Clock.o
	g++ -o test_mem.out test_mem.o BPTree.o Clock.o -Wall -g

gen.out: gen.cpp
	g++ -o gen.out gen.cpp

clean:
	rm *.out *.o -v

cleanmem:
	sudo sh -c 'free -m && echo 1 > /proc/sys/vm/drop_caches && free -m'