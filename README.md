# FSRoot

******
.cshrc
******

Give the location of FSRoot in the login shell script:

    setenv FSROOT [xxxxx]/FSRoot

Also probably add the FSRoot directory to DYLD_LIBRARY_PATH and LD_LIBRARY_PATH.  For example:

     setenv DYLD_LIBRARY_PATH $ROOTSYS/lib\:$CLHEP_LIB_DIR\:$FSROOT
     setenv   LD_LIBRARY_PATH $ROOTSYS/lib\:$CLHEP_LIB_DIR\:$FSROOT


*******
.rootrc
*******

There is usually a .rootrc file in your home directory.

It should contain lines like this, which tell root the
location of FSRoot:

    Unix.*.Root.DynamicPath: .:$(FSROOT):$(ROOTSYS)/lib:
    Unix.*.Root.MacroPath:   .:$(FSROOT):



