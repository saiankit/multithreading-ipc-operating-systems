a. Write a multi-threaded C program (P1) that reads a file containing space-separated positive integers in ASCII format. Please generate more input files in order to generate the plots as required below. The number of integers can be arbitrarily large (upto 1 million integers).

- Different threads should read different parts of the file. Vary the number of threads from 1… to arbitrarily large
- Record the time that it takes to read the entire file into memory with different number of threads (1, 2, … n)
- Plot time against the number of threads for different numbers of integers in the input file. Analyse the findings from the plots.

b. Write a C program (P2) which uses IPC mechanisms to receive the numbers read in the program in part (a). This program spawns multiple threads to sum the numbers passed by the program P1 in part (a). The program prints the sum on the terminal.

- Vary the number of threads from 1… to arbitrarily large. Different threads can calculate the partial sums of the series
- Record the time it takes to sum the integers with different number of threads
- Plot the time against the number of threads for different numbers of integers. Analyse the findings from the plots.
