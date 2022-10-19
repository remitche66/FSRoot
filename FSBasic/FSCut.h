#if !defined(FSCUT_H)
#define FSCUT_H

#include <utility>
#include <vector>
#include <string>
#include <map>
#include "TString.h"


using namespace std;
class FSCutInfo;


class FSCut{

  public:

      // ********************************************************
      // DEFINE A CUT
      // ********************************************************

    static void defineCut(TString cutName, TString cut, 
                          TString cutSideBand = "", double weight = 1.0);

    static void defineCut(TString cutName, TString cutVariable,
                            TString sigLow, TString sigHigh, 
                            TString sbLow1 = "", TString sbHigh1 = "",
                            TString sbLow2 = "", TString sbHigh2 = "", double weight = 1.0);


      // ********************************************************
      // VARY AND RESET CUTS
      // ********************************************************

    static void varyCut(TString cutName, TString cut,
                        TString cutSideBand = "", double weight = 1.0);

    static void varyCut(TString cutName, TString cutVariable,
                            TString sigLow = "", TString sigHigh = "", 
                            TString sbLow1 = "", TString sbHigh1 = "",
                            TString sbLow2 = "", TString sbHigh2 = "", double weight = 1.0);

    static void resetCut(TString cutName = "*");


      // ********************************************************
      // DRAW CUT ARROWS
      // ********************************************************

    static void drawCutArrows(TString cutName);


      // ********************************************************
      // DISPLAY CUTS
      // ********************************************************

    static void display(TString cutName = "*");


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
    static FSCutInfo* getCut(TString cutName);


      // global caches

    static map< TString, FSCutInfo* > m_cutCache;


};



class FSCutInfo{
  friend class FSCut;
  private:
    FSCutInfo(TString cutName, TString cut, TString cutSideBand, double weight):
      m_cutName(cutName), m0_cut(cut), m_cut(cut), 
      m0_cutSideBand(cutSideBand), m_cutSideBand(cutSideBand), 
      m0_cutVariable(""), m_cutVariable(""),
      m0_sSigLow (""),  m_sSigLow (""),
      m0_sSigHigh(""),  m_sSigHigh(""),
      m0_sSbLow1 (""),  m_sSbLow1 (""),
      m0_sSbHigh1(""),  m_sSbHigh1(""),
      m0_sSbLow2 (""),  m_sSbLow2 (""),
      m0_sSbHigh2(""),  m_sSbHigh2(""),
      m0_weight(weight), m_weight(weight) {}
    FSCutInfo(TString cutName, TString cutVariable, 
                            TString sigLow, TString sigHigh, 
                            TString sbLow1, TString sbHigh1,
                            TString sbLow2, TString sbHigh2, double weight):
      m_cutName(cutName), m0_cut(""), m_cut(""), 
      m0_cutSideBand(""), m_cutSideBand(""),  
      m0_cutVariable(cutVariable), m_cutVariable(cutVariable), 
      m0_sSigLow (sigLow), m_sSigLow (sigLow), 
      m0_sSigHigh(sigHigh),m_sSigHigh(sigHigh),
      m0_sSbLow1 (sbLow1), m_sSbLow1 (sbLow1), 
      m0_sSbHigh1(sbHigh1),m_sSbHigh1(sbHigh1),
      m0_sSbLow2 (sbLow2), m_sSbLow2 (sbLow2), 
      m0_sSbHigh2(sbHigh2),m_sSbHigh2(sbHigh2),
      m0_weight(weight), m_weight(weight) { makeCutStrings(); }
    void makeCutStrings();
    TString m_cutName;
    TString m0_cut;          TString m_cut;        
    TString m0_cutSideBand;  TString m_cutSideBand;
    TString m0_cutVariable;  TString m_cutVariable;
    TString m0_sSigLow;  TString m_sSigLow ;
    TString m0_sSigHigh; TString m_sSigHigh;
    TString m0_sSbLow1;  TString m_sSbLow1 ;
    TString m0_sSbHigh1; TString m_sSbHigh1;
    TString m0_sSbLow2;  TString m_sSbLow2 ;
    TString m0_sSbHigh2; TString m_sSbHigh2;
    double m0_weight;   double m_weight; 
};


#endif
