#if !defined(FSCUT_H)
#define FSCUT_H

#include <utility>
#include <vector>
#include <string>
#include <map>
#include "TString.h"


using namespace std;


class FSCut{

  public:

      // ********************************************************
      // DEFINE A CUT
      // ********************************************************

    static void defineCut(TString cutName, TString cut, 
                          TString cutSideBand = "", double weight = 1.0);


      // ********************************************************
      // DISPLAY CUTS
      // ********************************************************

    static void display(TString cutName = "");


      // ********************************************************
      // TEST CUTS
      // ********************************************************

    static void testCuts(TString cuts){ expandCuts(cuts,true); }


      // ********************************************************
      // EXPAND CUTS
      // ********************************************************

    static vector< pair<TString,double> > expandCuts(TString cuts, 
                                                     bool showDetails = false);


      // ********************************************************
      // CLEAR GLOBAL CACHES
      // ********************************************************

    static void clearCuts();


  private:

    static vector< pair<TString,double> > makeCut(vector<TString> cutList);
    static vector< pair<TString,double> > makeCutSB(vector<TString> cutList);
    static vector< pair<TString,double> > makeCutWT(vector<TString> cutList);
    static vector< pair<TString,double> > makeCutSBWT(vector<TString> cutList);
    static pair< pair<TString,TString>, double> getCut(TString cutName);


      // global caches

    static map< TString, pair< pair<TString,TString>, double > > m_cutCache;


};



#endif
