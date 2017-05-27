vpath %.h header
vpath %.c src
vpath %.o obj
vpath main bin

CC = gcc
CFLAGS = -g -Wall -Wextra
IFLAGS = -Iinclude

ALL = all
PEER = peer
OBJFILES = obj/connecthandling.o obj/segment.o obj/filehandling.o obj/peer.o obj/parser.o 
OPATH = obj
CPATH = src
BPATH = bin

$(ALL) : $(PEER)

$(PEER) : $(OBJFILES)
	$(CC) -o $@ $^ $(LDLIBS)
	mv $@ $(BPATH)

$(OPATH)/%.o : $(CPATH)/%.c
	$(CC) $(CFLAGS) -c $< $(IFLAGS) -o $@
	

clean :
	rm -r $(OPATH)/* $(BPATH)/* 
archive :
	tar czvf Projet_JRO_LUTTRINGER_RP.tar.gz .
