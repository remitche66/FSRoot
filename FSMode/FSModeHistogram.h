#if !defined(FSMODEHISTOGRAM_H)
#define FSMODEHISTOGRAM_H

#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <map>
#include "TChain.h"
#include "TFile.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "FSBasic/FSControl.h"
#include "FSBasic/FSHistogram.h"
#include "FSMode/FSModeCollection.h"

using namespace std;


class FSModeHistogram{

  public:

      // ********************************************************
      // CREATE A HISTOGRAM FROM A TREE
      // ********************************************************

    static TH1F* getTH1F(TString fileName, TString ntName, TString category,
                                TString variable, TString bounds,
                                TString cuts,     TString options = "",
                                float scale = 1.0);

    static TH2F* getTH2F(TString fileName, TString ntName, TString category,
                                TString variable, TString bounds,
                                TString cuts,     TString options = "",
                                float scale = 1.0);


      // ********************************************************
      // DRAW HISTOGRAMS SEPARATELY FOR EACH MODE
      // ********************************************************

    static void drawTH1F(TString fileName, TString ntName, TString category,
                                TString variable, TString bounds,
                                TString cuts,     TString options = "",
                                float scale = 1.0, TCanvas* c1 = NULL);

    static void drawTH2F(TString fileName, TString ntName, TString category,
                                TString variable, TString bounds,
                                TString cuts,     TString options = "",
                                float scale = 1.0, TCanvas* c1 = NULL);



      // ********************************************************
      // DECONSTRUCT A HISTOGRAM INTO ITS MC COMPONENTS
      //   (using MCDecayCode1 and MCDecayCode2)
      // ********************************************************

    static TH1F* drawMCComponents(TString fileName, TString ntName, 
                                TString category, TString variable, 
                                TString bounds, TString cuts, TString options = "",
                                float scale = 1.0, TCanvas* c1 = NULL);

    static vector<TString> getMCComponents(TString fileName, TString ntName, 
                                TString category, TString variable, 
                                TString bounds, TString cuts, TString options = "",
                                float scale = 1.0, bool moreInfo = false);

    static float getMCComponentSize(TString modeString);


      // ********************************************************
      // DUMP OR READ A HISTOGRAM CACHE TO/FROM A FILE
      // ********************************************************

    static void dumpHistogramCache(string cacheName = "");
    static void readHistogramCache(string cacheName = "");


      // ********************************************************
      // CLEAR HISTOGRAMS
      //    [[only frees memory]]
      // ********************************************************

    static void clearHistogramCache();


      // ********************************************************
      // SKIM A TREE AND OUTPUT TO A DIFFERENT FILE
      // ********************************************************

    static void skimTree(TString fileNameInput, TString ntName, TString category,
                         TString fileNameOutput, TString cuts, 
                	 unsigned int iModeStart = 1, unsigned int iModeEnd = 1000000);

    static void skimTree(TString fileNameInput, TString ntName, TString category,
                            TString variable, TString bounds,
                            TString cuts, TString fileNameOutput, 
                	    unsigned int iModeStart = 1, unsigned int iModeEnd = 1000000);


      // ********************************************************
      // RANK EVENTS ACCORDING TO Chi2DOF
      //    To Use:
      //      1.  First create skims of events that are to be ranked.
      //             (For better organization, put these in a new directory.)
      //      2.  Run createChi2Friends on those events.
      //             New friend trees are created (in the same directory) with the variables:
      //                NCombinations:  number of combinations within a final state
      //                Chi2Rank: rank of this combination within a final state
      //                NCombinationsGlobal:  number of combinations in all final states
      //                Chi2RankGlobal:  rank of this combination in all final states
      //      3.  To access these new variables, set FSControl::CHAINFRIEND = "chi"
      // ********************************************************

    static void createChi2Friends(TString fileName, TString ntName, TString category, bool mc = false);









  private:

      // global cache

    static map<TString, pair<TH1F*,TH2F*> > m_modeHistogramCache;

      // MC components

    static map<TString, float> m_mcComponentsMap;
    static map<TString, map<TString, float> > m_cacheComponentsMap;
    static void dumpComponentsCache(string cacheName = "");
    static void readComponentsCache(string cacheName = "");


      // helper functions for histograms

    static pair<TH1F*,TH2F*> getHistogramGeneral(int dimension,
                                   TString fileName, TString ntName, TString category,
                                   TString variable, TString bounds,
                                   TString cuts,     TString options,
                                   float scale);

    static void drawHistogramGeneral(int dimension,
                                   TString fileName, TString ntName, TString category,
                                   TString variable, TString bounds,
                                   TString cuts,     TString options,
                                   float scale, TCanvas* c1);

};


#endif
