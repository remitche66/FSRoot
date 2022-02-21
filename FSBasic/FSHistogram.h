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
#include "RVersion.h"
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,18,0)
#include "ROOT/RDataFrame.hxx"
#endif

using namespace std;
class FSHistogramInfo;

// ***********************
// useful global strings
// ***********************

extern TString FSAND;   // = "&&'
extern TString FSOR;    // = "||"
extern TString FSNOT;   // = "!"


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

    static TH1F* getTH1F(TString fileName, TString ntName, TString variable, TString bounds,
                                TString cuts, double scale = 1.0, bool TESTONLY = false);

    static TH2F* getTH2F(TString fileName, TString ntName, TString variable, TString bounds,
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
      //  MAKE PROJECTIONS, DO INTEGRALS, ETC.
      // ********************************************

    static TH1F* getTH1FRandom(TH1F* hist, int numRandomTrials);
    static TH2F* getTH2FRandom(TH2F* hist, int numRandomTrials);

    static TH1F* getTH1F(TH2F* hist2d, TString projectionAxis = "X", bool function = false,
                                        double range1 = 0.0, double range2 = -1.0);

    static pair<double,double> integral(TH1F* hist, bool function = false, 
                                        double x1 = 0.0, double x2 = -1.0);
    static pair<double,double> integral(TH2F* hist, bool function = false,
                                        double x1 = 0.0, double x2 = -1.0,
                                        double y1 = 0.0, double y2 = -1.0);


      // ********************************************************
      // SET MAXIMA/MINIMA SO HISTOGRAMS CAN BE SHOWN TOGETHER
      //    MAXMIN = "MAXMIN": set maxima and minima
      //    MAXMIN = "MAX": set maxima
      //    MAXMIN = "MIN": set minima
      // ********************************************************

    static void setHistogramMaxMin(TH1F* hist, 
                                   bool zeroSuppression = false, TString MAXMIN = "MAXMIN");

    static void setHistogramMaxMin(vector<TH1F*> histVector,
                                   bool zeroSuppression = false, TString MAXMIN = "MAXMIN");

    static void setHistogramMaxMin(TH2F* hist, 
                                   bool zeroSuppression = false, TString MAXMIN = "MAXMIN");

    static void setHistogramMaxMin(vector<TH2F*> histVector,
                                   bool zeroSuppression = false, TString MAXMIN = "MAXMIN");
    

      // ********************************************************
      // DUMP OR READ A HISTOGRAM CACHE TO/FROM A FILE
      // ********************************************************

    static void dumpHistogramCache(TString cacheName = "", TString selectIndex = "*");
    static void readHistogramCache(TString cacheName = "", TString selectIndex = "*");


      // ********************************************************
      // INTERACT WITH THE HISTOGRAM CACHE
      //    histograms in the cache are named:  "FSRootHist:(histNumber)"
      //       for example, a histogram named "FSRootHist:000003"
      //                       has histNuber = 3
      //    indexComponent is a two-letter symbol that picks out
      //        part of the index (for example, "CU" is for cuts)
      // ********************************************************

    static void clearHistogramCache(TString histName = "", TString selectIndex = "*");
    static void showHistogramCache(TString histName = "", bool showDetails = false, TString selectIndex = "*");
    static TString getHistogramInfo(TString histName = "", TString indexComponent = "", TString selectIndex = "*");

    static void clearHistogramCache(int histNumber, TString selectIndex = "*");
    static void showHistogramCache(int histNumber, bool showDetails = false, TString selectIndex = "*");
    static TString getHistogramInfo(int histNumber, TString indexComponent = "", TString selectIndex = "*");


      // ********************************************************
      // INTERACT WITH RDATAFRAME
      // ********************************************************

    static void enableRDataFrame(bool executeImmediately = true, int numThreads = 0);
    static void executeRDataFrame();
    static void disableRDataFrame();



    friend class FSModeHistogram;
    friend class FSHistogramInfo;

  private:

      // ********************************************************
      // BASIC FUNCTIONS: HISTOGRAMS ALWAYS GO THROUGH THESE
      // ********************************************************

    static pair<TH1F*,TH2F*> getTHNFBasicFile(TString index, TString& STATUS);
    static pair<TH1F*,TH2F*> getTHNFBasicTree(TString index, TString& STATUS);
    static pair<TH1F*,TH2F*> getTHNFBasicEmpty(TString index);
    static pair<TH1F*,TH2F*> getTHNFBasicFormula(TString index, TString& STATUS);

    static TTree* getTHNFBasicContents(TTree* histTree, TString index,
                         vector< pair<TString,TString> > extraTreeContents, TString& STATUS);

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,18,0)
    static pair< ROOT::RDF::RResultPtr<TH1D>, 
                 ROOT::RDF::RResultPtr<TH2D> >  getTHNFBasicTreeRDF(TString index, TString& STATUS);
    static map< TString, ROOT::RDataFrame* > m_RDataFrameCache;
#endif


      // ********************************************************
      // GENERALIZED BASIC FUNCTIONS CREATED FROM INDICES
      // ********************************************************

    static pair<TH1F*,TH2F*> getTHNFBasicIndex(TString index, TString& STATUS);


      // *******************************************
      // INTERFACE TO THE FSHISTOGRAMINFO CLASS
      // *******************************************

    static FSHistogramInfo* getFSHistogramInfo(TString index, 
                          vector<TString> subIndices = vector<TString>());


      // ***********************************************
      // helper functions for histogram indices
      // ***********************************************

    static TString getHistogramIndex(map<TString,TString> indexMap);
    static TString getHistogramIndexFile(int dimension, TString fileName, TString histName);
    static TString getHistogramIndexFormula(int dimension, TString formula, TString bounds);
    static TString getHistogramIndexEmpty(int dimension, TString bounds);

    static vector<TString> expandHistogramIndexTree(TString index);
    static pair<TString, vector<TString> >  getHistogramIndexTree(int dimension,
                                                   TString fileName, TString ntName,
                                                   TString variable, TString bounds,
                                                   TString cuts, double scale = 1.0);

    static map<TString,TString> parseHistogramIndex(TString index);

    static void printIndexInfo(TString index);
    static void printIndexInfo(TString index, vector<TString> subIndices);

    static TString checkIndex(TString index, TString type = "");


      // make histogram names

    static TString makeFSRootHistName(int histNumber = -1);
    static int getFSRootHistNumber(TString hName);
    static TString makeFSRootTempName();


      // global caches

    static map< TString, FSHistogramInfo* >  m_FSHistogramInfoCache;
    static unsigned int m_indexFSRootHistName;
    static unsigned int m_indexFSRootTempName;
    static vector<TString> getFSHistogramInfoCacheIndices(TString histName = "", bool exactOnly = true,
                                                          TString selectIndex = "*");


      // *******************************************
      // INTERFACE TO RDATAFRAME
      // *******************************************

    static bool m_USEDATAFRAME;
    static bool m_USEDATAFRAMENOW;
    static map< TString, pair<TString,TString> > m_RDFVariableDefinitions;
    static unsigned int m_RDFVariableCounter;

};



  // *****************************************************
  // *****************************************************
  // FSHistogramInfo class (private):
  //    (1) holds histograms
  //    (2) manages loops over histograms
  //    (3) holds RDataFrame RResultPtr objects
  // *****************************************************
  // *****************************************************

class FSHistogramInfo{
  friend class FSHistogram;
  friend class FSModeHistogram;
  private:
    FSHistogramInfo(TString index, vector<TString> expandedIndices);
    pair<TH1F*,TH2F*> getTHNF();
    TTree* getTHNFContents(vector< pair<TString,TString> > extraTreeContents 
                              = vector< pair<TString,TString> >());
    TString getHistName(){ if (m_histPair.first)  return m_histPair.first->GetName();
                           if (m_histPair.second) return m_histPair.second->GetName(); 
                           return TString(""); };
    bool basic() { return (m_basicHistograms.size() == 0); }
    bool waitingForEventLoop();
    TString status();
    TString infoString();
    void show(bool showDetails = false);

    TString m_index;
    bool m_waitingForEventLoop;
    TString m_status;
    vector<FSHistogramInfo*> m_basicHistograms;
    pair<TH1F*,TH2F*> m_histPair;
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,18,0)
    pair< ROOT::RDF::RResultPtr<TH1D>, ROOT::RDF::RResultPtr<TH2D> > m_histPairRDF;
#endif

};


#endif
