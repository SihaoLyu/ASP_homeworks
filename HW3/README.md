# Homework 3
## Name: Sihao Lyu
## UFID: 1888-8483

For this assignment, I only build a file named transProg.cpp, all outcomes will print at the screen. Each function is fully tested. The main thread takes transfer requests and puts them into a public task pool which is a FIFO queue and can be accessed by all working threads. To ensure the transfer sequence, each work thread needs to ensure both transmitter and receiver are ready to process by using a bool condition variable array.

For the testfile, I need the each number and word is splited by a space, and each transfer record needs to start with "Transfer". A sample test file is offered.

Usage: When compiling please add "-std=c++11" and "-pthread". transProg will expect 3 arguments, where argv[1] is the test file path and argv[2] is the number of working threads. An example is: "./transProg testfile 3".