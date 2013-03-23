################################################################################
# IPK 1
################################################################################

################################################################################
# promenne prekladu


NAME = webclient server
CPP  = g++
CC   = g++
SCFLAGS = -g -pedantic -Wall -W -O2 -DINLINE
INCS =
CXXINCS =
CXXFLAGS = -Wall $(SCFLAGS)
LOGIN = Tuxilero
TARBALL = Makefile main.cpp parser.cpp parser.h socket.cpp socket.h
#CFLAGS = $(INCS)  -fmessage-length=0

.PHONY: gzip
.PHONY: setLd
.PHONY: clear
.PHONY: clean
.PHONY: clnobj

################################################################################
# obecny cil kompilace
all: $(NAME)

################################################################################
# linkovani vysledneho souboru
webclient: main.o parser.o socket.o
	$(CC) -o $@ main.o parser.o socket.o
	
server: server.o
	$(CC) -o $@ server.o

################################################################################
# kompilace hlavniho souboru
main.o: main.cpp
	$(CC) -c main.cpp -o main.o $(CXXFLAGS)
	
parser.o: parser.cpp
	$(CC) -c parser.cpp -o parser.o $(CXXFLAGS)
	
socket.o: socket.cpp
	$(CC) -c socket.cpp -o socket.o $(CXXFLAGS)
	
server.o: server.cpp
	$(CC) -c server.cpp -o server.o $(CXXFLAGS)

################################################################################
# kompilace modulu

# dynamicka knihovna

################################################################################
# cil vycisteni souboru prekladu
clean: clear
clear:
	rm -f *.o $(NAME) $(LIBS) $(LOGIN).tar

clnobj:
	rm -f *.o

setLD:
	export LD_LIBRARY_PATH="."

$(LOGIN).tar: $(TARBALL)
	tar -cf $(LOGIN).tar $(TARBALL)
	
gzip: $(LOGIN).tar
#	gzip $(LOGIN).tar

################################################################################
################################################################################
