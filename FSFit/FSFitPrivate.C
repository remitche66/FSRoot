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
  else {
    cout << "FSFitMinuit ERROR:  right now only CHI2 and LIKELIHOOD are defined for FCN" << endl;
    exit(0);
  }
}

void FSFitMinuit::setMNAME(){
  FSFitFCN::MNAME = m_mName;
}

vector< pair<TString,TString> >  FSFitMinuit::fitComponents(){
  return FSFitMinuitList::getFSFitComponents(m_mName);
}

