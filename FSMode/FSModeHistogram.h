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
                                TString cuts, double scale = 1.0, bool TESTONLY = false);

    static TH2F* getTH2F(TString fileName, TString ntName, TString category,
                                TString variable, TString bounds,
                                TString cuts, double scale = 1.0, bool TESTONLY = false);


      // ********************************************************
      // CREATE A TREE IN THE SAME WAY A HISTOGRAM IS CREATED ABOVE
      //   the tree is called "HistContents" and includes variables:
      //       "x" for the x variable
      //       "y" for the y variable (2d only)
      //       "wt" for a weight (set by scale and FSCut)
      //   as well as optional extra variables in extraTreeContents,
      //     where pair<TString,TString> = 
      //      pair< extra variable name, formula to calculate extra variable >
      // ********************************************************

    static TTree*  getTH1FContents(TString fileName, TString ntName, TString category,
                                   TString variable, TString bounds, 
                                   TString cuts, double scale = 1.0,
                                   vector< pair<TString,TString> > extraTreeContents = 
                                     vector< pair<TString,TString> >());


    static TTree*  getTH2FContents(TString fileName, TString ntName, TString category,
                                   TString variable, TString bounds, 
                                   TString cuts, double scale = 1.0,
                                   vector< pair<TString,TString> > extraTreeContents = 
                                     vector< pair<TString,TString> >());



      // ********************************************************
      // DRAW HISTOGRAMS SEPARATELY FOR EACH MODE
      // ********************************************************

    static void drawTH1F(TString fileName, TString ntName, TString category,
                                TString variable, TString bounds,
                                TString cuts,     TString options = "",
                                double scale = 1.0, TCanvas* c1 = NULL);

    static void drawTH2F(TString fileName, TString ntName, TString category,
                                TString variable, TString bounds,
                                TString cuts,     TString options = "",
                                double scale = 1.0, TCanvas* c1 = NULL);



      // ********************************************************
      // DECONSTRUCT A HISTOGRAM INTO ITS MC COMPONENTS
      //   * uses variables MCDecayCode1, MCDecayCode2, and MCExtras
      //      (described in the FSModeInfo.h header)
      //   * a single component is labeled by the string:
      //      MCExtras_MCDecayCode2_MCDecayCode1 
      // ********************************************************

    static TH1F* drawMCComponents(TString fileName, TString ntName, 
                                TString category, TString variable, 
                                TString bounds, TString cuts,
                                double scale = 1.0, TCanvas* c1 = NULL,
                                bool reverseOrder = false);

    static void drawMCComponentsSame(TString fileName, TString ntName, 
                                TString category, TString variable, 
                                TString bounds, TString cuts, double scale = 1.0,
                                bool reverseOrder = false);

    static vector< pair<TString, float> > 
        getMCComponentsAndSizes(TString fileName, TString ntName, 
                                TString category, TString variable, 
                                TString bounds, TString cuts,
                                double scale = 1.0, bool moreInfo = false, bool show = false);

    static vector<TString> 
                getMCComponents(TString fileName, TString ntName, 
                                TString category, TString variable, 
                                TString bounds, TString cuts,
                                double scale = 1.0, bool moreInfo = false, bool show = false);

    static vector<TH1F*>
            getMCComponentsTH1F(TString fileName, TString ntName, 
                                TString category, TString variable, 
                                TString bounds, TString cuts,
                                double scale = 1.0);

    static TString getMCComponentCut(TString component);    

    static TString formatMCComponent(TString component, float fraction = -1.0);


      // ********************************************************
      // DUMP OR READ A HISTOGRAM CACHE TO/FROM A FILE
      // ********************************************************

    static void dumpHistogramCache(TString cacheName = "", TString selectIndex = "*");
    static void readHistogramCache(TString cacheName = "", TString selectIndex = "*");



  private:


      // helper functions to interact with the FSHistogramInfo class

    static vector<TString> expandHistogramIndexTree(TString index);
    static pair<TString, vector<TString> >  getHistogramIndexTree(int dimension,
                                              TString fileName, TString ntName, TString category,
                                              TString variable, TString bounds,
                                              TString cuts, double scale = 1.0,
                                        vector< pair<TString,TString> > extraTreeContents = 
                                        vector< pair<TString,TString> >());



      // MC components

    static map<TString, map<TString, float> > m_cacheComponentsMap;
    static void dumpComponentsCache(TString cacheName = "");
    static void readComponentsCache(TString cacheName = "");


      // helper functions for histograms

    static void drawTHNF(int dimension,
                         TString fileName, TString ntName, TString category,
                         TString variable, TString bounds,
                         TString cuts,     TString options,
                         double scale, TCanvas* c1);

};


#endif
