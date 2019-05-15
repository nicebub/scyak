CC=/usr/local/bin/gcc-8

SRCDIR = src
BINDIR = bin
SFILES = $(SRCDIR)/init.c \
$(SRCDIR)/spcread.c \
$(SRCDIR)/sclex.yy.c \
$(SRCDIR)/scyak.c \
$(SRCDIR)/parser.c \
$(SRCDIR)/tables.c \
$(SRCDIR)/lr0.c \
$(SRCDIR)/lr0b.c \
$(SRCDIR)/functions.c \


YFILES = $(SRCDIR)/init.c \
$(SRCDIR)/spcread.c \
$(SRCDIR)/lex.yy.c \
$(SRCDIR)/scyak.c \
$(SRCDIR)/parser.c \
$(SRCDIR)/tables.c \
$(SRCDIR)/lr0.c \
$(SRCDIR)/lr0b.c \
$(SRCDIR)/functions.c \

all: scyak yyyak
	
scyak: $(SFILES)
	$(CC) -g -o $(BINDIR)/$@ $(SFILES) -I include/ -L lib/ -lbuffer

yyyak: $(YFILES)
	$(CC) -g -o $(BINDIR)/$@ $(YFILES) -DFLEX -I include/ -L lib/ -lbuffer -ll

clean:
	rm -rf $(BINDIR)/{scyak,yyyak} $(BINDIR)/*.dSYM

test:
	bin/scyak ex.r
	
.PHONY: all clean test