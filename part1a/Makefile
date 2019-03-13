target: matrixMaxMin.c
	gcc -o minMax matrixMaxMin.c -pthread -lm

.PHONY: clean run init

run:
	./matrixMaxMin 

init:
	rm matrixMaxMin

clean:
	rm -f *.o
