all: gen.out hw4.out

gen.out: gen.cpp
	g++ -o gen.out gen.cpp

db.o: db.cpp db.h
	g++ -c db.cpp -gxt_sort

BPTree.o: BPTree.cpp BPTree.h
	g++ -c BPTree.cpp -g

hw4.o: hw4.cpp db.h BPTree.h
	g++ -c hw4.cpp -g

hw4.out: hw4.o db.o BPTree.o
	g++ -o hw4.out hw4.o db.o BPTree.o -Wall -g

clean:
	rm *.out *.o -v

cleanmem:
	sudo sh -c 'free -m && echo 1 > /proc/sys/vm/drop_caches && free -m'