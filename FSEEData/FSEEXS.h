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
    TString      sourceName()    { return m_sourceName; }
    FSEEDataSet* dataSet()       { return FSEEDataSetList::getDataSet(m_dataSetName); }

    double ecm()  { return dataSet()->ecm(); }
    double lum()  { return dataSet()->lum(); }
    double xs()   { return m_XS; }
    double exs()  { return m_EXS; }
    double eff()  { return m_EFF; }
    double bgxs() { return m_BGXS; }

    double nsignal()  { return xs()*lum()*eff(); }
    double ensignal() { if (xs() != 0) return nsignal()*exs()/xs(); return 0.0; }
    double nbg()      { return bgxs()*lum(); }

    vector<TString> xsCategories()  { return m_xsCategories; }

    void display(int counter = -1);


  private:

    void clearXS();

    FSEEXS(TString reactionName, TString dataSetName, TString sourceName){
      m_reactionName = reactionName;
      m_dataSetName = dataSetName;
      m_sourceName = sourceName;
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
    TString m_sourceName;
    vector<TString> m_xsCategories;

    double m_XS;
    double m_EXS;
    double m_EFF;
    double m_BGXS;

    friend class FSEEXSList;

};

#endif

