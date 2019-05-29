#if !defined(FSEEXS_H)
#define FSEEXS_H

#include <iostream>
#include "TRandom3.h"
#include "FSEEData/FSEEDataSet.h"
#include "FSEEData/FSEEDataSetList.h"


using namespace std;

/*

every measurement is defined by:
  (ECM) (LUM) (XS) (EXS) (EFF) (BGXS)

variables used:
  -ECM = center of mass energy
  -LUM = luminosity
  -XS = cross section
  -EXS = error on cross section (assuming statistical is dominant)
  -EFF = efficiency (and ISR effects and branching fractions)
  -BGXS = background per luminosity
  +NSIGNAL = number of signal events
  +ENSIGNAL = error on signal events
  +NBG = number of background events

variables are related using:
  XS = NSIGNAL/(LUM*EFF)
  EXS/XS = ENSIGNAL/NSIGNAL
  ENSIGNAL = sqrt(NSIGNAL + NBG)
  BGXS = NBG/LUM

*/


class FSEEXS{

  public:


    TString      reactionName()  { return m_reactionName; }
    TString      dataSetName()   { return m_dataSetName; }
    FSEEDataSet* dataSet()       { return FSEEDataSetList::getDataSet(m_dataSetName); }

    double ECM()  { return dataSet()->ecm(); }
    double LUM()  { return dataSet()->lum(); }
    double XS()   { return m_XS; }
    double EXS()  { return m_EXS; }
    double EFF()  { return m_EFF; }
    double BGXS() { return m_BGXS; }

    double NSIGNAL()  { return XS()*LUM()*EFF(); }
    double ENSIGNAL() { if (XS() != 0) return NSIGNAL()*EXS()/XS(); return 0.0; }
    double NBG()      { return BGXS()*LUM(); }

    vector<TString> categories()  { return m_categories; }

    void display(int counter = -1);

    friend class FSEEXSList;

  private:

    void clearXS();

    FSEEXS(TString reactionName, TString dataSetName){
      m_reactionName = reactionName;
      m_dataSetName = dataSetName;
      clearXS();
    }

    void initWithXS1(double n_XS, double n_EXS, double n_NSIGNAL);
    void initWithXS2(double n_XS, int n_NSIGREGION, int n_NBGREGION, double n_SCALE,
                          double n_MANUALEFF = -1.0);
    void initWithXS3(double n_XS, int n_EXShigh, double n_EFF);
    void initWithXS4(double n_NSIGNAL, int n_ENSIGNAL, double n_EFF);

    void initWithPrediction(double n_XS, double n_EFF, double n_BGXS, bool n_fluctuate = false);

    bool hasXSCategory(TString cat);
    void addXSCategory(TString cat);

    TString m_reactionName;
    TString m_dataSetName;
    vector<TString> m_categories;

    double m_XS;
    double m_EXS;
    double m_EFF;
    double m_BGXS;


};

#endif

