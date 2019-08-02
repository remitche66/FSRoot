#! gnumake

ROOTFLAGS     = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) -lMinuit -lMathCore -lTreePlayer

FSBASIC_LIBS = $(FSROOT)/FSBasic/FSControl.o $(FSROOT)/FSBasic/FSCanvas.o $(FSROOT)/FSBasic/FSTree.o $(FSROOT)/FSBasic/FSHistogram.o $(FSROOT)/FSBasic/FSCut.o $(FSROOT)/FSBasic/FSString.o $(FSROOT)/FSBasic/FSPhysics.o
FSMODE_LIBS = $(FSROOT)/FSMode/FSModeInfo.o $(FSROOT)/FSMode/FSModeTree.o $(FSROOT)/FSMode/FSModeHistogram.o $(FSROOT)/FSMode/FSModeCollection.o $(FSROOT)/FSMode/FSModeString.o

MYINCLUDES = -I. -I$(FSROOT)/

all: $(FSBASIC_LIBS) $(FSMODE_LIBS) 

%: %.cc $(FSBASIC_LIBS) $(FSMODE_LIBS) 
	g++  $(ROOTFLAGS) $(MYINCLUDES) -c -o $*.o $*.cc
	g++  $(ROOTFLAGS) $(MYINCLUDES) $(ROOTLIBS) $(FSBASIC_LIBS) $(FSMODE_LIBS) $*.o -o $*


clean:
	@rm -f *.o $* */*.o
