CC=g++
FLAGS=--std=c++17 -O3
GP=gprof
AS=nasm

dist:
	$(CC) $(FLAGS) dist.cpp -o dist
	./dist
	find -name "*_dist.csv" -exec ./graph.py {} \; 

prof:
	$(CC) $(FLAGS) meas.cpp -o meas
	time -p ./meas
	$(CC) $(FLAGS) -pg meas.cpp -o meas
	./meas
	$(GP) meas gmon.out | python3 -m gprof2dot -s | dot -Tpng -o prof.png

asmprof:
	$(AS) -f elf64 xor_hash.s -o xor_hash.o
	$(AS) -f elf64 strcmp.s -o strcmp.o
	$(CC) $(FLAGS) -DASMOPTIMIZATION meas.cpp xor_hash.o strcmp.o -o meas
	time -p ./meas
	$(CC) $(FLAGS) -DASMOPTIMIZATION -pg meas.cpp xor_hash.o strcmp.o -o meas
	./meas
	$(GP) meas gmon.out | python3 -m gprof2dot -s | dot -Tpng -o asmprof.png

clean:
	rm -f dist meas gmon.out xor_hash.o strcmp.o
