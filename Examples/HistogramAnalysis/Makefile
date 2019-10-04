#! gnumake

ROOTFLAGS     = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) -lMinuit -lMathCore -lMathMore -lTreePlayer

FSBASIC_LIBS = $(FSROOT)/FSBasic/FSControl.o $(FSROOT)/FSBasic/FSCanvas.o $(FSROOT)/FSBasic/FSTree.o $(FSROOT)/FSBasic/FSHistogram.o $(FSROOT)/FSBasic/FSCut.o $(FSROOT)/FSBasic/FSString.o $(FSROOT)/FSBasic/FSPhysics.o
FSMODE_LIBS = $(FSROOT)/FSMode/FSModeInfo.o $(FSROOT)/FSMode/FSModeTree.o $(FSROOT)/FSMode/FSModeHistogram.o $(FSROOT)/FSMode/FSModeCollection.o $(FSROOT)/FSMode/FSModeString.o
FSDATA_LIBS = $(FSROOT)/FSData/FSEEDataSet.o $(FSROOT)/FSData/FSEEDataSetList.o $(FSROOT)/FSData/FSEEXS.o $(FSROOT)/FSData/FSEEXSList.o $(FSROOT)/FSData/FSXYPoint.o $(FSROOT)/FSData/FSXYPointList.o
FSFIT_LIBS = $(FSROOT)/FSFit/FSFitFunctions.o $(FSROOT)/FSFit/FSFitPrivate.o $(FSROOT)/FSFit/FSFitUtilities.o
FSANALYSIS_LIBS = $(FSROOT)/FSAnalysis/FSHistogramAnalysis.o

MYINCLUDES = -I. -I$(FSROOT)/

all: $(FSBASIC_LIBS) $(FSMODE_LIBS) $(FSDATA_LIBS) $(FSFIT_LIBS) $(FSANALYSIS_LIBS)

%: %.cc $(FSBASIC_LIBS) $(FSMODE_LIBS) $(FSDATA_LIBS) $(FSFIT_LIBS) $(FSANALYSIS_LIBS)
	g++  $(ROOTFLAGS) $(MYINCLUDES) -c -o $*.o $*.cc
	g++  $(ROOTFLAGS) $(MYINCLUDES) $(ROOTLIBS) $(FSBASIC_LIBS) $(FSMODE_LIBS) $(FSDATA_LIBS) $(FSFIT_LIBS) $(FSANALYSIS_LIBS) $*.o -o $*


clean:
	@rm -f *.o $* */*.o
