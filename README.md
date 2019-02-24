# FSRoot

FSRoot is a set of utilities to help manipulate information about different Final States~(FS) produced in particle physics experiments.  The utilities are built around the CERN ROOT framework.


*********************
Notes on Installation
*********************

(1) Set the location of FSRoot in your login shell script (e.g. .cshrc):

    setenv FSROOT [xxxxx]/FSRoot

(2) Also probably add the FSRoot directory to $DYLD_LIBRARY_PATH and $LD_LIBRARY_PATH.  For example:

     setenv DYLD_LIBRARY_PATH $DYLD_LIBRARY_PATH\:$FSROOT
     setenv   LD_LIBRARY_PATH   $LD_LIBRARY_PATH\:$FSROOT

(3) There is usually a .rootrc file in your home directory that ROOT uses for initialization.  Add lines like these to .rootrc, which tell ROOT the location of FSRoot:

    Unix.*.Root.DynamicPath: .:$(FSROOT):$(ROOTSYS)/lib:
    Unix.*.Root.MacroPath:   .:$(FSROOT):

(4) Now when you open ROOT, the FSRoot utilities should be loaded and compiled -- you should see a message saying "Loading the FSRoot Macros" along with the output of the compilation.
