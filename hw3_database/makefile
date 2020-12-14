# all: gen.out test_bptree.out test_mem.out test_db.out hw3.out
all: hw3.out

# Header Files
BPTree.o: BPTree.cpp BPTree.h
	g++ -c BPTree.cpp -g

db.o: db.cpp db.h
	g++ -c db.cpp -g

Clock.o: Clock.cpp Clock.h
	g++ -c Clock.cpp -g

# Driver Code Files
gen.out: gen.cpp
	g++ -o gen.out gen.cpp

# test_bptree.o: test_bptree.cpp db.h
# 	g++ -c test_bptree.cpp -g
# test_bptree.out: test_bptree.o db.o BPTree.o
# 	g++ -o test_bptree.out test_bptree.o db.o BPTree.o -Wall -g

# test_mem.o: test_mem.cpp BPTree.h Clock.h
# 	g++ -c test_mem.cpp -g
# test_mem.out: test_mem.o BPTree.o Clock.o
# 	g++ -o test_mem.out test_mem.o BPTree.o Clock.o -Wall -g

# test_db.o: test_db.cpp db.h Clock.h
# 	g++ -c test_db.cpp -g
# test_db.out: test_db.o db.o BPTree.o Clock.o
# 	g++ -o test_db.out test_db.o db.o BPTree.o Clock.o -Wall -g

hw3.o: hw3.cpp db.h Clock.h
	g++ -c hw3.cpp -g
hw3.out: hw3.o db.o BPTree.o Clock.o
	g++ -o hw3.out hw3.o db.o BPTree.o Clock.o -Wall -g

clean:
	rm *.out *.o *output /storage/*.file -v

cleanmem:
	sudo sh -c 'free -m && echo 1 > /proc/sys/vm/drop_caches && free -m'