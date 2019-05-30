#include <iostream>
#include "FSEEData/FSEEXS.h"

void
FSEEXS::clearXS(){
  m_XS =  0.0;
  m_EXS  = 0.0;
  m_EFF  = 0.0;
  m_BGXS = 0.0;
}


void
FSEEXS::display(int counter){
  if (counter >= 0) cout << counter << ". ";
  cout << "FSEEXS: " << endl;
  cout << "  reaction: " << reactionName() << endl;
  cout << "  dataSet:  " << dataSetName() << endl;
  cout << "  source:   " << sourceName() << endl;
  cout << "    categories: ";
  for (unsigned int j = 0; j < m_xsCategories.size(); j++){
    if (j%4 == 0){  cout << endl;  cout << "        ";  }
    cout << m_xsCategories[j] << "  ";
  }
  cout << endl;
  cout << "  ECM    = " <<  ecm() << endl;
  cout << "  LUM    = " <<  lum() << endl;
  cout << "   XS    = " <<   xs() << endl;
  cout << "  EXS    = " <<  exs() << endl;
  cout << "  EFF    = " <<  eff() << endl;
  cout << " BGXS    = " << bgxs() << endl;
  cout << endl;
}


bool
FSEEXS::hasXSCategory(TString category){
  for (unsigned int i = 0; i < m_xsCategories.size(); i++){
    if (m_xsCategories[i] == category) return true;
  }
  return false;
}

void
FSEEXS::addXSCategory(TString category){
  if (!hasXSCategory(category) && category != "") m_xsCategories.push_back(category);
}




      // ****************************
      // MEASUREMENTS
      //   input XS, EXS, and NSIGNAL 
      //     and derive EFF and BGXS
      // ****************************

void 
FSEEXS::initWithXS1(double n_XS, double n_EXS, double n_NSIGNAL){
  clearXS();
  if (lum() <= 0)     { clearXS(); return; }
  if (n_XS == 0)      { clearXS(); return; }
  if (n_NSIGNAL == 0) { clearXS(); return; }
  m_XS = n_XS;
  m_EXS = n_EXS;
  m_EFF = n_NSIGNAL / (lum()*xs());
  double NBG =  ensignal()*ensignal() - nsignal();  if (NBG < 0) NBG = 0.0;
  m_BGXS = NBG / lum();
}


      // ****************************
      // MEASUREMENTS
      //   input XS, signal and sidebands 
      //     and derive EFF, BGXS, and EXS
      // ****************************

void 
FSEEXS::initWithXS2(double n_XS, int n_NSIGREGION, int n_NBGREGION, double n_SCALE,
                          double n_MANUALEFF){
  clearXS();
  if (lum() <= 0)     { clearXS(); return; }
  if (n_XS == 0)      { clearXS(); return; }
  m_XS = n_XS;
  double NSIGNAL = n_NSIGREGION - (n_SCALE * n_NBGREGION);
  double eNSIGNAL = sqrt(n_NSIGREGION + (n_SCALE * n_SCALE * n_NBGREGION));
  double NBG = n_SCALE * n_NBGREGION;
  if (n_MANUALEFF > 0.0){ m_EFF = n_MANUALEFF; }
  else{ m_EFF = NSIGNAL / (lum() * xs()); }
  if (eff() == 0)  { clearXS(); return; }
  m_EXS = eNSIGNAL / (lum()*eff());
  m_BGXS = NBG / lum();
}


      // ****************************
      // MEASUREMENTS
      //   input XS, EXS_high, EFF
      //     and derive EXS, BGXS
      // ****************************

void 
FSEEXS::initWithXS3(double n_XS, int n_EXShigh, double n_EFF){
  clearXS();
  if (lum() <= 0)     { clearXS(); return; }
  if (n_XS == 0)      { clearXS(); return; }
  if (n_EXShigh <= 0) { clearXS(); return; }
  if (n_EFF <= 0)     { clearXS(); return; }
  m_XS = n_XS;
  m_EXS = n_EXShigh - n_XS;
  m_EFF = n_EFF;
  double NBG =  ensignal()*ensignal() - nsignal();  if (NBG < 0) NBG = 0.0;
  m_BGXS = NBG / lum();
}


      // ****************************
      // MEASUREMENTS
      //   input NSIGNAL, ENSIGNAL, EFF
      //     and derive XS, EXS, BGXS
      // ****************************

void 
FSEEXS::initWithXS4(double n_NSIGNAL, int n_ENSIGNAL, double n_EFF){
  clearXS();
  if (lum() <= 0)     { clearXS(); return; }
  if (n_NSIGNAL == 0) { clearXS(); return; }
  if (n_EFF <= 0)     { clearXS(); return; }
  m_EFF = n_EFF;
  m_XS = n_NSIGNAL/(eff()*lum());
  m_EXS = n_ENSIGNAL/(eff()*lum());
  double NBG =  ensignal()*ensignal() - nsignal();  if (NBG < 0) NBG = 0.0;
  m_BGXS = NBG / lum();
}


      // ****************************
      // PREDICTIONS (at new ECM)
      //   input LUM, XS, EFF and BGXS 
      //     and derive EXS
      //   update XS and EXS if fluctuate is true
      // ****************************

void 
FSEEXS::initWithPrediction(double n_XS, double n_EFF, double n_BGXS, bool n_fluctuate){
  if (lum() <= 0)     { clearXS(); return; }
  if (n_XS == 0)      { clearXS(); return; }
  if (n_EFF <= 0)     { clearXS(); return; }
  m_XS = n_XS;
  m_EFF = n_EFF;
  m_BGXS = n_BGXS;
  if (n_fluctuate){
    double NSIGNAL = gRandom->PoissonD(nsignal()+nbg()) - nbg();
    if (NSIGNAL < 0) NSIGNAL = 0;
    m_XS = NSIGNAL / (lum()*eff());
  }
  double eNSIGNAL = sqrt(nsignal() + nbg());
  m_EXS = eNSIGNAL / (lum()*eff());
}

