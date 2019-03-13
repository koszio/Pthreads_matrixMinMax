# Pthreads_matrixMinMax
Calculate Minimum and Maximum value of a matrix using Ptheads

(a) The program calculates the sum, it finds and prints a value and a position (indexes) of the maximum element of the matrix,
and a value and a position of the minimum element of the matrix. Initialize elements of the matrix to random values to check the solution. 

(b) Change the program developed in (a) so that the main thread prints the final results. In that implementation there is no call to Barrier function, and do no use of arrays for partial results, such as sums.

(c) The program developed in (b) is changed so that it uses a "bag of tasks" that is represented as a row counter, which is initialized to 0. A worker gets a task (i.e. the number of the row to process) out of the bag by reading a value of the counter and incrementing the counter.
