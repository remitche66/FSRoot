#if !defined(FSEEDATASET_H)
#define FSEEDATASET_H

#include <utility>
#include <map>
#include <vector>
#include <cmath>
#include "TString.h"

using namespace std;

// NOTES: 
//  * all errors absolute unless specified otherwise
//  * all ecm are GeV
//  * all xs are pb

class FSEEDataSet{

  public:

    TString     name()         { return m_name; }
    vector<int> runStart()     { return m_runStart; }
    vector<int> runEnd()       { return m_runEnd; }
    double      ecm()          { return m_ecm; }
    double      ecmStatError() { return m_ecmStatError; }
    double      ecmSystError() { return m_ecmSystError; }
    double      ecmError()     { return sqrt(m_ecmStatError*m_ecmStatError
                                           + m_ecmSystError*m_ecmSystError); }
    double      ecmLow()       { return m_ecmLow; }
    double      ecmHigh()      { return m_ecmHigh; }
    double      lum()          { return m_lum; }
    double      lumStatError() { return m_lumStatError; }
    double      lumSystError() { return m_lumSystError; }
    double      lumError()     { return sqrt(m_lumStatError*m_lumStatError
                                           + m_lumSystError*m_lumSystError); }
    double      lumStatErrorRel() { if (m_lum > 0.0) 
                                    return m_lumStatError/m_lum; 
                                    return 0.0; }
    double      lumSystErrorRel() { if (m_lum > 0.0) 
                                    return m_lumSystError/m_lum; 
                                    return 0.0; }

    vector<TString> dsCategories()  { return m_dsCategories; }
    vector<TString> lumCategories() { return m_lumCategories; }

    vector<FSEEDataSet*> subSets()  { return m_subSets; }

    TString     runCut();

    void display(int counter = -1);


  private:

    FSEEDataSet();
    void setName(TString n_name);
    void clearDS();

    FSEEDataSet(TString  n_name,
              int      n_runStart,
              int      n_runEnd,
              double   n_ecm,
              double   n_ecmStatError,
              double   n_ecmSystError,
              double   n_lum,
              double   n_lumStatError,
              double   n_lumSystError,
              vector<TString> n_extraDSCategories);

    FSEEDataSet(TString  n_name,
              double   n_ecm,
              double   n_lum,
              vector<TString> n_extraDSCategories);

    void addSubSet(FSEEDataSet* dataSet);

    bool hasDSCategory(TString cat);
    void addDSCategory(TString cat, bool propagateToSubsets);

    bool hasLUMCategory(TString cat);
    void addLUMCategory(TString cat);
    void addLUMCategories();


    TString      m_name;
    vector<int>  m_runStart;
    vector<int>  m_runEnd;
    double       m_ecm;
    double       m_ecmStatError;
    double       m_ecmSystError;
    double       m_ecmLow;
    double       m_ecmHigh;
    double       m_lum;
    double       m_lumStatError;
    double       m_lumSystError;
    vector<FSEEDataSet*> m_subSets;
    vector<TString> m_dsCategories;
    vector<TString> m_lumCategories;

    static vector< pair<TString, pair<double,double> > > m_vectorLumCategories;

    friend class FSEEDataSetList;
    friend class FSEEXS;
    friend class FSEEXSList;

};


#endif
