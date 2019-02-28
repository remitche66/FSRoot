# FSRoot

FSRoot is a set of utilities to help manipulate information about different Final States (FS) produced in particle physics experiments.  The utilities are built around the CERN ROOT framework.  

See Documentation/FSRoot.pdf and the examples in the Examples directory for help.


*********************
Notes on Installation
*********************

(1) Get the source, as above:

    git clone https://github.com/remitche66/FSRoot.git FSRoot

(2) Set the location of FSRoot in your login shell script (e.g. .cshrc):

    setenv FSROOT [xxxxx]/FSRoot

(3) Also probably add the FSRoot directory to $DYLD_LIBRARY_PATH and $LD_LIBRARY_PATH.  This allows you to compile code including FSRoot functions.  For example:

     setenv DYLD_LIBRARY_PATH $DYLD_LIBRARY_PATH\:$FSROOT
     setenv   LD_LIBRARY_PATH   $LD_LIBRARY_PATH\:$FSROOT

(4) There is usually a .rootrc file in your home directory that ROOT uses for initialization.  Add lines like these to .rootrc, which tell ROOT the location of FSRoot:

    Unix.*.Root.DynamicPath: .:$(FSROOT):$(ROOTSYS)/lib:
    Unix.*.Root.MacroPath:   .:$(FSROOT):

(5) To load and unload FSRoot automatically when you open and close ROOT, you can also add lines like these to the .rootrc:

    Rint.Logon:    $(FSROOT)/rootlogon.FSRoot.C
    Rint.Logoff:   $(FSROOT)/rootlogoff.FSRoot.C

Alternatively, you could have those lines point to other login and logoff files and then load and unload FSRoot from those files with lines like these [this is the method to use in the case that you want to load several different sets of macros]:

    gROOT->ProcessLine(".x $FSROOT/rootlogon.FSRoot.C");
    gROOT->ProcessLine(".x $FSROOT/rootlogoff.FSRoot.C");

As a third alternative, you could load FSRoot manually from ROOT:

    .x $FSROOT/rootlogon.FSRoot.C


(6) When FSRoot is loaded and compiled, you should see a message saying "Loading the FSRoot Macros" along with the output of the compilation.  



