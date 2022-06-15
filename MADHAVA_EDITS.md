Notes:

1) doing plain Make only compiles test-simple.c, not test-correct.c or test.c
2) for test-correct.c, add flag: TEST=correct and for test-old.c, add flag: TEST=old
3) Only the first thread inserts, the others only delete or update values
4) intset.c defines operations, which use the actual implementation. It feels like it's an adapter for maximum compatibility.
5) No option to specify random seed in ASCYLIB, however it was there in synchrobench. Digging deeper, synchrobench uses a single random number generator, but ASCYLIB uses a different type of random generation and occurs in stages. 3 seeds are required instead of 1 in this case, while only one was required in synchrobench. It is possible to generate the same sequence in both if the random generator in synchrobench (purely due to convenient modifiability) is modified to that in ASCYLIB.