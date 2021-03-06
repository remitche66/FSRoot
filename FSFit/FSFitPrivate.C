#include "FSFit/FSFitPrivate.h"

map< TString, FSFitParameter* > FSFitParameterList::m_fitParameterMap;
map< TString, FSFitFunction* >  FSFitFunctionList::m_fitFunctionMap;
map< TString, FSFitDataSet* >   FSFitDataSetList::m_fitDataSetMap;
map< TString, FSFitMinuit* >    FSFitMinuitList::m_fitMinuitMap;
map< TString, vector< pair<TString,TString> > > FSFitMinuitList::m_fitComponentMap;
TString FSFitFCN::MNAME;
int FSFitFunctionList::m_NAMECOUNTER = 0;


FSFitParameter* FSFitParameter::otherParameter(TString n_fpName){
  return FSFitParameterList::getParameter(n_fpName);
}



FSFitFunction* FSFitFunction::otherFunction(TString n_fName){
  return FSFitFunctionList::getFunction(n_fName);
}

TString FSFitFunction::generateName(){
  return FSFitFunctionList::generateName();
}

FSFitFunction* FSFitFunction::cloneBASE(){
  FSFitFunction* newff = clone();
  FSFitFunctionList::addFunction(newff,this->fName());
  return newff;
}




void FSFitMinuit::setFCN(TString fcnName){
       if (fcnName == "CHI2")       m_minuit->SetFCN(FSFitFCN::FCN_CHI2);
  else if (fcnName == "LIKELIHOOD") m_minuit->SetFCN(FSFitFCN::FCN_LIKELIHOOD);
  else if (fcnName == "UNBINNED")   m_minuit->SetFCN(FSFitFCN::FCN_UNBINNED);
  else {
    cout << "FSFitMinuit ERROR:  no option for FCN with fcnName = " << fcnName << endl;
    cout << "                    options for FCN currently include CHI2, LIKELIHOOD, and UNBINNED" << endl;
    exit(0);
  }
}

void FSFitMinuit::setMNAME(){
  FSFitFCN::MNAME = m_mName;
}

const vector< pair<TString,TString> >&  FSFitMinuit::fitComponents(){
  return FSFitMinuitList::getFitComponents(m_mName);
}

