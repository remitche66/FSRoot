#if !defined(FSFITPRIVATE_H)
#define FSFITPRIVATE_H

#include <iostream>
#include "TF1.h"

#include <utility>
#include <vector>
#include <string>
#include <map>
#include "TChain.h"
#include "TChainElement.h"
#include "TFile.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMinuit.h"
#include "TMath.h"
#include "FSBasic/FSString.h"
#include "FSData/FSXYPoint.h"

class FSFitParameterList;
class FSFitFunctionList;
class FSFitFunction;
class FSFitFunctionComposite;
class FSFitMinuit;
class FSFitMinuitList;

using namespace std;


//  ABOUT NAMING:
//  "fName" is a function name
//  "pName" is a parameter name
//  "fpName" is "fName:pName" (where fName is not a composite function)
//  "dName" is data name
//  "mName" is minuit name


// ******************************
//  The FSFitParameter Class
// ******************************

// This class holds information about a single fit parameter.
// All instances of FSFitParameter are owned by the global FSFitParameterList.
// The FSFitParameterList is accessed using fpName ("fName:pName"), where "fName"
//   is not a composite function and therefore every parameter appears only once.

// Note:  FSFitParameter is completely self-containted, except for the
//        otherParameter method, which looks in FSFitParameterList
//        for an external (to this parameter) parameter.

// Note:  FSFitParameter gets added to the global FSFitParameterList
//        in the addParameter method of the FSFitFunction class,
//        so every FSFitParameter is associated with a FSFitFunction.

// Note:  Access to every FSFitParameter instance is through the
//        the global FSFitParameterList class.

// Note:  parNumber is only used internally to number parameters in minuit.


class FSFitParameter{

  public:

    FSFitParameter(TString n_fName, TString n_pName, double n_initialValue, TString n_description):
      m_fName(n_fName), 
      m_pName(n_pName), 
      m_initialValue(n_initialValue), 
      m_description(n_description)
    { 
      m_fpName = m_fName;  m_fpName += ":";  m_fpName += m_pName;
      resetDefaults();
    }

    void initialize(FSFitParameter* parInitializer){
      setValue(parInitializer->value());
      setError(parInitializer->error());
      setErrorLowHigh(parInitializer->errorLow(),parInitializer->errorHigh());
      setStep (parInitializer->step());
      setFixed(parInitializer->fixed());
      resetParNumber();
    }

    void resetDefaults(){
      resetConstraints();
      resetValue();
      resetError();
      resetErrorLowHigh();
      resetStep();
      resetFixed();
      resetParNumber();
    }

    FSFitParameter* otherParameter(TString n_fpName);

    ~FSFitParameter() { resetConstraints(); }

    double           value()         { return m_value; }
    double           initialValue()  { return m_initialValue; }
    double           error()         { return m_error; }
    double            errorLow()     { return m_errorLow; }
    double            errorHigh()    { return m_errorHigh; }
    double            step()         { return m_step; }
    TString          fpName()        { return m_fpName; }
    TString           fName()        { return m_fName; }
    TString           pName()        { return m_pName; }
    TString          description()   { return m_description; }
    int              parNumber()     { return m_parNumber; }
    bool             fixed()         { return m_fixed; }
    vector<TString>  constraints()   { return m_constraints; }


      // manage values (setting the value automatically sets the step size)

    void setValue(double n_value, bool distribute = true) { 
      m_value = n_value;
      m_step  = getStepBasedOnValue(m_value);
      if (distribute){
        for (unsigned int i = 0; i < m_constraints.size(); i++){
          otherParameter(m_constraints[i])->setValue(n_value,false);
        }
      }
    }
    void resetValue() { setValue(m_initialValue); }


      // manage errors (setting the error automatically sets the step size, unless it's <= 0)

    void setError(double n_error, bool distribute = true) { 
      m_error = n_error;
      if (m_error > 0) m_step  = m_error;
      if (distribute){
        for (unsigned int i = 0; i < m_constraints.size(); i++){
          otherParameter(m_constraints[i])->setError(m_error,false);
        }
      }
    }
    void resetError() { setError(-1.0); }


      // manage errors (sets two-sided errors -- these are never used as input)

    void setErrorLowHigh(double n_errorLow, double n_errorHigh, bool distribute = true) { 
      m_errorLow  = n_errorLow;
      m_errorHigh = n_errorHigh;
      if (distribute){
        for (unsigned int i = 0; i < m_constraints.size(); i++){
          otherParameter(m_constraints[i])->setErrorLowHigh(m_errorLow,m_errorHigh,false);
        }
      }
    }
    void resetErrorLowHigh() { setErrorLowHigh(-1.0,-1.0); }


      // manage steps (overrides the automatic setting from setValue or setError)

    void setStep(double n_step, bool distribute = true) { 
      m_step = n_step;
      if (distribute){
        for (unsigned int i = 0; i < m_constraints.size(); i++){
          otherParameter(m_constraints[i])->setStep(m_step,false);
        }
      }
    }
    double getStepBasedOnValue(double n_value){ if (n_value == 0) return 0.1; return fabs(n_value) / 10.0; }
    void resetStep() { setStep(getStepBasedOnValue(m_initialValue)); }


      // manage fixed

    void setFixed(bool n_fixed, bool distribute = true) { 
      m_fixed = n_fixed;
      if (distribute){
        for (unsigned int i = 0; i < m_constraints.size(); i++){
          otherParameter(m_constraints[i])->setFixed(m_fixed,false);
        }
      }
    }
    void resetFixed() { setFixed(false); }


      // manage numbering

    void setParNumber(int n_parNumber, bool distribute = true) { 
      m_parNumber = n_parNumber;
      if (distribute){
        for (unsigned int i = 0; i < m_constraints.size(); i++){
          otherParameter(m_constraints[i])->setParNumber(n_parNumber,false);
        }
      }
    }
    void resetParNumber() { setParNumber(-1); }


      // manage constraints (just a bit confusing), note:
      //    * addConstraint propogates constraints to other parameters (as it should);
      //        it also initializes other parameters with this parameter (by convention)
      //    * resetConstraints removes all constraints from this parameter
      //        and takes this parameter out of other lists (as it should)
      //    * removeConstraint does not propogate changes and is only a helper
      //        for the resetConstraints method

    void addConstraint(TString n_fpName, bool distribute = true) { 
        // check:  don't constrain this parameter to itself
      if (n_fpName == m_fpName) return;
        // check:  don't add this constraint if it is already in the list
      for (unsigned int i = 0; i < m_constraints.size(); i++){
        if (n_fpName == m_constraints[i]) return;
      }
        // add this constraint
      m_constraints.push_back(n_fpName);
        // initialize the other parameter with this parameter 
      otherParameter(n_fpName)->initialize(this);
        // distribute this new constraint everywhere
      if (distribute){
          // these are all the local constraints:
        vector<TString> con1 = m_constraints;
                        con1.push_back(m_fpName);
          // these are all the remote constraints:
        vector<TString> con2 = otherParameter(n_fpName)->constraints();
                        con2.push_back(n_fpName);
          // link all combinations of local and remote constraints
        for (unsigned int i1 = 0; i1 < con1.size(); i1++){
        for (unsigned int i2 = 0; i2 < con2.size(); i2++){
          otherParameter(con1[i1])->addConstraint(con2[i2],false);
          otherParameter(con2[i2])->addConstraint(con1[i1],false);
        }}
      }
    }
    void removeConstraint(TString n_fpName) { 
      for (unsigned int i = 0; i < m_constraints.size(); i++){
        if (n_fpName == m_constraints[i]){
          m_constraints.erase(m_constraints.begin()+i);
          return;
        }
      }
    }
    void resetConstraints() {
      vector<TString> con1 = m_constraints;
      for (unsigned int i1 = 0; i1 < con1.size(); i1++){
        otherParameter(con1[i1])->removeConstraint(m_fpName);
      }
      m_constraints.clear();
    }


      // printing

    void print() { cout << m_parNumber << ". ";
                   printDescription();
                   if (m_constraints.size() > 0){
                     cout << "\t    constraints:" << endl;
                     printConstraints();
                   }
                   cout << "\t    initial value = " << initialValue() << endl;
                   cout << "\t    current value = " << value() << endl; 
                   cout << "\t      errors (parabolic, low, high) = (" << error() << "," 
                                               << errorLow() << "," << errorHigh() << ")" << endl;
                   cout << "\t      step                          = " << step() << endl;
                   if (m_fixed) cout << "\t\t FIXED" << endl; }

    void printDescription(){ cout << fpName() << "  (" << description() << ")" << endl; }
    void printConstraints(){ for (unsigned int i = 0; i < m_constraints.size(); i++){
                             cout << "\t\t";  otherParameter(m_constraints[i])->printDescription(); } }

  private:
    
    TString m_fName;
    TString m_pName;
    TString m_fpName;
    double m_value;
    double m_initialValue;
    double m_error;
    double m_errorLow;
    double m_errorHigh;
    double m_step;
    TString m_description;
    vector<TString> m_constraints;
    int m_parNumber;
    bool m_fixed;

};


// FSFitParameterList is a static class that manages the global m_fitParameterMap.


class FSFitParameterList{

  public:

    static void addParameter(TString fName, TString pName, double initialValue, TString description){ 
      FSFitParameter* fp = new FSFitParameter(fName,pName,initialValue,description);
      map<TString,FSFitParameter*>::iterator it = m_fitParameterMap.find(fp->fpName());
      if (it != m_fitParameterMap.end()){
        cout << "FSFitParameterList WARNING:  overwriting parameter " << fp->fpName() << endl;
        FSFitParameter* oldPar = it->second;
        oldPar->resetDefaults();
        delete oldPar;
      }
      m_fitParameterMap[fp->fpName()] = fp;
    }

    static FSFitParameter* getParameter(TString fName, TString pName){
      TString fpName(fName); fpName += ":"; fpName += pName;
      return getParameter(fpName);
    }

    static FSFitParameter* getParameter(TString fpName){
      map<TString,FSFitParameter*>::iterator it = m_fitParameterMap.find(fpName);
      if (it != m_fitParameterMap.end()) return it->second;
      cout << "FSFitParameterList ERROR:  cannot find parameter named " << fpName << endl;
      exit(0);
      return NULL;
    }

    static void constrainParameters(TString fpName1, TString fpName2, double value){
      FSFitParameter* par1 = getParameter(fpName1);
      par1->setValue(value);
      par1->addConstraint(fpName2);
    }

    static void constrainParameters(TString fpName1, TString fpName2){
      FSFitParameter* par1 = getParameter(fpName1);
      par1->addConstraint(fpName2);
    }

    static void removeConstraints(TString fpName){
      getParameter(fpName)->resetConstraints();
    }

    static void clearParameters(){
      for (map<TString,FSFitParameter*>::iterator it = m_fitParameterMap.begin();
           it != m_fitParameterMap.end(); it++){
        it->second->resetDefaults();
        delete it->second;
      }
      m_fitParameterMap.clear();
    }


  private:

    static map< TString, FSFitParameter* > m_fitParameterMap;

};





// ******************************
//  The FSFitFunction Class
// ******************************

// This class holds information about a fit function.
// All instances of FSFitFunction are owned by the global FSFitFunctionList.
// All parameter names are contained in fpNames()
//   -- the "Unique" one is used internally for constraints

class FSFitFunction{
  public:

    FSFitFunction(TString n_fName, double n_xLow, double n_xHigh):
       m_fName(n_fName), m_xLow(n_xLow), m_xHigh(n_xHigh) {
      if (n_fName == ""){
        m_fName = generateName();
      }
    }

    virtual ~FSFitFunction(){}

    virtual FSFitFunction* clone() = 0;

    FSFitFunction* cloneBASE();

    FSFitFunction* otherFunction(TString n_fName);

    TString generateName();

    TString          fName()         { return m_fName; }
    double           xLow()          { return m_xLow; }
    double           xHigh()         { return m_xHigh; }
    vector<TString>  fpNames()       { return m_fpNames; }
    vector<TString>  fpNamesUnique() { return m_fpNamesUnique; }

    void setXLow (double n_xLow)  { m_xLow  = n_xLow; }
    void setXHigh(double n_xHigh) { m_xHigh = n_xHigh; }


      // the actual function

    virtual double fx (double x) = 0;

    virtual double efx(double x) { return 0.0*x; }


      // the integral of the function

    virtual double integral(double x1, double x2) {
      cout << "FSFitFunction ERROR: integral is not defined" << endl;
      exit(0); 
      return -1.0 + 0.0*(x2-x1);
    }

    double integral(vector< pair<double,double> > xLimits){
      double total = 0.0;
      for (unsigned int i = 0; i < xLimits.size(); i++){
        if (xLimits[i].first < xLimits[i].second)
          total += integral(xLimits[i].first,xLimits[i].second);
      }
      return total;
    }

      // interface to TF1 (either of these works, I think)

    //Double_t rootFunction(Double_t* x, Double_t* par) { if (par == NULL) par = NULL; return fx(x[0]); }

    double operator() (double *x, double *p) { if (p == NULL) p = NULL; return fx(x[0]); }

    TF1* getTF1(){
      FSFitFunction* newff = cloneBASE();
      //TF1* ff = new TF1(newff->fName(), newff, &FSFitFunction::rootFunction, m_xLow, m_xHigh, 0);
      TF1* ff = new TF1(newff->fName(), newff, m_xLow, m_xHigh, 0);
      ff->SetNpx(2000);
      return ff;
    }


      // manage parameters

    void resetParameters(){
      for (unsigned int i = 0; i < m_fpNames.size(); i++){
         FSFitParameterList::getParameter(m_fpNames[i])->resetDefaults();
      }
    }


    void setParameters(double value1, double value2 = 0, double value3 = 0, double value4 = 0,
                                      double value5 = 0, double value6 = 0, double value7 = 0,
                                      double value8 = 0){
      if (m_fpNames.size() > 0) FSFitParameterList::getParameter(m_fpNames[0])->setValue(value1);
      if (m_fpNames.size() > 1) FSFitParameterList::getParameter(m_fpNames[1])->setValue(value2);
      if (m_fpNames.size() > 2) FSFitParameterList::getParameter(m_fpNames[2])->setValue(value3);
      if (m_fpNames.size() > 3) FSFitParameterList::getParameter(m_fpNames[3])->setValue(value4);
      if (m_fpNames.size() > 4) FSFitParameterList::getParameter(m_fpNames[4])->setValue(value5);
      if (m_fpNames.size() > 5) FSFitParameterList::getParameter(m_fpNames[5])->setValue(value6);
      if (m_fpNames.size() > 6) FSFitParameterList::getParameter(m_fpNames[6])->setValue(value7);
      if (m_fpNames.size() > 7) FSFitParameterList::getParameter(m_fpNames[7])->setValue(value8);
    }

    void setParErrors (double value1, double value2 = 0, double value3 = 0, double value4 = 0,
                                      double value5 = 0, double value6 = 0, double value7 = 0,
                                      double value8 = 0){
      if (m_fpNames.size() > 0) FSFitParameterList::getParameter(m_fpNames[0])->setError(value1);
      if (m_fpNames.size() > 1) FSFitParameterList::getParameter(m_fpNames[1])->setError(value2);
      if (m_fpNames.size() > 2) FSFitParameterList::getParameter(m_fpNames[2])->setError(value3);
      if (m_fpNames.size() > 3) FSFitParameterList::getParameter(m_fpNames[3])->setError(value4);
      if (m_fpNames.size() > 4) FSFitParameterList::getParameter(m_fpNames[4])->setError(value5);
      if (m_fpNames.size() > 5) FSFitParameterList::getParameter(m_fpNames[5])->setError(value6);
      if (m_fpNames.size() > 6) FSFitParameterList::getParameter(m_fpNames[6])->setError(value7);
      if (m_fpNames.size() > 7) FSFitParameterList::getParameter(m_fpNames[7])->setError(value8);
    }

    void setParSteps (double value1, double value2 = 0, double value3 = 0, double value4 = 0,
                                     double value5 = 0, double value6 = 0, double value7 = 0,
                                     double value8 = 0){
      if (m_fpNames.size() > 0) FSFitParameterList::getParameter(m_fpNames[0])->setStep(value1);
      if (m_fpNames.size() > 1) FSFitParameterList::getParameter(m_fpNames[1])->setStep(value2);
      if (m_fpNames.size() > 2) FSFitParameterList::getParameter(m_fpNames[2])->setStep(value3);
      if (m_fpNames.size() > 3) FSFitParameterList::getParameter(m_fpNames[3])->setStep(value4);
      if (m_fpNames.size() > 4) FSFitParameterList::getParameter(m_fpNames[4])->setStep(value5);
      if (m_fpNames.size() > 5) FSFitParameterList::getParameter(m_fpNames[5])->setStep(value6);
      if (m_fpNames.size() > 6) FSFitParameterList::getParameter(m_fpNames[6])->setStep(value7);
      if (m_fpNames.size() > 7) FSFitParameterList::getParameter(m_fpNames[7])->setStep(value8);
    }

      // done immediately before the numbers are needed, resets all the numbers and recounts

    int numberParameters(int startNumber){
      m_fpNamesUnique.clear();
      int npar = startNumber;
      for (unsigned int i = 0; i < m_fpNames.size(); i++){
        FSFitParameter* fp = FSFitParameterList::getParameter(m_fpNames[i]);
        if (fp->parNumber() == -1){
          fp->setParNumber(npar++);
          m_fpNamesUnique.push_back(m_fpNames[i]);
        }
      }
      return npar;
    }

    void resetNumberParameters(){
      for (unsigned int i = 0; i < m_fpNames.size(); i++){
        FSFitParameterList::getParameter(m_fpNames[i])->resetParNumber();
      }
    }

    void printParameters(){
      resetNumberParameters();
      numberParameters(1);
      for (unsigned int i = 0; i < m_fpNames.size(); i++){
         FSFitParameterList::getParameter(m_fpNames[i])->print();
      }
    }

  protected:

    TString m_fName;
    double m_xLow;
    double m_xHigh;
    vector<TString> m_fpNames;
    vector<TString> m_fpNamesUnique;


      // helper for the composite functions

    void addParameter(FSFitParameter* fp){
      for (unsigned int i = 0; i < m_fpNames.size(); i++){
        if (m_fpNames[i] == fp->fpName()){
          cout << "FSFitFunction ERROR:  parameter being used twice?  " << fp->fpName() << endl;
          exit(0);
        }
      }
      m_fpNames.push_back(fp->fpName());
    }

      // helpers for user functions

    void addParameter(TString pName, double initialValue, TString description){
      FSFitParameterList::addParameter(fName(),pName,initialValue,description);
      FSFitParameter* fp = FSFitParameterList::getParameter(fName(),pName);
      m_fpNames.push_back(fp->fpName());
    }

    double getParameterValue(TString pName){
      return FSFitParameterList::getParameter(fName(),pName)->value();
    }

};




class FSFitFunctionComposite : public FSFitFunction{
  public:

    FSFitFunctionComposite(TString n_fName, TString n_formula) :
      FSFitFunction(n_fName,0.0,0.0){ 

      m_fName1 = "";
      m_fName2 = "";
      for (unsigned int i = 1; i <= 4; i++){
        if (i == 1) m_sign = "+";
        if (i == 2) m_sign = "-";
        if (i == 3) m_sign = "*";
        if (i == 4) m_sign = "/";
        if (FSString::parseTStringSize(n_formula,m_sign) == 2){
          m_fName1 = FSString::parseTStringElement(n_formula,0,m_sign);
          m_fName2 = FSString::parseTStringElement(n_formula,1,m_sign);
          break;
        }
        m_sign = "";
      }
      if (m_sign == "" || m_fName1 == "" || m_fName2 == ""){
        cout << "FSFitFunctionComposite ERROR:  problem parsing formula " << n_formula << endl;
        exit(0);
      }

      FSFitFunction* fun1 = otherFunction(m_fName1);
      FSFitFunction* fun2 = otherFunction(m_fName2);


        //  put parameters into the composite

      if (fun1 == fun2){
        cout << "FSFitFunctionComposite ERROR:  same function used twice in " << n_formula << endl;
        exit(0);
      }
      vector<TString> fpNi[2];
      fpNi[0] = fun1->fpNames();
      fpNi[1] = fun2->fpNames();
      for (unsigned int i = 0; i < 2; i++){
        vector<TString> fpN = fpNi[i];
        for (unsigned int j = 0; j < fpN.size(); j++){
          FSFitParameter* fp = FSFitParameterList::getParameter(fpN[j]);
          addParameter(fp);
        }
      }


        // set limits

      if (fun1->xLow()  >= fun2->xLow())  setXLow (fun1->xLow());
      if (fun2->xLow()  >= fun1->xLow())  setXLow (fun2->xLow());
      if (fun1->xHigh() <= fun2->xHigh()) setXHigh(fun1->xHigh());
      if (fun2->xHigh() <= fun1->xHigh()) setXHigh(fun2->xHigh());

    }

    double fx(double x){
      FSFitFunction* fun1 = otherFunction(m_fName1);
      FSFitFunction* fun2 = otherFunction(m_fName2);
      if (m_sign == "+") return (fun1->fx(x) + fun2->fx(x));
      if (m_sign == "-") return (fun1->fx(x) - fun2->fx(x));
      if (m_sign == "*") return (fun1->fx(x) * fun2->fx(x));
      if (m_sign == "/") return (fun1->fx(x) / fun2->fx(x));
      return 0.0;
    }

    double efx(double x){
      FSFitFunction* fun1 = otherFunction(m_fName1);
      FSFitFunction* fun2 = otherFunction(m_fName2);
      if ((m_sign == "+") || (m_sign == "-")){
        double ef1 = fun1->efx(x);
        double ef2 = fun2->efx(x);
        return sqrt(ef1*ef1 + ef2*ef2);
      }
      if ((m_sign == "*") || (m_sign == "/")){
        double f   = fx(x);
        double f1  = fun1->fx(x);
        double f2  = fun2->fx(x);
        double ef1 = fun1->efx(x);
        double ef2 = fun2->efx(x);
        if ((f1 == 0) || (f2 == 0)) return 0.0;
        return f*sqrt((ef1/f1)*(ef1/f1) + (ef2/f2)*(ef2/f2));
      }
      return 0.0;
    }

    double integral(double x1, double x2){
      FSFitFunction* fun1 = otherFunction(m_fName1);
      FSFitFunction* fun2 = otherFunction(m_fName2);
      if (m_sign == "+") return (fun1->integral(x1,x2) + fun2->integral(x1,x2));
      if (m_sign == "-") return (fun1->integral(x1,x2) - fun2->integral(x1,x2));
      if ((m_sign == "*") || (m_sign == "/")){
        cout << "FSFitFunction ERROR: integral is not defined for * or /" << endl;
        exit(0);
      }
      return -1.0;
    }

    FSFitFunctionComposite* clone() { 
      FSFitFunction* fun1 = otherFunction(m_fName1)->cloneBASE();
      FSFitFunction* fun2 = otherFunction(m_fName2)->cloneBASE();
      TString formula("");
      formula += fun1->fName();
      formula += m_sign;
      formula += fun2->fName();
      return new FSFitFunctionComposite("",formula);
    }


  private:

    TString m_fName1;
    TString m_fName2;
    TString m_sign;


};




class FSFitFunctionList{

  public:

    static void addFunction(FSFitFunction* userFunction, TString fNameInitializer = ""){ 
      map<TString,FSFitFunction*>::iterator it = m_fitFunctionMap.find(userFunction->fName());
      if (it != m_fitFunctionMap.end()){
        cout << "FSFitFunctionList WARNING:  overwriting function " << userFunction->fName() << endl;
        it->second->resetParameters();
        delete it->second;
      }
      m_fitFunctionMap[userFunction->fName()] = userFunction;
      if (fNameInitializer != ""){
        FSFitFunction* funcInitializer = getFunction(fNameInitializer);
        vector<TString> fpNnew = userFunction->fpNames();
        vector<TString> fpNold = funcInitializer->fpNames();
        if (fpNnew.size() != fpNold.size()){
          cout << "FSFitFunctionList WARNING:  trying to initialize function " << userFunction->fName()
               << " with function " << fNameInitializer << " but they have different numbers of parameters "
               << endl;
          return;
        }
        for (unsigned int i = 0; i < fpNnew.size(); i++){
          FSFitParameter* oldPar = FSFitParameterList::getParameter(fpNold[i]);
          FSFitParameter* newPar = FSFitParameterList::getParameter(fpNnew[i]);
          newPar->initialize(oldPar);
        }
      }
    }

    static void addFunction(TString fName, TString function){ 
      FSFitFunctionComposite* func = new FSFitFunctionComposite(fName,function);
      addFunction(func); 
    }

    static FSFitFunction* getFunction(TString fName){
      map<TString,FSFitFunction*>::iterator it = m_fitFunctionMap.find(fName);
      if (it != m_fitFunctionMap.end()) return it->second;
      cout << "FSFitFunctionList ERROR:  cannot find function named " << fName << endl;
      exit(0);
      return NULL;
    }

    static void clearFunctions(){
      for (map<TString,FSFitFunction*>::iterator it = m_fitFunctionMap.begin();
           it != m_fitFunctionMap.end(); it++){
        delete it->second;
      }
      FSFitParameterList::clearParameters();
      m_fitFunctionMap.clear();
    }

    static TString generateName(){
      m_NAMECOUNTER++;
      TString newname("CLONEn");
      newname += m_NAMECOUNTER;
      return newname;
    }

  private:

    static map< TString, FSFitFunction* > m_fitFunctionMap;

    static int m_NAMECOUNTER;

};








class FSFitDataSet {
  public:

    FSFitDataSet(TString n_dName, TH1F* hist) : 
        m_dName(n_dName){
      int nbins = hist->GetNbinsX();
      for (int i = 1; i <= nbins; i++){
        m_x.push_back(hist->GetBinCenter(i));
        m_y.push_back(hist->GetBinContent(i));
        m_ey.push_back(hist->GetBinError(i));
      }
      clearLimits();
      fillSelected();
    }

    FSFitDataSet(TString n_dName, vector<FSXYPoint*> points, bool includeSystErrors) : 
        m_dName(n_dName){
      for (unsigned int i = 0; i < points.size(); i++){
        m_x.push_back(points[i]->xValue());
        m_y.push_back(points[i]->yValue());
        if (!includeSystErrors) m_ey.push_back(points[i]->yError());
        if  (includeSystErrors) m_ey.push_back(points[i]->yErrorTot());
      }
      clearLimits();
      fillSelected();
    }

    FSFitDataSet(TString n_dName, vector<double> xUnbinnedData) : 
        m_dName(n_dName), m_x(xUnbinnedData){
      m_y.clear();
      m_ey.clear();
      clearLimits();
      fillSelected();
    }

    void clearLimits() { 
      m_xLimits.clear(); 
      fillSelected();
    }
    void addLimits(double lowLimit, double highLimit) {
      m_xLimits.push_back(pair<double,double>(lowLimit,highLimit));
      fillSelected();
    }
    void setLimits(double lowLimit, double highLimit) {
      clearLimits();
      addLimits(lowLimit,highLimit);
    }

    TString dName()  { return m_dName; }

    int numPoints() { return m_x.size(); }

    vector<double>  x() { return m_xSelected; }
    vector<double>  y() { return m_ySelected; }
    vector<double> ey() { return m_eySelected; }

    bool hasY()  { return m_ySelected.size(); }
    bool hasEY() { return m_eySelected.size(); }

    vector< pair<double,double> > xLimits() { return m_xLimits; }

  private:

    TString m_dName;

    vector<double> m_x;
    vector<double> m_y;
    vector<double> m_ey;
    vector<double> m_xSelected;
    vector<double> m_ySelected;
    vector<double> m_eySelected;

    void fillSelected(){
      m_xSelected.clear();
      m_ySelected.clear();
      m_eySelected.clear();
      if (m_xLimits.size() == 0){
        m_xSelected = m_x;
        m_ySelected = m_y;
        m_eySelected = m_ey;
      }
      else{
        for (unsigned int i = 0; i < m_x.size(); i++){
          for (unsigned int j = 0; j < m_xLimits.size(); j++){
            double lowLimit = m_xLimits[j].first;
            double highLimit = m_xLimits[j].second;
            if ((m_x[i] > lowLimit) && (m_x[i] < highLimit)){
              m_xSelected.push_back(m_x[i]);
              if (hasY())  m_ySelected.push_back(m_y[i]);
              if (hasEY()) m_eySelected.push_back(m_ey[i]);
            }
          }
        }
      }
    }

    vector< pair<double,double> > m_xLimits;

};





class FSFitDataSetList {

  public:


    static void addDataSet(TString dName, TH1F* hist){ 
      FSFitDataSet* data = new FSFitDataSet(dName,hist);
      addDataSet(data);
    }

    static void addDataSet(TString dName, vector<FSXYPoint*> points, bool includeSystErrors){ 
      FSFitDataSet* data = new FSFitDataSet(dName,points,includeSystErrors);
      addDataSet(data);
    }

    static void addDataSet(TString dName, vector<double> xUnbinnedData){ 
      FSFitDataSet* data = new FSFitDataSet(dName,xUnbinnedData);
      addDataSet(data);
    }

    static void addDataSet(FSFitDataSet* data){
      if (!data){
        cout << "FSFitDataSetList ERROR: data=NULL in addDataSet" << endl;
        exit(0);
      }
      TString dName = data->dName();
      map<TString,FSFitDataSet*>::iterator it = m_fitDataSetMap.find(dName);
      if (it != m_fitDataSetMap.end()){
        cout << "FSFitDataSetList WARNING:  overwriting data set " << dName << endl;
        delete it->second;
      }
      m_fitDataSetMap[dName] = data;
    }

    static FSFitDataSet* getDataSet(TString dName){
      map<TString,FSFitDataSet*>::iterator it = m_fitDataSetMap.find(dName);
      if (it != m_fitDataSetMap.end()) return it->second;
      cout << "FSFitDataSetList ERROR:  cannot find data set named " << dName << endl;
      exit(0);
      return NULL;
    }

    static void clearDataSets(){
      for (map<TString,FSFitDataSet*>::iterator it = m_fitDataSetMap.begin();
           it != m_fitDataSetMap.end(); it++){
        delete it->second;
      }
      m_fitDataSetMap.clear();
    }


  private:

    static map< TString, FSFitDataSet* > m_fitDataSetMap;

};




class FSFitMinuit {

  public:

    FSFitMinuit(TString n_mName, TString fcnName = "CHI2") :
        m_mName(n_mName) {
      m_minuit = new TMinuit(1000);
      m_minuit->mncler();
      setFCN(fcnName);
    }

    TString mName() { return m_mName; }

    void migrad(){
      preFSFitSetup();
        Double_t calls[1];  calls[0] = 10000;
        Int_t err;
        m_minuit->mnexcm("MIGRAD",calls,1,err);
      postFSFitSetup(false);
    }

    void minos(){
      preFSFitSetup();
        Double_t calls[1];  calls[0] = 10000;
        Int_t err;
        m_minuit->mnexcm("MINOS",calls,1,err);
      postFSFitSetup(true);
    }

    double fcnValue(){
      double fmin;  double fedm;  double errdef;
      int npari; int nparx; int istat;
      m_minuit->mnstat(fmin, fedm, errdef, npari, nparx, istat);
      return fmin;
    }

    int nFreeParameters(){
      double fmin;  double fedm;  double errdef;
      int npari; int nparx; int istat;
      m_minuit->mnstat(fmin, fedm, errdef, npari, nparx, istat);
      return npari;
    }

    int fitStatus(){
      double fmin;  double fedm;  double errdef;
      int npari; int nparx; int istat;
      m_minuit->mnstat(fmin, fedm, errdef, npari, nparx, istat);
      return istat;
    }

    void setParameters(Double_t* par){
      int ipar = 0;
      vector< pair<TString,TString> > comps = fitComponents();
      for (unsigned int i = 0; i < comps.size(); i++){
        TString fName = comps[i].second;
        vector<TString> fpNames = FSFitFunctionList::getFunction(fName)->fpNamesUnique();
        for (unsigned int ip = 0; ip < fpNames.size(); ip++){
          FSFitParameterList::getParameter(fpNames[ip])->setValue(par[ipar++]);
        }
      }
    }

    double likelihood(){
      double fcn = 0.0;
      vector< pair<TString,TString> > comps = fitComponents();
      for (unsigned int i = 0; i < comps.size(); i++){
        TString dName = comps[i].first;
        TString fName = comps[i].second;
        vector<double> vx  = FSFitDataSetList::getDataSet(dName)->x();
        vector<double> vy  = FSFitDataSetList::getDataSet(dName)->y();
        if (vy.size() == 0){
          cout << "FSFit likelihood ERROR: no y values" << endl;
          exit(0);
        }
        FSFitFunction* func = FSFitFunctionList::getFunction(fName);
        for (unsigned int idata = 0; idata < vx.size(); idata++){
          double x = vx[idata];
          double y = vy[idata];
          double fx = func->fx(x);
          if (fx < 0) fcn += 1.0e3;  // NOT SURE EXACTLY HOW TO DO THIS 
          //if (fx < 0) fx = fabs(fx); 
          if (fx > 0) fcn += 2.0*(fx - y*log(fx));
        }
        if (fcn == 0.0) fcn += 1.0e6;
      }
      return fcn;
    }


    double unbinned(){
      double fcn = 0.0;
      vector< pair<TString,TString> > comps = fitComponents();
      for (unsigned int i = 0; i < comps.size(); i++){
        TString dName = comps[i].first;
        TString fName = comps[i].second;
        vector<double> vx  = FSFitDataSetList::getDataSet(dName)->x();
        FSFitFunction* func = FSFitFunctionList::getFunction(fName);
        double mu = func->integral(FSFitDataSetList::getDataSet(dName)->xLimits());
        if (mu <= 0){
          cout << "FSFitMinuit::unbinned ERROR: problem with normalization of function " 
               << func->fName() << endl;
          exit(0);
        }
        for (unsigned int idata = 0; idata < vx.size(); idata++){
          double x = vx[idata];
          double fx = func->fx(x);
          if (fx < 0) fcn += 1.0e3;  // NOT SURE EXACTLY HOW TO DO THIS 
          if (fx > 0) fcn += 2.0*(mu - log(fx));
        }
      }
      return fcn;
    }


    double chisquare(){
      double fcn = 0.0;
      vector< pair<TString,TString> > comps = fitComponents();
      for (unsigned int i = 0; i < comps.size(); i++){
        TString dName = comps[i].first;
        TString fName = comps[i].second;
        vector<double> vx  = FSFitDataSetList::getDataSet(dName)->x();
        vector<double> vy  = FSFitDataSetList::getDataSet(dName)->y();
        vector<double> vey = FSFitDataSetList::getDataSet(dName)->ey();
        if ((vy.size() == 0) || (vey.size() == 0)){
          cout << "FSFit chi2 ERROR: no y or ey values" << endl;
          exit(0);
        }
        FSFitFunction* func = FSFitFunctionList::getFunction(fName);
        for (unsigned int idata = 0; idata < vx.size(); idata++){
          double x = vx[idata];
          double y = vy[idata];
          double ey = vey[idata];
          double fx = func->fx(x);
          double efx = func->efx(x);
          if (ey > 0) fcn += (y-fx)*(y-fx)/(ey*ey+efx*efx);
        }
      }
      return fcn;
    }

    int nDataPoints(){
      int n = 0;
      vector< pair<TString,TString> > comps = fitComponents();
      for (unsigned int i = 0; i < comps.size(); i++){
        TString dName = comps[i].first;
        TString fName = comps[i].second;
        vector<double> vx  = FSFitDataSetList::getDataSet(dName)->x();
        n += vx.size();
      }
      return n;
    }



//***************************************
// SOME USEFUL TMINUIT FUNCTIONS
//***************************************
// TMinuit -- constructor
// mncler  --  clears parameters
// mnparm  --  define parameters (name,value,step,bounds)
// FixParameter -- fix a parameter
// SetFCN  -- to set the fcn
// Migrad -- migrad fit
// mnmnos -- minos fit
// mnemat -- get the error matrix
// mnerrs -- get the minos errors
// SetPrintLevel  --  suppress printing
// GetParameter  --  get parameter information
// mnhess   --  do a hess fit
// mnseek   --  look around the minimum
// mnstat  -- get statistics (fcn minimum, fit status)
//***************************************
//      Access to the fit covariance matrix
//      ===================================
//      Example1:
//         TH1F h("h","test",100,-2,2);
//         h.FillRandom("gaus",1000);
//         h.Fit("gaus");
//         Double_t matrix[3][3];
//         gMinuit->mnemat(&matrix[0][0],3);
//      Example2:
//         TH1F h("h","test",100,-2,2);
//         h.FillRandom("gaus",1000);
//         h.Fit("gaus");
//         TVirtualFitter *fitter = TVirtualFitter::GetFitter();
//         TMatrixD matrix(npar,npar,fitter->GetCovarianceMatrix());
//         Double_t errorFirstPar = fitter->GetCovarianceMatrixElement(0,0);



  private:

    TString m_mName;
    TMinuit* m_minuit;

    void preFSFitSetup(){
      m_minuit->mncler();
      setMNAME();
      Int_t outflag;
      vector< pair<TString,TString> > comps = fitComponents();
        // first reset all parameter numbers
      for (unsigned int i = 0; i < comps.size(); i++){
        TString fName = comps[i].second;
        FSFitFunctionList::getFunction(fName)->resetNumberParameters();
      }
      int npar = 1;
      for (unsigned int i = 0; i < comps.size(); i++){
        TString fName = comps[i].second;
        npar = FSFitFunctionList::getFunction(fName)->numberParameters(npar);
        vector<TString> fpNames = FSFitFunctionList::getFunction(fName)->fpNamesUnique();
        for (unsigned int ip = 0; ip < fpNames.size(); ip++){
          FSFitParameter* par = FSFitParameterList::getParameter(fpNames[ip]);
          m_minuit->mnparm(par->parNumber()-1,par->fpName(),par->value(),par->step(),  0.0,0.0, outflag);
          if (par->fixed()) m_minuit->FixParameter(par->parNumber()-1);
        }
      }
    }


    void postFSFitSetup(bool isMinosFit){
      vector< pair<TString,TString> > comps = fitComponents();
        // first reset all parameter numbers
      for (unsigned int i = 0; i < comps.size(); i++){
        TString fName = comps[i].second;
        FSFitFunctionList::getFunction(fName)->resetNumberParameters();
      }
      int npar = 1;
      for (unsigned int i = 0; i < comps.size(); i++){
        TString fName = comps[i].second;
        npar = FSFitFunctionList::getFunction(fName)->numberParameters(npar);
        vector<TString> fpNames = FSFitFunctionList::getFunction(fName)->fpNamesUnique();
        for (unsigned int ip = 0; ip < fpNames.size(); ip++){
          FSFitParameter* par = FSFitParameterList::getParameter(fpNames[ip]);
          double value; double error;
          m_minuit->GetParameter(par->parNumber()-1,value,error);
          par->setValue(value); par->setError(error);
          if (isMinosFit){
            double elow; double ehigh; double eparab; double globalCorrelation;
            m_minuit->mnerrs(par->parNumber()-1,ehigh,elow,eparab,globalCorrelation);
            par->setErrorLowHigh(fabs(elow),ehigh); par->setError(eparab);
          }
        }
      }
    }

    void setFCN(TString fcnName = "CHI2");

    void setMNAME();

    vector< pair<TString,TString> >  fitComponents();

};




class FSFitMinuitList {

  public:


    static void addMinuit(TString mName, TString fcnName = "CHI2"){ 
      map<TString,FSFitMinuit*>::iterator it = m_fitMinuitMap.find(mName);
      if (it != m_fitMinuitMap.end()){
        cout << "FSFitMinuitList WARNING:  overwriting fit " << mName << endl;
        delete it->second;
      }
      m_fitMinuitMap[mName] = new FSFitMinuit(mName,fcnName);
      m_fitComponentMap[mName] = vector< pair<TString,TString> >();
    }

    static FSFitMinuit* getMinuit(TString mName){
      map<TString,FSFitMinuit*>::iterator it = m_fitMinuitMap.find(mName);
      if (it != m_fitMinuitMap.end()) return it->second;
      cout << "FSFitMinuitList ERROR:  cannot find minuit fit named " << mName << endl;
      exit(0);
      return NULL;
    }

    static void clearMinuit(){
      for (map<TString,FSFitMinuit*>::iterator it = m_fitMinuitMap.begin();
           it != m_fitMinuitMap.end(); it++){
        delete it->second;
      }
      m_fitComponentMap.clear();
      m_fitMinuitMap.clear();
    }

    static void addFitComponent(TString mName, TString dName, TString fName){ 
      vector< pair<TString,TString> > comps = getFitComponents(mName);
      comps.push_back(pair<TString,TString>(dName,fName));
      m_fitComponentMap[mName] = comps;
    }

    static vector< pair<TString,TString> > getFitComponents(TString mName){
      map<TString,vector< pair<TString,TString> > >::iterator it = m_fitComponentMap.find(mName);
      if (it != m_fitComponentMap.end()) return it->second;
      cout << "FSFitMinuitList ERROR:  cannot find fit components for minuit fit named " << mName << endl;
      exit(0);
      return vector< pair<TString,TString> >();
    }


  private:

    static map< TString, FSFitMinuit* > m_fitMinuitMap;
    static map< TString, vector< pair<TString,TString> > > m_fitComponentMap;

};




class FSFitFCN{

  public:

    static TString MNAME;

    static void FCN_CHI2(Int_t& npar, Double_t* grad, Double_t& fcn, Double_t* par, Int_t iflag){
      npar = npar; grad = grad; iflag = iflag;  // to suppress warnings
      FSFitMinuit* minuit = FSFitMinuitList::getMinuit(FSFitFCN::MNAME);
      minuit->setParameters(par);
      fcn = minuit->chisquare();
    }

    static void FCN_LIKELIHOOD(Int_t& npar, Double_t* grad, Double_t& fcn, Double_t* par, Int_t iflag){
      npar = npar; grad = grad; iflag = iflag;  // to suppress warnings
      FSFitMinuit* minuit = FSFitMinuitList::getMinuit(FSFitFCN::MNAME);
      minuit->setParameters(par);
      fcn = minuit->likelihood();
    }

    static void FCN_UNBINNED(Int_t& npar, Double_t* grad, Double_t& fcn, Double_t* par, Int_t iflag){
      npar = npar; grad = grad; iflag = iflag;  // to suppress warnings
      FSFitMinuit* minuit = FSFitMinuitList::getMinuit(FSFitFCN::MNAME);
      minuit->setParameters(par);
      fcn = minuit->unbinned();
    }

};



#endif


