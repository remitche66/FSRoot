#if !defined(DATASETLIST_H)
#define DATASETLIST_H

#include <utility>
#include <map>
#include <vector>
#include "TString.h"
#include "FSData/FSDataSet.h"


using namespace std;


class FSDataSetList{

  public:

    static void addDataSetsFromFile(TString fileName);

    static FSDataSet* getDataSet(TString category = "", double ecmGrouping = -1.0);

    static void display(TString category = "", double ecmGrouping = -1.0);

    static void clearDataSets();

  private:

    static void clearTempDataSets();
    static FSDataSet* findTempDataSet(TString name);

    static vector<FSDataSet*> getDataSetVector(TString category = "", double ecmGrouping = -1.0);

    static vector<FSDataSet*> m_vectorDataSets;
    static map< TString, FSDataSet* > m_mapTempDataSets;

};


#endif
