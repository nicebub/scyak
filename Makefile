CC=/usr/local/bin/gcc-8

SRCDIR = src
BINDIR = bin
INCLDIR = include
#LIBDIR = lib
LIBDIR= ''
#DEBUG = "-g"
SFILES = $(SRCDIR)/init.c \
$(SRCDIR)/spcread.c \
$(SRCDIR)/scyak.c \
$(SRCDIR)/parser.c \
$(SRCDIR)/tables.c \
$(SRCDIR)/lr0.c \
$(SRCDIR)/lr0b.c \
$(SRCDIR)/functions.c \
$(SRCDIR)/run_parser.c 


YFILES = $(SRCDIR)/init.c \
$(SRCDIR)/spcread.c \
$(SRCDIR)/lex.yy.c \
$(SRCDIR)/scyak.c \
$(SRCDIR)/parser.c \
$(SRCDIR)/tables.c \
$(SRCDIR)/lr0.c \
$(SRCDIR)/lr0b.c \
$(SRCDIR)/functions.c \
$(SRCDIR)/run_parser.c 

PFILES = $(SRCDIR)/scy.tab.c \
$(SRCDIR)/buffer.c \
$(SRCDIR)/test_parser.c

all: scyak

copy:
	./$(BINDIR)/scyak -d scyak.y;mv scy.tab.h $(INCLDIR)/ && mv scy.tab.c $(SRCDIR)/	
scyak: $(SFILES)
	$(CC) $(DEBUG) -o $(BINDIR)/$@ $(SFILES) -I $(INCLDIR)/ -L $(LIBDIR)/

parser: copy ${PFILES}
	$(CC) $(DEBUG) -o $(BINDIR)/$@ $(PFILES) -I $(INCLDIR)/ -L $(LIBDIR)/

#yyyak: $(YFILES)
#	$(CC) $(DEBUG) -o $(BINDIR)/$@ $(YFILES) -DFLEX -I include/ -L lib/ -ll

clean:
	rm -rf $(BINDIR)/{scyak,yyyak,parser} $(BINDIR)/*.dSYM $(INCLDIR)/scy.tab.h $(SRCDIR)/scy.tab.c

test: scyak parser
	bin/scyak ex.r
	
.PHONY: all clean test
