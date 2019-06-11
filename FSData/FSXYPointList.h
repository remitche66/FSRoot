#if !defined(FSXYPOINTLIST_H)
#define FSXYPOINTLIST_H

#include <utility>
#include <map>
#include <vector>
#include "TString.h"
#include "TH1F.h"
#include "TGraphAsymmErrors.h"
#include "FSBasic/FSString.h"
#include "FSData/FSXYPoint.h"


using namespace std;


class FSXYPointList{

  public:

//addDerivedXYPoints


    static void addXYPointsFromFile(TString fileName);

    static vector<FSXYPoint*> getXYPoints(TString category = "");

    static void display(TString category = "");

    static TH1F* getTH1F(TString category = "", TString histBounds = "(5000,0.0,5.0)");

    static TH1F* getEmptyTH1F(TString category = "", TString histBounds = "(5000,0.0,5.0)");

    static TGraphAsymmErrors* getTGraph(TString category = "");

    static void clearXYPoints();


  private:

    static vector<FSXYPoint*> m_vectorXYPoints;

};


#endif
