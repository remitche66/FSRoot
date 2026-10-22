ROOT_CFLAGS   := $(shell root-config --cflags)
ROOT_LIBFLAGS := $(shell root-config --libs) -lMinuit -lMathCore -lMathMore -lTreePlayer

FSROOT_SRCDIRS := FSBasic FSMode FSData FSFit FSAnalysis
FSROOT_LIBDIR := lib
FSROOT_LIBFILE := $(FSROOT_LIBDIR)/libFSRoot.a
FSROOT_SOFILE  := $(FSROOT_LIBDIR)/libFSRoot.so
FSROOT_SRCFILES := $(foreach dir,$(FSROOT_SRCDIRS),$(wildcard $(dir)/*.C))
FSROOT_INCFLAGS := -I$(FSROOT)
FSROOT_OBJFILES := $(FSROOT_SRCFILES:%.C=%.o)

all:  $(FSROOT_LIBFILE) $(FSROOT_SOFILE)

$(FSROOT_LIBFILE): $(FSROOT_OBJFILES)
	ar -rsv $@ $(FSROOT_OBJFILES) && ranlib $(FSROOT_LIBFILE)

$(FSROOT_SOFILE): $(FSROOT_OBJFILES)
	g++ -shared -o $@ $(FSROOT_OBJFILES) $(ROOT_LIBFLAGS)

$(FSROOT_OBJFILES): | $(FSROOT_LIBDIR)

%.o: %.C %.h
	g++ $(ROOT_CFLAGS) -fPIC $(FSROOT_INCFLAGS) -c -o $@ $*.C

$(FSROOT_LIBDIR):
	mkdir -p $(FSROOT_LIBDIR)

clean:
	rm -rf $(FSROOT_LIBDIR) $(FSROOT_OBJFILES) */*.so */*.pcm */*.d
