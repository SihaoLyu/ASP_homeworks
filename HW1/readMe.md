# HW1 Instruction

Name: Sihao Lyu;
UFID: 1888-8483;

To compile homework files, I added a makefile, which has 5 targets, for compiling all files and each of them and cleaning them.
Use test file path as the second input argument for mapper and combiner; Reducer just reads keyboard input tuples.

By running run.sh, there will be expected to show 3 parts of outcome without a space, but at the end of 2rd (reducer) and 3th
(combiner) parts there will be a line speratly that is  "END OF STDIN" to show the end of part.