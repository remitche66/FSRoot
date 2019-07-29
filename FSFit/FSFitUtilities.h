#if !defined(FSFITUTILITIES_H)
#define FSFITUTILITIES_H

#include "FSFit/FSFitPrivate.h"

using namespace std;


// The methods inside FSFitUtilities are the only ones meant to be used.
//   They are generally an interface to methods inside FSFitPrivate,
//     which are not meant to be used.


//  ABOUT NAMING:
//  "fName" is a function name
//  "pName" is a parameter name
//  "fpName" is "fName:pName" (where fName is not a composite function)
//  "dName" is data name
//  "mName" is minuit name



class FSFitUtilities{

  public:

    // *************************
    //  METHODS FOR FUNCTIONS
    // *************************

      // Create a new function using a FSFitFunction object.
      //  Initialize the parameters using a preexisting function 
      //    with the optional fNameInitializer.

    static void createFunction(FSFitFunction* userFunction, TString fNameInitializer = ""){ 
      FSFitFunctionList::addFunction(userFunction, fNameInitializer);
    }


      // Create a new composite function from preexisting functions combined using 
      //   a formula.  Recognizes "a+b", "a-b", "a*b", "a/b" for 
      //   functions named "a" and "b", where "a" and "b" have already
      //   been defined in a previous call to createFunction. 

    static void createFunction(TString fName, TString formula){ 
      FSFitFunctionList::addFunction(fName, formula);
    }


      // Return a TF1* associated with a given function (for drawing).

    static TF1* getTF1(TString fName){
      return FSFitFunctionList::getFunction(fName)->getTF1();
    }


      // Evaluate a function at a point (sometimes useful for setting 
      //  normalization parameter values)

    static double evaluateFunction(TString fName, double x){
      return FSFitFunctionList::getFunction(fName)->fx(x);
    }

    // *************************
    //  METHODS FOR PARAMETERS
    // *************************


      // Reset parameters to their defaults.

    static void resetParameters(TString fName){ 
      FSFitFunctionList::getFunction(fName)->resetParameters();
    }

    static void resetParameter(TString fpName){ 
      FSFitParameterList::removeConstraints(fpName);
      FSFitParameterList::getParameter(fpName)->resetDefaults();
    }


      // Set parameters to new values.
      //   The first of these can also be used for function composites -- the ordering of
      //     the parameters is given by the order of the functions specified in the 
      //     formula for the composite function.
      //   In the second, "fpName" is "fName:pName", and the "fName" is not for composites
      //     (like everywhere else).

    static void setParameters(TString fName, double value1, double value2 = 0,
                                                            double value3 = 0,
                                                            double value4 = 0,
                                                            double value5 = 0,
                                                            double value6 = 0,
                                                            double value7 = 0,
                                                            double value8 = 0){
      FSFitFunctionList::getFunction(fName)->setParameters(value1,value2,value3,
                                                            value4,value5,value6,
                                                            value7,value8);
    }

    static void setParameter(TString fpName, double value){
      FSFitParameterList::getParameter(fpName)->setValue(value);
    }


      // Fix and release parameters.

    static void fixParameters(TString fName){
      vector<TString> fpNames = FSFitFunctionList::getFunction(fName)->fpNames();
      for (unsigned int i = 0; i < fpNames.size(); i++){
        FSFitParameterList::getParameter(fpNames[i])->setFixed(true);
      }
    }

    static void fixParameter(TString fpName){
      FSFitParameterList::getParameter(fpName)->setFixed(true);
    }

    static void fixParameter(TString fpName, double value){
      FSFitUtilities::setParameter(fpName,value);
      FSFitUtilities::fixParameter(fpName);
    }

    static void releaseParameter(TString fpName){
      FSFitParameterList::getParameter(fpName)->setFixed(false);
    }


      // Set step sizes. The defaults are either 0.1 x abs(initial value) 
      //   or 0.1 if the initial value is zero.

    static void setParameterSteps(TString fName, double value1, double value2 = 0,
                                                                double value3 = 0,
                                                                double value4 = 0,
                                                                double value5 = 0,
                                                                double value6 = 0,
                                                                double value7 = 0,
                                                                double value8 = 0){
      FSFitFunctionList::getFunction(fName)->setParSteps(value1,value2,value3,
                                                            value4,value5,value6,
                                                            value7,value8);
    }

    static void setParameterStep(TString fpName, double value){
      FSFitParameterList::getParameter(fpName)->setStep(value);
    }


      // Constrain parameters.  All parameter properties (value, error, fixed) are
      //  distributed to all other constrained parameters.
      //   If the value is not given, it is taken from the first parameter.

    static void constrainParameters(TString fpName1, TString fpName2, double value){
      FSFitParameterList::constrainParameters(fpName1, fpName2, value);
    }

    static void constrainParameters(TString fpName1, TString fpName2){
      FSFitParameterList::constrainParameters(fpName1, fpName2);
    }

    static void removeConstraints(TString fpName){
      FSFitParameterList::removeConstraints(fpName);
    }


      // Get parameter values.
      //  The low and high errors are only set if minos is called.

    static double getParameterValue(TString fpName){
      return FSFitParameterList::getParameter(fpName)->value();
    }

    static double getParameterError(TString fpName){
      return FSFitParameterList::getParameter(fpName)->error();
    }

    static double getParameterErrorLow(TString fpName){
      return FSFitParameterList::getParameter(fpName)->errorLow();
    }

    static double getParameterErrorHigh(TString fpName){
      return FSFitParameterList::getParameter(fpName)->errorHigh();
    }


      // Print a list of parameters to the screen.

    static void printParameters(TString fName){
      FSFitFunctionList::getFunction(fName)->printParameters();
    }


    // *************************
    //  METHODS FOR DATA (to be fitted)
    // *************************

      // Create a data set from a histogram or a vector<FSXYPoint*>

    static void createDataSet(TString dName, TH1F* hist){
      FSFitDataSetList::addDataSet(dName,hist);
    }

    static void createDataSet(TString dName, vector<FSXYPoint*> points, bool includeSystErrors = false){
      FSFitDataSetList::addDataSet(dName,points,includeSystErrors);
    }

    static void createDataSet(TString dName, vector<double> xUnbinnedData){
      FSFitDataSetList::addDataSet(dName,xUnbinnedData);
    }


      // Manage fit ranges for different data sets.
      //   Multiple fit ranges can be added to each data set.

    static void clearFitRange(TString dName){
      FSFitDataSetList::getDataSet(dName)->clearLimits();
    }

    static void setFitRange(TString dName, double lowLimit, double highLimit){
      FSFitDataSetList::getDataSet(dName)->setLimits(lowLimit,highLimit);
    }

    static void addFitRange(TString dName, double lowLimit, double highLimit){
      FSFitDataSetList::getDataSet(dName)->addLimits(lowLimit,highLimit);
    }


    // *************************
    //  METHODS FOR FITTING
    // *************************

      // Create a minuit object (optionally add a data set to fit and a function to fit it with).
      //  Options for fcnName are "CHI2", "LIKELIHOOD", or "UNBINNED".

    static void createMinuit(TString mName, TString dName = "", TString fName = "", 
                               TString fcnName = "CHI2"){
      FSFitMinuitList::addMinuit(mName,fcnName);
      if (dName != "" && fName != "") FSFitMinuitList::addFitComponent(mName,dName,fName);
    }

      // Add a data set to fit and a function to fit it with.  Many of these can be added for
      //  simultaneous fits (with constraints between parameters).

    static void addFitComponent(TString mName, TString dName, TString fName){
      FSFitMinuitList::addFitComponent(mName,dName,fName);
    }

      // Do a migrad fit.

    static void migrad(TString mName){
      FSFitMinuitList::getMinuit(mName)->migrad();
    }

      // Do a minos fit.

    static void minos(TString mName){
      FSFitMinuitList::getMinuit(mName)->minos();
    }

      // Get the value of the FCN (only works after a fit).

    static double fcnValue(TString mName){
      return FSFitMinuitList::getMinuit(mName)->fcnValue();
    }

      // Get the likelihood.

    static double likelihood(TString mName){
      return FSFitMinuitList::getMinuit(mName)->likelihood();
    }

      // Get the chisquare.

    static double chisquare(TString mName){
      return FSFitMinuitList::getMinuit(mName)->chisquare();
    }

      // Get the number of points

    static int nDataPoints(TString mName){
      return FSFitMinuitList::getMinuit(mName)->nDataPoints();
    }

      // Get the number of free parameters.

    static int nFreeParameters(TString mName){
      return FSFitMinuitList::getMinuit(mName)->nFreeParameters();
    }

      // Get the number of degrees of freedom.

    static int nDOF(TString mName){
      return FSFitMinuitList::getMinuit(mName)->nDataPoints()
              - FSFitMinuitList::getMinuit(mName)->nFreeParameters();
    }

      // Get the status of the covariance matrix.

    static int fitStatus(TString mName){
      return FSFitMinuitList::getMinuit(mName)->fitStatus();
    }


    // *************************
    //  RESET EVERYTHING
    // *************************

    static void clear(){
      FSFitParameterList::clearParameters();
      FSFitFunctionList::clearFunctions();
      FSFitDataSetList::clearDataSets();
      FSFitMinuitList::clearMinuit();
    }


};



#endif
