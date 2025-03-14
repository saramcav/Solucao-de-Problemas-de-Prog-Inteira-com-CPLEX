################################

SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic
CPLEXDIR   = /opt/ibm/ILOG/CPLEX_Studio2211/cplex
CONCERTDIR = /opt/ibm/ILOG/CPLEX_Studio2211/concert

################

# Compilador 
CCC = g++ -O0

# Opcoes de compilacao
CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD -Wno-ignored-attributes

# Bibliotecas e includes
CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(SYSTEM)
CPLEXLIB      = cplex$(dynamic:yes=2010)

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR) $(dynamic:yes=-L$(CPLEXBINDIR))
CCLNFLAGS = -lconcert -lilocplex -l$(CPLEXLIB) -lm -lpthread -ldl

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 

all : coloracao

coloracao.o: coloracao.cpp coloracao.h 
	$(CCC) -c $(CCFLAGS) coloracao.cpp -o coloracao.o
io.o: io.cpp coloracao.h
	$(CCC) -c $(CCFLAGS) io.cpp -o io.o
coloracao: coloracao.o io.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o coloracao coloracao.o io.o $(CCLNFLAGS)
clean:
	rm --force coloracao coloracao.o io.o