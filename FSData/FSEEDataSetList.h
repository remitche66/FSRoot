#if !defined(FSEEDATASETLIST_H)
#define FSEEDATASETLIST_H

#include <utility>
#include <map>
#include <vector>
#include "TString.h"
#include "TH1F.h"
#include "FSBasic/FSString.h"
#include "FSData/FSEEDataSet.h"


using namespace std;


class FSEEDataSetList{

  public:

    static void addDataSetsFromFile(TString fileName);

    static FSEEDataSet* getDataSet(TString dsCategory = "",
                                   TString lumCategory = "", 
                                   double ecmGrouping = -1.0);

    static void display(TString dsCategory = "",
                        TString lumCategory = "", 
                        double ecmGrouping = -1.0);

    static TH1F* histLuminosity(TString dsCategory = "",
                                TString lumCategory = "", 
                                TString histBounds = "(5000,0.0,5.0)");

    static void clearDataSets();


  private:

    static void clearTempDataSets();
    static FSEEDataSet* findTempDataSet(TString name);

    static vector<FSEEDataSet*> getDataSetVector(TString dsCategory = "",
                                                 TString lumCategory = "", 
                                                 double ecmGrouping = -1.0);

    static vector<FSEEDataSet*> m_vectorDataSets;
    static map< TString, FSEEDataSet* > m_mapTempDataSets;

};


#endif
