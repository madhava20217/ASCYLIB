Notes:

1) doing plain Make only compiles test-simple.c, not test-correct.c or test.c
2) for test-correct.c, add flag: TEST=correct and for test-old.c, add flag: TEST=old
3) Only the first thread inserts, the others only delete or update values
4) 