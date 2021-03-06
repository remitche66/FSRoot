#if !defined(FSXYPOINTLIST_H)
#define FSXYPOINTLIST_H

#include <utility>
#include <map>
#include <vector>
#include <cmath>
#include "TString.h"
#include "TH1F.h"
#include "TGraphAsymmErrors.h"
#include "FSBasic/FSString.h"
#include "FSData/FSXYPoint.h"


using namespace std;


class FSXYPointList{

  public:

    static void addXYPointsFromFile(TString fileName);

    static void addXYPointFromString(TString inputString);

    static void addDerivedXYPoints(TString newCategory, TString oldCategory1, TString operation, 
                                   TString oldCategory2, double xTolerance = 1.0e-8);

    static vector<FSXYPoint*> getXYPoints(TString category = "");

    static pair<double,double> getMinMax(TString category = "", bool includeErrors = false);

    static void display(TString category = "");

    static TH1F* getTH1F(TString category = "", TString histBounds = "(5000,0.0,5.0)", bool includeSystErrors = false);

    static TH1F* getEmptyTH1F(TString category = "", TString histBounds = "(5000,0.0,5.0)");

    static TGraphAsymmErrors* getTGraph(TString category = "", bool includeSystErrors = false, 
                     int color = 1, int markerStyle = 20, double lineWidth = 2.0, double markerSize = 1.0);

    static TF1* getIdeogram(TString category = "", TString XY = "X", bool includeSystErrors = false);

    static void clearXYPoints();


  private:

    static vector<FSXYPoint*> m_vectorXYPoints;

};


#endif
