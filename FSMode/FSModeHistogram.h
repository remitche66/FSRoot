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
#include "FSBasic/FSTree.h"
#include "FSBasic/FSHistogram.h"
#include "FSMode/FSModeCollection.h"
#include "FSMode/FSModeTree.h"

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



  private:


      // MC components

    static map<TString, float> m_mcComponentsMap;
    static map<TString, map<TString, float> > m_cacheComponentsMap;
    static void dumpComponentsCache(string cacheName = "");
    static void readComponentsCache(string cacheName = "");


      // helper functions for histograms

    static pair<TH1F*,TH2F*> getTHNF(int dimension,
                                   TString fileName, TString ntName, TString category,
                                   TString variable, TString bounds,
                                   TString cuts,     TString options,
                                   float scale);

    static void drawTHNF(int dimension,
                         TString fileName, TString ntName, TString category,
                         TString variable, TString bounds,
                         TString cuts,     TString options,
                         float scale, TCanvas* c1);

};


#endif
