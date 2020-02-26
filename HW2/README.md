# Homework 2
## Name: Sihao Lyu
## UFID: 1888-8483

For this assignment, I use 3 files like last assignment, which are combiner.c,
mapper.c and reducer.c. All public and shared varaibles are stored at combiner, and mapper and reducer use extern variables to declare them rather
than including combiner.c. Each file is fully tested. Since thread number equals to user number, it is critical to input a thread number larger than actual user number, every bad number will be denied and terminate all threads.

Usage: combiner will expect 3 arguments , where argv[1] is the buffer size for each thread and argv[2] is the number of all reducer threads. Mapper will read records from stdin, so use redirection for an input test file. An Example is: "./combiner 5 10 < input.txt"