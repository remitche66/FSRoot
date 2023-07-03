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


      // Create a new function using a FSFitFunction object.
      //  Initialize the parameters with values.

    static void createFunction(FSFitFunction* userFunction, double value1, double value2 = 0,
                                     double value3 = 0, double value4 = 0, double value5 = 0, 
                                     double value6 = 0, double value7 = 0, double value8 = 0){
      FSFitFunctionList::addFunction(userFunction);
      FSFitFunctionList::getFunction(userFunction->fName())->setParameters(value1,value2,value3,
                                                             value4,value5,value6,value7,value8);
    }


      // Create a new composite function from preexisting functions combined using 
      //   a formula.  Recognizes "a+b", "a-b", "a*b", "a/b" for 
      //   functions named "a" and "b", where "a" and "b" have already
      //   been defined in a previous call to createFunction. 

    static void createFunction(TString fName, TString formula){ 
      FSFitFunctionList::addFunction(fName, formula);
    }


      // Return a TF1* associated with a given function (for drawing).

    static TF1* getTF1(TString fName, double xLow = -99.0, double xHigh = -999.0){
      return FSFitFunctionList::getFunction(fName)->getTF1(xLow,xHigh);
    }


      // Return a TH1F* associated with a given function.
      //   This is more stable than the above getTF1 -- once created it is
      //    independent of the function and its parameters.

    static TH1F* getTH1F(TString fName, TString bounds){
      return FSFitFunctionList::getFunction(fName)->getTH1F(bounds);
    }


      // Evaluate a function at a point (sometimes useful for setting 
      //  normalization parameter values)

    static double evaluateFunction(TString fName, double x){
      return FSFitFunctionList::getFunction(fName)->fx(x);
    }

      // Print a list of functions to the screen.

    static void showFunctions(bool showDetails = false, bool showClones = false){
      FSFitFunctionList::showFunctions(showDetails,showClones);
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
                      double value3 = 0, double value4 = 0, double value5 = 0, 
                      double value6 = 0, double value7 = 0, double value8 = 0){
      FSFitFunctionList::getFunction(fName)->setParameters(value1,value2,value3,
                                            value4,value5,value6,value7,value8);
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

    static void releaseParameters(TString fName){
      vector<TString> fpNames = FSFitFunctionList::getFunction(fName)->fpNames();
      for (unsigned int i = 0; i < fpNames.size(); i++){
        FSFitParameterList::getParameter(fpNames[i])->setFixed(false);
      }
    }


      // Set step sizes. The defaults are either 0.1 x abs(initial value) 
      //   or 0.1 if the initial value is zero.

    static void setParameterSteps(TString fName, double value1, double value2 = 0, 
                          double value3 = 0, double value4 = 0, double value5 = 0,
                          double value6 = 0, double value7 = 0, double value8 = 0){
      FSFitFunctionList::getFunction(fName)->setParSteps(value1,value2,value3,
                                           value4,value5,value6,value7,value8);
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


      // Get parameter values and errors.
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

      // Note: this gives sigma_ij = rho_ij * sigma_i * sigma_j
    static double getCorrelatedError(TString mName, TString fpName1, TString fpName2){
      return FSFitMinuitList::getMinuit(mName)->correlatedError(fpName1,fpName2);
    }


      // Print a list of parameters to the screen.

    static void showParameters(TString fName){
      FSFitFunctionList::getFunction(fName)->showParameters();
    }


    // *************************
    //  METHODS FOR DATA (to be fitted)
    // *************************

      // Create a data set from a histogram or a vector<FSXYPoint*>

    static void createDataSet(TString dName = "dDefault", TH1F* hist = NULL){
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

    static void clearFitRange(TString dName = "dDefault"){
      FSFitDataSetList::getDataSet(dName)->clearLimits();
    }

    static void setFitRange(TString dName, double lowLimit, double highLimit){
      FSFitDataSetList::getDataSet(dName)->setLimits(lowLimit,highLimit);
    }
    static void setFitRange(double lowLimit, double highLimit){
      FSFitDataSetList::getDataSet("dDefault")->setLimits(lowLimit,highLimit);
    }

    static void addFitRange(TString dName, double lowLimit, double highLimit){
      FSFitDataSetList::getDataSet(dName)->addLimits(lowLimit,highLimit);
    }
    static void addFitRange(double lowLimit, double highLimit){
      FSFitDataSetList::getDataSet("dDefault")->addLimits(lowLimit,highLimit);
    }

      // Print a list of data sets to the screen.

    static void showDataSets(){
      FSFitDataSetList::showDataSets();
    }

    // *************************
    //  METHODS FOR FITTING
    // *************************

      // Create a minuit object (optionally add a data set to fit and a function to fit it with).
      //  Options for fcnName are "CHI2", "LIKELIHOOD", or "UNBINNED".

    static void createMinuit(TString mName = "mDefault", TString dName = "", TString fName = "", 
                               TString fcnName = "CHI2"){
      FSFitMinuitList::addMinuit(mName,fcnName);
      if (dName != "" && fName != "") FSFitMinuitList::addFitComponent(mName,dName,fName);
    }

      // Add a data set to fit and a function to fit it with.  Many of these can be added for
      //  simultaneous fits (with constraints between parameters).

    static void addFitComponent(TString mName, TString dName, TString fName){
      FSFitMinuitList::addFitComponent(mName,dName,fName);
    }
    static void addFitComponent(TString dName, TString fName){
      FSFitMinuitList::addFitComponent("mDefault",dName,fName);
    }

      // Do a migrad fit.

    static void migrad(TString mName = "mDefault", int strategy = 1){
      FSFitMinuitList::getMinuit(mName)->migrad(strategy);
    }
    static void migrad(TH1F* hist, TString fName, TString fcnName = "CHI2", int strategy = 1){
      FSFitUtilities::createDataSet("dDefault",hist);
      FSFitUtilities::createMinuit("mDefault","dDefault",fName,fcnName);
      FSFitUtilities::migrad("mDefault",strategy);
    }

      // Do a minos fit.

    static void minos(TString mName = "mDefault", int strategy = 1){
      FSFitMinuitList::getMinuit(mName)->minos(strategy);
    }
    static void minos(TH1F* hist, TString fName, TString fcnName = "CHI2", int strategy = 1){
      FSFitUtilities::createDataSet("dDefault",hist);
      FSFitUtilities::createMinuit("mDefault","dDefault",fName,fcnName);
      FSFitUtilities::minos("mDefault",strategy);
    }

      // Get the value of the FCN (only works after a fit).

    static double fcnValue(TString mName = "mDefault"){
      return FSFitMinuitList::getMinuit(mName)->fcnValue();
    }

      // Get the likelihood.

    static double likelihood(TString mName = "mDefault"){
      return FSFitMinuitList::getMinuit(mName)->likelihood();
    }

      // Get the chisquare.

    static double chisquare(TString mName = "mDefault"){
      return FSFitMinuitList::getMinuit(mName)->chisquare();
    }

      // Get the number of points

    static int nDataPoints(TString mName = "mDefault"){
      return FSFitMinuitList::getMinuit(mName)->nDataPoints();
    }

      // Get the number of free parameters.

    static int nFreeParameters(TString mName = "mDefault"){
      return FSFitMinuitList::getMinuit(mName)->nFreeParameters();
    }

      // Get the number of degrees of freedom.

    static int nDOF(TString mName = "mDefault"){
      return FSFitMinuitList::getMinuit(mName)->nDataPoints()
              - FSFitMinuitList::getMinuit(mName)->nFreeParameters();
    }

      // Get the status of the covariance matrix.

    static int fitStatus(TString mName = "mDefault"){
      return FSFitMinuitList::getMinuit(mName)->fitStatus();
    }

      // Get a likelihood scan.

    static TH1F* getLikelihoodScan(TString mName, TString fpName, double xLow, double xHigh, unsigned int nSteps, double xScale=1.0){
      // need to make sure the parameter is released before we try to scan so we get the best likelihood to start
      FSFitUtilities::releaseParameter(fpName);
      return FSFitMinuitList::getMinuit(mName)->scanLikelihood(fpName, xLow, xHigh, nSteps, xScale);
    }

      // Print a list of minuit objects to the screen.

    static void showMinuitList(bool showDetails = false){
      FSFitMinuitList::showMinuitList(showDetails);
    }

    // *************************
    //  RESET EVERYTHING
    // *************************

    static void clear(bool clearClones = false){
      FSFitParameterList::clearParameters(clearClones);
      FSFitFunctionList::clearFunctions(clearClones);
      FSFitDataSetList::clearDataSets();
      FSFitMinuitList::clearMinuit();
    }


};



#endif
