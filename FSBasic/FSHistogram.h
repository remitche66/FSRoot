#if !defined(FSHISTOGRAM_H)
#define FSHISTOGRAM_H

#include <utility>
#include <vector>
#include <string>
#include <map>
#include "TChain.h"
#include "TChainElement.h"
#include "TFile.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"

using namespace std;


// useful global strings

extern TString FSAND;
extern TString FSOR;
extern TString FSNOT;


class FSHistogram{

  public:

      // ********************************************************
      // SET HISTOGRAM ATTRIBUTES
      // ********************************************************

    static TH1F* getTH1F(TH1F* hist);
    static TH2F* getTH2F(TH2F* hist);


      // ********************************************************
      // RETRIEVE A HISTOGRAM FROM A FILE AND CACHE IT
      // ********************************************************

    static TH1F* getTH1F(TString fileName, TString histName);
    static TH2F* getTH2F(TString fileName, TString histName);


      // ********************************************************
      // CREATE A HISTOGRAM FROM A TREE AND CACHE IT
      // ********************************************************

    static TH1F* getTH1F(TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,     TString options = "",
                                float scale = 1.0);

    static TH2F* getTH2F(TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,     TString options = "",
                                float scale = 1.0);

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

    static TTree*  getTH1FContents(TString fileName, TString ntName,
                                   TString variable, TString bounds, 
                                   TString cuts, TString options = "",
                                   float scale = 1.0,
                                   vector< pair<TString,TString> > extraTreeContents = 
                                     vector< pair<TString,TString> >());

    static TTree*  getTH2FContents(TString fileName, TString ntName,
                                   TString variable, TString bounds, 
                                   TString cuts, TString options = "",
                                   float scale = 1.0,
                                   vector< pair<TString,TString> > extraTreeContents = 
                                     vector< pair<TString,TString> >());


      // ********************************************************
      // SET MAXIMA/MINIMA SO HISTOGRAMS CAN BE SHOWN TOGETHER
      // ********************************************************

    static void setHistogramMaxima(TH1F* h1, TH1F* h2=NULL, 
                                   TH1F* h3=NULL, TH1F* h4=NULL, 
                                   TH1F* h5=NULL, TH1F* h6=NULL,
                                   TH1F* h7=NULL, TH1F* h8=NULL,
                                   TH1F* h9=NULL, TH1F* h10=NULL);

    static void setHistogramMinima(TH1F* h1, TH1F* h2=NULL, 
                                   TH1F* h3=NULL, TH1F* h4=NULL, 
                                   TH1F* h5=NULL, TH1F* h6=NULL,
                                   TH1F* h7=NULL, TH1F* h8=NULL,
                                   TH1F* h9=NULL, TH1F* h10=NULL);


      // ********************************************************
      // GET USEFUL INFORMATION ABOUT A HISTOGRAM
      // ********************************************************

    static int    getNBins(TH1F* hist);
    static double getLow  (TH1F* hist);
    static double getHigh (TH1F* hist);
    

      // ********************************************************
      // DUMP OR READ A HISTOGRAM CACHE TO/FROM A FILE
      // ********************************************************

    static void dumpHistogramCache(string cacheName = "");
    static void readHistogramCache(string cacheName = "");


      // ********************************************************
      // KEEP RUNNING TOTALS FOR HISTOGRAMS
      //   reset using clearAddCache
      // ********************************************************

    static TH1F* addTH1F(TString addName, TH1F* hist, float scale = 1.0);
    static TH2F* addTH2F(TString addName, TH2F* hist, float scale = 1.0);

    static void clearAddCache( TString addName = "" );


      // ********************************************************
      // CLEAR GLOBAL CACHES
      // ********************************************************

    static void clearHistogramCache();
    static void clearTempHistCache();

    friend class FSModeHistogram;

  private:

      // helper functions for histogram caching

    static TString getTH1FIndex(TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,     TString options = "",
                                float scale = 1.0);

    static TString getTH2FIndex(TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,     TString options = "",
                                float scale = 1.0);

    static TString getTH1FIndex(TH1F* hist1d);

    static TString getTH2FIndex(TH2F* hist2d);

    static pair<TH1F*,TH2F*> getHistogramFromCache(TString index);

    static pair<TH1F*,TH2F*> getTHNF(int dimension, 
                                     TString fileName, TString histName,
                                     TString index = "");

    static pair<TH1F*,TH2F*> getTHNF(int dimension,
                                TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,     TString options,
                                float scale, TTree* histTree,
                                vector< pair<TString,TString> > extraTreeContents = 
                                  vector< pair<TString,TString> >());

    static TString getTHNFIndex(int dimension,
                                TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,     TString options,
                                float scale);

    static TString getTHNFIndex(pair<TH1F*,TH2F*> hist);

    static TString makeHistogramName();
    static TString makeAddName();
    static TString makeTempHistName();

    static void addHistogramToCache(TString index, TH1F* hist1d, TH2F* hist2d);

    static pair<TH1F*,TH2F*> addTempHistToCache(TH1F* hist1d, TH2F* hist2d);

    static pair<TH1F*,TH2F*> addTHNF(TString addName, 
                                     TH1F* hist1d, TH2F* hist2d, 
                                     float scale);

      // helper functions for getTHNFContents

    static TTree*  addTHNFContents(TTree* histTree, int dimension,
                                   TString fileName, TString ntName,
                                   TString variable, TString bounds, 
                                   TString cuts, // TString options = "" 
                                   float scale = 1.0,
                                   vector< pair<TString,TString> > extraTreeContents = 
                                     vector< pair<TString,TString> >());

    static TTree* setTHNFContents(int dimension,
                                vector< pair<TString,TString> > extraTreeContents = 
                                 vector< pair<TString,TString> >());

      // global caches

    static map< TString, pair<TH1F*,TH2F*> > m_histogramCache;
    static map< TString, pair<TH1F*,TH2F*> > m_tempCache;
    static map< TString, pair<TH1F*,TH2F*> > m_addCache;
    static unsigned int m_addCacheTotalSize;

};



#endif
