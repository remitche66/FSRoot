#! gnumake

ROOTFLAGS     = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) -lMinuit -lMathCore -lMathMore -lTreePlayer -lgsl -lgslcblas

FSBASIC_LIBS = FSBasic/FSControl.o FSBasic/FSCanvas.o FSBasic/FSTree.o FSBasic/FSHistogram.o FSBasic/FSCut.o FSBasic/FSString.o FSBasic/FSPhysics.o
FSMODE_LIBS = FSMode/FSModeInfo.o FSMode/FSModeTree.o FSMode/FSModeHistogram.o FSMode/FSModeCollection.o FSMode/FSModeString.o

MYINCLUDES = -I.

all: $(FSBASIC_LIBS) $(FSMODE_LIBS) 

%: %.cc $(FSBASIC_LIBS) $(FSMODE_LIBS) 
	g++  $(ROOTFLAGS) $(MYINCLUDES) -c -o $*.o $*.cc
	g++  $(ROOTFLAGS) $(MYINCLUDES) $(ROOTLIBS) $(FSBASIC_LIBS) $(FSMODE_LIBS) $*.o -o $*

FSBasic/%.o: FSBasic/%.h FSBasic/%.C
	g++ $(ROOTFLAGS) $(MYINCLUDES) -c -o FSBasic/$*.o FSBasic/$*.C

FSMode/%.o: FSMode/%.h FSMode/%.C
	g++ $(ROOTFLAGS) $(MYINCLUDES) -c -o FSMode/$*.o FSMode/$*.C


clean:
	@rm -f *.o $* */*.o
