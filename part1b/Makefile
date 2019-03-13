target: matrixMaxMin.c
	gcc -o minMax matrixMaxMin.c -pthread -lm

.PHONY: clean run init

run:
	./minMax

init:
	rm minMax 

clean:
	rm -f *.o
