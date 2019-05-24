#if !defined(DATASET_H)
#define DATASET_H

#include <utility>
#include <map>
#include <vector>
#include "TString.h"


using namespace std;

// all errors absolute unless specified otherwise

class FSDataSet{

  public:

    TString     name()         { return m_name;}
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

    vector<TString> categories()  { return m_categories; }

    vector<FSDataSet*> subSets()  { return m_subSets; }

    TString     runCut();

    void display(int counter = -1);

    friend class FSDataSetList;

  private:

    FSDataSet(TString name);

    FSDataSet(TString  name,
              int      runStart,
              int      runEnd,
              double   ecm,
              double   ecmStatError,
              double   ecmSystError,
              double   lum,
              double   lumStatError,
              double   lumSystError,
              vector<TString> extraCategories);

    void addSubSet(FSDataSet* dataSet);

    bool hasCategory(TString cat);
    void addCategory(TString cat);

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
    vector<FSDataSet*> m_subSets;
    vector<TString> m_categories;

};


#endif
