#! gnumake

ROOTFLAGS     = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs) -lMinuit -lMathCore -lMathMore -lTreePlayer -lgsl -lgslcblas

FSBASIC_LIBS = FSBasic/FSControl.o FSBasic/FSCanvas.o FSBasic/FSString.o FSBasic/FSSystem.o FSBasic/FSPhysics.o FSBasic/FSCut.o FSBasic/FSTree.o FSBasic/FSHistogram.o
FSMODE_LIBS = FSMode/FSModeInfo.o FSMode/FSModeTree.o FSMode/FSModeHistogram.o FSMode/FSModeCollection.o FSMode/FSModeString.o
FSDATA_LIBS = FSData/FSEEDataSet.o FSData/FSEEDataSetList.o FSData/FSEEXS.o FSData/FSEEXSList.o FSData/FSXYPoint.o FSData/FSXYPointList.o
FSFIT_LIBS = FSFit/FSFitFunctions.o FSFit/FSFitPrivate.o FSFit/FSFitUtilities.o
FSANALYSIS_LIBS = FSAnalysis/FSHistogramAnalysis.o

MYINCLUDES = -I. -I$(FSROOT)/

all: $(FSBASIC_LIBS) $(FSMODE_LIBS) $(FSDATA_LIBS) $(FSFIT_LIBS) $(FSANALYSIS_LIBS)

%: %.cc $(FSBASIC_LIBS) $(FSMODE_LIBS) $(FSDATA_LIBS) $(FSFIT_LIBS) $(FSANALYSIS_LIBS)
	g++  $(ROOTFLAGS) $(MYINCLUDES) -c -o $*.o $*.cc
	g++  $(ROOTFLAGS) $(MYINCLUDES) $(ROOTLIBS) $(FSBASIC_LIBS) $(FSMODE_LIBS) $(FSDATA_LIBS) $(FSFIT_LIBS) $(FSANALYSIS_LIBS) $*.o -o $*

FSBasic/%.o: FSBasic/%.h FSBasic/%.C
	g++ $(ROOTFLAGS) $(MYINCLUDES) -c -o FSBasic/$*.o FSBasic/$*.C

FSMode/%.o: FSMode/%.h FSMode/%.C
	g++ $(ROOTFLAGS) $(MYINCLUDES) -c -o FSMode/$*.o FSMode/$*.C

FSData/%.o: FSData/%.h FSData/%.C
	g++ $(ROOTFLAGS) $(MYINCLUDES) -c -o FSData/$*.o FSData/$*.C

FSFit/%.o: FSFit/%.h FSFit/%.C
	g++ $(ROOTFLAGS) $(MYINCLUDES) -c -o FSFit/$*.o FSFit/$*.C

FSAnalysis/%.o: FSAnalysis/%.h FSAnalysis/%.C
	g++ $(ROOTFLAGS) $(MYINCLUDES) -c -o FSAnalysis/$*.o FSAnalysis/$*.C


clean:
	@rm -f *.o $* */*.o
