#if !defined(FSEEDATASETLIST_H)
#define FSEEDATASETLIST_H

#include <utility>
#include <map>
#include <vector>
#include "TString.h"
#include "FSEEData/FSEEDataSet.h"


using namespace std;


class FSEEDataSetList{

  public:

    static void addDataSetsFromFile(TString fileName);

    static FSEEDataSet* getDataSet(TString category = "", double ecmGrouping = -1.0);

    static void display(TString category = "", double ecmGrouping = -1.0);

    static void clearDataSets();

  private:

    static void clearTempDataSets();
    static FSEEDataSet* findTempDataSet(TString name);

    static vector<FSEEDataSet*> getDataSetVector(TString category = "", double ecmGrouping = -1.0);

    static vector<FSEEDataSet*> m_vectorDataSets;
    static map< TString, FSEEDataSet* > m_mapTempDataSets;

};


#endif
