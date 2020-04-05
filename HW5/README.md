# Homework 5

## Name: Sihao Lyu

## UFID: 1888-8483

2 files are included: original userapp.c file plus a test.c file to test the synchronization function in one device (/dev/mycdrv0). It creates 3 threads, where thread 0 writes a mark "2" in ramdisk to indicate thread 2, while thread 1 and 2 is keeping reading from the ramdisk. After thread 2 reads "2", it writes "-2 into ramdisk to indicate thread 0 and finishes. And then thread 1 reads "-1" and writes "1" to indicate thread 1 and finishes. Lastly thread 1 reads the mark "1" and finishes. The whole process illustrates no dead lock in the codes.
