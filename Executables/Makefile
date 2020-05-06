ROOT_CFLAGS   := $(shell root-config --cflags)
ROOT_LIBFLAGS := $(shell root-config --libs) -lMinuit -lMathCore -lMathMore -lTreePlayer

FSROOT_LIBFILE  := $(FSROOT)/lib/libFSRoot.a
FSROOT_LIBFLAGS := -L$(FSROOT)/lib -lFSRoot
FSROOT_INCFLAGS := -I$(FSROOT)

EXE_SRCFILES := $(wildcard *.cc)
EXE_EXEFILES := $(foreach file,$(EXE_SRCFILES),$(file:.cc=))

EXTRA_SRCFILES := $(wildcard *.C)
EXTRA_INCFILES := $(wildcard *.h)

all: $(EXE_EXEFILES)

%: %.cc $(EXTRA_SRCFILES) $(EXTRA_INCFILES) $(FSROOT_LIBFILE)
	g++ $(ROOT_CFLAGS) $(ROOT_LIBFLAGS) $(FSROOT_LIBFLAGS) $(FSROOT_INCFLAGS) -I. -o $@ $@.cc $(EXTRA_SRCFILES)

clean:
	rm -rf $(EXE_EXEFILES)

