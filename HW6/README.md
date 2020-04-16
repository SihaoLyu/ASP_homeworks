# Homework 6

## Name: Sihao Lyu

## UFID: 1888-8483

In this assignment, I created 4 user-space programs which are prefixed with "user_test_" to test each dealock scenario. I choosed original assignment6.c file to verify that.

Deadlocks:

In user_test_1, main function creates 2 threads who nearly open the dd file at the same time and then send IOCMODE2 simultaneously (use sleep() to ensure the faster thread will not send IOCMODE2 before the slower opening dd file), and two threads enter queue1 meaning no other thread is able to wake them up.

In user_test_2, thread 1 firstly sends IOCMODE2 before thread 2 created and set the dd as mode2 successfully (line 19). Then both threads try to enter IOCMODE1 in order (line 21 & 32). Both are entering queue2 and no other thread can wake them up.

In user_test_3, main thread opens dd driver firstly and waits for its sub thread finished and then close its fd. Since sub thread cannot obtain sem2 so it keeps waiting, hence a deadlock generates.

In user_test_4, main thread opens dd driver and enter IOCMODE2 successfully then waits for its sub thread. Sub thread tries to enter IOCMODE1 but enters queue2 and main thread cannot wake it up, so a deadlock generates.

Race conditions:

In e2_open(), count1 and count2 are accessed with sem1, without this each threads can modify it without consistency. Sem2 is used to ensure only 1 thread can operate dd file at a time on mode1, so it also need sem1 to ensure not every thread can preempt it at the same time. However this mechanism may be invalid if the only thread holding sem1 waits for other threads finishing at least e2_open(), and a potential deadlock generates.

In e2_release(), like I mentioned above, count1 count2, sem2, queue1 and queue2 are accessed and also need sem1 to protect them.

In e2_read() and e2_write(), ramdisk is accessed and modified which need sem1 to protect itself.

In e2_ioctl(), count1 & 2 and sem2 are modified which need sem1 to protect them. However, since modifying count1 & 2 opration is behind while loop which let thread go into queue1 or 2, it will cause the condition, e.g. if count1 & 2 > 1 is always negative when 2 or more threads exist and a potential deadlock generates.
