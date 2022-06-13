ROOT = ../..

include $(ROOT)/common/Makefile.common

BINS = $(BINDIR)/sq-ll
PROF = $(ROOT)/src

.PHONY:	all clean

all:	main

measurements.o:
	$(CC) $(CFLAGS) -no-pie -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c

ssalloc.o:
	$(CC) $(CFLAGS) -no-pie -c -o $(BUILDIR)/ssalloc.o $(PROF)/ssalloc.c

linkedlist-seq.o: 
	$(CC) $(CFLAGS) -no-pie -c -o $(BUILDIR)/linkedlist-seq.o linkedlist-seq.c

seq.o: linkedlist-seq.h linkedlist-seq.o
	$(CC) $(CFLAGS) -no-pie -c -o $(BUILDIR)/seq.o seq.c

intset.o: linkedlist-seq.h seq.h
	$(CC) $(CFLAGS) -no-pie -c -o $(BUILDIR)/intset.o intset.c

test.o: linkedlist-seq.h seq.h intset.h
	$(CC) $(CFLAGS) -no-pie -c -o $(BUILDIR)/test.o $(TEST_FILE)

main: measurements.o ssalloc.o linkedlist-seq.o seq.o intset.o test.o
	$(CC) $(CFLAGS) -no-pie $(BUILDIR)/measurements.o $(BUILDIR)/ssalloc.o $(BUILDIR)/linkedlist-seq.o $(BUILDIR)/seq.o $(BUILDIR)/intset.o $(BUILDIR)/test.o -o $(BINS) $(LDFLAGS) 

clean:
	rm -f $(BINS)
