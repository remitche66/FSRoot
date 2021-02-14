
INC_DIR :=  -I.. -I$(shell root-config --incdir) \
  -I$(AMPTOOLS) -I$(FSAMPTOOLS)
#  -I$(AMPTOOLS) -I$(AMPPLOTTER) -I$(FSAMPTOOLS)

LIB_DIR := -L$(shell root-config --libdir) \
	-L$(AMPTOOLS)/lib -L$(FSAMPTOOLS)/lib
#	-L$(AMPTOOLS)/lib -L$(AMPPLOTTER)/lib -L$(FSAMPTOOLS)/lib

ROOTLIBS := $(shell root-config --glibs)

CXX := gcc
#CXX_FLAGS := -O3 -m32
CXX_FLAGS := -O3 $(shell root-config --cflags)

# for more detailed info, use VERBOSE=1
ifdef VERBOSE
	Q :=
	vecho = @true
else
	Q := @
	vecho = @echo
endif

ifdef MPI

CXX := mpicxx -cc=$(CXX)
CXX_FLAGS += -DUSEMPI

endif


# To build code instrumented for Vampir Trace  VTRACE=1

ifdef VTRACE

CXX := vtcxx -vt:inst manual
MPICC := vtcxx -vt:mpi -vt:inst manual

CXX_FLAGS += -DVTRACE

endif

ifdef GPU

INC_DIR += -I$(CUDA_INSTALL_PATH)/include
LIB_DIR += -L$(CUDA_INSTALL_PATH)/lib64

LIBS :=  -lFSAmpTools_GPU -lAmpTools_GPU $(ROOTLIBS) -lcudart -lstdc++
#LIBS :=  -lFSAmpTools_GPU -lAmpPlotter -lAmpTools_GPU $(ROOTLIBS) -lcudart -lstdc++
CXX_FLAGS += -DGPU_ACCELERATION

else

#LIBS :=  -lFSAmpTools -lAmpPlotter -lAmpTools $(ROOTLIBS) -lstdc++
LIBS :=  -lFSAmpTools -lAmpTools $(ROOTLIBS) -lstdc++

endif

SRCFILES := $(wildcard *.cc)
TARGET_EXE := $(subst .cc,_exe,$(SRCFILES))

.PHONY: default
.PRECIOUS: %.o

default: $(TARGET_EXE)

# generic build rule (requires all libraries)

%_exe: %.o
	$(vecho) "--> Linking $(subst _exe,,$@)"
	$(Q)$(CXX)  $(CXX_FLAGS) \
	-Wl,-rpath,$(ROOTSYS)/lib -Wl,-rpath,$(AMPTOOLS)/lib -Wl,-rpath,$(FSAMPTOOLS)/lib \
	-o $(subst _exe,,$@) $< -O3 $(INC_DIR) $(LIB_DIR) $(LIBS)

%.o : %.cc
	$(vecho) "-> Compiling $<"
	$(Q)$(CXX) $(CXX_FLAGS) -M -o $*.d $< $(INC_DIR) ; \
	cp $*.d $*.dep; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.dep; \
	rm -f $*.d $*.d.bak
	$(Q)$(CXX) $(CXX_FLAGS) -c -o $@ $< $(INC_DIR)

clean:
	$(Q)rm -f *.o *.dep $(subst _exe,,$(TARGET_EXE))

-include $(SRCFILES:.cc=.dep)
