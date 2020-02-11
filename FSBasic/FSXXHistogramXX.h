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
#include "TF1.h"
#include "TF2.h"
#include "TFormula.h"


using namespace std;
class FSHistogramInfo;


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
                                TString cuts, double scale = 1.0);

    static TH2F* getTH2F(TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts, double scale = 1.0);


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
                                   TString cuts, double scale = 1.0,
                                   vector< pair<TString,TString> > extraTreeContents = 
                                     vector< pair<TString,TString> >());

    static TTree*  getTH2FContents(TString fileName, TString ntName,
                                   TString variable, TString bounds, 
                                   TString cuts, double scale = 1.0,
                                   vector< pair<TString,TString> > extraTreeContents = 
                                     vector< pair<TString,TString> >());


      // ********************************************
      //  CREATE HISTOGRAMS FROM FUNCTIONS
      // ********************************************

    static TH1F* getTH1FFormula(TString formula, TString bounds, int numRandomTrials = -1);
    static TH2F* getTH2FFormula(TString formula, TString bounds, int numRandomTrials = -1);
    static TH1F* getTH1FFormula(TF1* function, TString bounds, int numRandomTrials = -1);
    static TH2F* getTH2FFormula(TF2* function, TString bounds, int numRandomTrials = -1);


      // ********************************************
      //  MAKE PROJECTIONS
      // ********************************************

    static TH1F* getTH1F(TH2F* hist2d, TString projectionAxis = "X", bool function = false);

    static pair<double,double> integral(TH1F* hist, bool function = false);
    static pair<double,double> integral(TH2F* hist, bool function = false);


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

    static int    getNBins (TH1F* hist);
    static int    getNBinsX(TH1F* hist);
    static int    getNBinsX(TH2F* hist);
    static int    getNBinsY(TH2F* hist);
    static double getLow   (TH1F* hist);
    static double getLowX  (TH1F* hist);
    static double getLowX  (TH2F* hist);
    static double getLowY  (TH2F* hist);
    static double getHigh  (TH1F* hist);
    static double getHighX (TH1F* hist);
    static double getHighX (TH2F* hist);
    static double getHighY (TH2F* hist);
    

      // ********************************************************
      // DUMP OR READ A HISTOGRAM CACHE TO/FROM A FILE
      // ********************************************************

    static void dumpHistogramCache(string cacheName = "");
    static void readHistogramCache(string cacheName = "");


      // ********************************************************
      // KEEP RUNNING TOTALS FOR HISTOGRAMS
      //   reset using clearAddCache
      // ********************************************************

    static TH1F* addTH1F(TString addName, TH1F* hist, double scale = 1.0);
    static TH2F* addTH2F(TString addName, TH2F* hist, double scale = 1.0);

    static void clearAddCache(TString addName = "");


      // ********************************************************
      // CLEAR GLOBAL CACHES
      // ********************************************************

    static void clearHistogramCache();
    static void clearTempHistCache();


    //vector<TString> expandIndex(TString index);



//  private:

      // helper functions for histogram caching

    static TString getHistogramIndex(int dimension,
                            TString fileName, TString ntName,
                            TString variable, TString bounds,
                            TString cuts, double scale = 1.0,
                            bool useCat = false, TString cat = "");

    static TString getHistogramIndex(int dimension,
                            TString fileName, TString histName);

    static TString getHistogramIndex(map<TString,TString> indexMap);

    static map<TString,TString> parseHistogramIndex(TString index);


    friend class FSModeHistogram;
    friend class FSHistogramInfo;

  private:

    static FSHistogramInfo* getHistogramInfo(TString index, bool cache = true,
                                  vector<TString> expandedIndices = vector<TString>());

    static vector<TString> expandIndices(TString index);


      // **************************
      // MANAGE GLOBAL HISTOGRAM CACHES (private)
      // **************************

      // the global caches
    static map< TString, FSHistogramInfo* >  m_histogramInfoCache;
    static map< TString, pair<TH1F*,TH2F*> > m_tempCache;
    static map< TString, pair<TH1F*,TH2F*> > m_addCache;
    static unsigned int m_indexFSRootHistName;

      // make histogram names
    static TString makeFSRootHistName();

      // retrieve histograms
    //static pair<TH1F*,TH2F*> getHistogramFromCache(TString index);

      // add histograms
    //static void addHistogramToCache(TString index, TH1F* hist1d, TH2F* hist2d);
    static pair<TH1F*,TH2F*> addTempHistToCache(TH1F* hist1d, TH2F* hist2d);



/*
    static TString getIndex(TH1F* hist1d);
    static TString getIndex(TH2F* hist2d);
    static TString getIndex(pair<TH1F*,TH2F*> hist);
*/



    static pair<TH1F*,TH2F*> getTHNF(int dimension, TString fileName, 
                                     TString histName, TString index = "");

    static pair<TH1F*,TH2F*> getTHNF(int dimension,
                                TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts, double scale, TTree* histTree,
                                vector< pair<TString,TString> > extraTreeContents = 
                                  vector< pair<TString,TString> >());






    static pair<TH1F*,TH2F*> addTHNF(TString addName, 
                                     TH1F* hist1d, TH2F* hist2d, 
                                     double scale);

      // helper function for making histograms from functions

    static pair<TH1F*,TH2F*> getTHNFFormula(int dimension, TString formula, TString bounds, int numRandomTrials = -1);
    static pair<TH1F*,TH2F*> getTHNFFormulaRandom(int dimension, TString formula, TString bounds, int numRandomTrials = -1);


      // helper functions for getTHNFContents

    static TTree*  addTHNFContents(TTree* histTree, int dimension,
                                   TString fileName, TString ntName,
                                   TString variable, TString bounds, 
                                   TString cuts, double scale = 1.0,
                                   vector< pair<TString,TString> > extraTreeContents = 
                                     vector< pair<TString,TString> >());

    static TTree* setTHNFContents(int dimension,
                                vector< pair<TString,TString> > extraTreeContents = 
                                 vector< pair<TString,TString> >());

      // global caches



};


class FSHistogramInfo{
  friend class FSHistogram;
  friend class FSModeHistogram;
  private:
    FSHistogramInfo(TString index, vector<TString> expandedIndices);
    pair<TH1F*,TH2F*> getTHNF();
    TTree* getTHNFContents(TTree* histTree = NULL,
      vector< pair<TString,TString> > extraTreeContents = vector< pair<TString,TString> >());
    TString m_index;
    vector<TString> m_expandedIndices;
    TString m_histName;
    TH1F* m_hist1d;
    TH2F* m_hist2d;
};



#endif