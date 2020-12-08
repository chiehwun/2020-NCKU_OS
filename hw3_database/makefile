all: gen.out hw3.out

gen.out: gen.cpp
	g++ -o gen.out gen.cpp

db.o: db.cpp db.h
	g++ -c db.cpp -g

BPTree.o: BPTree.cpp BPTree.h
	g++ -c BPTree.cpp -g

hw3.o: hw3.cpp db.h BPTree.h
	g++ -c hw3.cpp -g

hw3.out: hw3.o db.o BPTree.o
	g++ -o hw3.out hw3.o db.o BPTree.o -Wall -g

clean:
	rm *.out *.o -v

cleanmem:
	sudo sh -c 'free -m && echo 1 > /proc/sys/vm/drop_caches && free -m'