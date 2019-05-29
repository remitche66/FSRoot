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
  cout << "    categories: ";
  for (unsigned int j = 0; j < m_categories.size(); j++){
    if (j%4 == 0){  cout << endl;  cout << "        ";  }
    cout << m_categories[j] << "  ";
  }
  cout << endl;
  cout << "  ECM    = " <<  ECM() << endl;
  cout << "  LUM    = " <<  LUM() << endl;
  cout << "   XS    = " <<   XS() << endl;
  cout << "  EXS    = " <<  EXS() << endl;
  cout << "  EFF    = " <<  EFF() << endl;
  cout << " BGXS    = " << BGXS() << endl;
  cout << endl;
}


bool
FSEEXS::hasXSCategory(TString category){
  for (unsigned int i = 0; i < m_categories.size(); i++){
    if (m_categories[i] == category) return true;
  }
  return false;
}

void
FSEEXS::addXSCategory(TString category){
  if (!hasXSCategory(category) && category != "") m_categories.push_back(category);
}




      // ****************************
      // MEASUREMENTS
      //   input XS, EXS, and NSIGNAL 
      //     and derive EFF and BGXS
      // ****************************

void 
FSEEXS::initWithXS1(double n_XS, double n_EXS, double n_NSIGNAL){
  clearXS();
  if (LUM() <= 0)     { clearXS(); return; }
  if (n_XS == 0)      { clearXS(); return; }
  if (n_NSIGNAL == 0) { clearXS(); return; }
  m_XS = n_XS;
  m_EXS = n_EXS;
  m_EFF = n_NSIGNAL / (LUM()*XS());
  double nbg =  ENSIGNAL()*ENSIGNAL() - NSIGNAL();  if (nbg < 0) nbg = 0.0;
  m_BGXS = nbg / LUM();
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
  if (LUM() <= 0)     { clearXS(); return; }
  if (n_XS == 0)      { clearXS(); return; }
  m_XS = n_XS;
  double nsignal = n_NSIGREGION - (n_SCALE * n_NBGREGION);
  double ensignal = sqrt(n_NSIGREGION + (n_SCALE * n_SCALE * n_NBGREGION));
  double nbg = n_SCALE * n_NBGREGION;
  if (n_MANUALEFF > 0.0){ m_EFF = n_MANUALEFF; }
  else{ m_EFF = nsignal / (LUM() * XS()); }
  if (EFF() == 0)  { clearXS(); return; }
  m_EXS = ensignal / (LUM()*EFF());
  m_BGXS = nbg / LUM();
}


      // ****************************
      // MEASUREMENTS
      //   input XS, EXS_high, EFF
      //     and derive EXS, BGXS
      // ****************************

void 
FSEEXS::initWithXS3(double n_XS, int n_EXShigh, double n_EFF){
  clearXS();
  if (LUM() <= 0)     { clearXS(); return; }
  if (n_XS == 0)      { clearXS(); return; }
  if (n_EXShigh <= 0) { clearXS(); return; }
  if (n_EFF <= 0)     { clearXS(); return; }
  m_XS = n_XS;
  m_EXS = n_EXShigh - n_XS;
  m_EFF = n_EFF;
  double nbg =  ENSIGNAL()*ENSIGNAL() - NSIGNAL();  if (nbg < 0) nbg = 0.0;
  m_BGXS = nbg / LUM();
}


      // ****************************
      // MEASUREMENTS
      //   input NSIGNAL, ENSIGNAL, EFF
      //     and derive XS, EXS, BGXS
      // ****************************

void 
FSEEXS::initWithXS4(double n_NSIGNAL, int n_ENSIGNAL, double n_EFF){
  clearXS();
  if (LUM() <= 0)     { clearXS(); return; }
  if (n_NSIGNAL == 0) { clearXS(); return; }
  if (n_EFF <= 0)     { clearXS(); return; }
  m_EFF = n_EFF;
  m_XS = n_NSIGNAL/(EFF()*LUM());
  m_EXS = n_ENSIGNAL/(EFF()*LUM());
  double nbg =  ENSIGNAL()*ENSIGNAL() - NSIGNAL();  if (nbg < 0) nbg = 0.0;
  m_BGXS = nbg / LUM();
}


      // ****************************
      // PREDICTIONS (at new ECM)
      //   input LUM, XS, EFF and BGXS 
      //     and derive EXS
      //   update XS and EXS if fluctuate is true
      // ****************************

void 
FSEEXS::initWithPrediction(double n_XS, double n_EFF, double n_BGXS, bool n_fluctuate){
  if (LUM() <= 0)     { clearXS(); return; }
  if (n_XS == 0)      { clearXS(); return; }
  if (n_EFF <= 0)     { clearXS(); return; }
  m_XS = n_XS;
  m_EFF = n_EFF;
  m_BGXS = n_BGXS;
  if (n_fluctuate){
    double nsignal = gRandom->PoissonD(NSIGNAL()+NBG()) - NBG();
    if (nsignal < 0) nsignal = 0;
    m_XS = nsignal / (LUM()*EFF());
  }
  double ensignal = sqrt(NSIGNAL() + NBG());
  m_EXS = ensignal / (LUM()*EFF());
}

