#if !defined(FSXYPOINTLIST_H)
#define FSXYPOINTLIST_H

#include <utility>
#include <map>
#include <vector>
#include "TString.h"
#include "TH1F.h"
#include "FSBasic/FSString.h"
#include "FSData/FSXYPoint.h"


using namespace std;


class FSXYPointList{

  public:

    static void addXYPointsFromFile(TString fileName);

    static vector<FSXYPoint*> getXYPoints(TString category = "");

    static void display(TString category);

/*
    static TH1F* histLuminosity(TString dsCategory = "",
                                TString lumCategory = "", 
                                TString histBounds = "(5000,0.0,5.0)");
*/

    static void clearXYPoints();


  private:

    static vector<FSXYPoint*> m_vectorXYPoints;

};


#endif
