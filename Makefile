CFLAGS?=	-Wall -ggdb -W -O
CC?=		gcc

object=	xref.o mem.o atom.o except.o table.o set.o arith.o


all:   xref tags

tags:  *.c *.h
	-ctags -R




xref: $(object)  Makefile
	$(CC) $(CFLAGS) $(LDFLAGS) -o xref $(object) 

clean:
	-rm -f *.o xref *~ core *.core tags

xref.o: xref.c mem.h table.h atom.h set.h

except.o: except.h
mem.o:	except.h mem.h
atom.o: mem.h atom.h 
table.o: table.h mem.h
set.o:	set.h mem.h arith.h
arith.o: arith.h

.PHONY: clean all tar
