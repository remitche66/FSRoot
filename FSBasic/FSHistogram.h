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
#include "ROOT/RDataFrame.hxx"

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

    static TH1F* getTH1F(TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts, double scale = 1.0);

    static TH2F* getTH2F(TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts, double scale = 1.0);

    static void testTH1F(TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,  double scale = 1.0);

    static void testTH2F(TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,  double scale = 1.0);


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
      // DUMP OR READ A HISTOGRAM CACHE TO/FROM A FILE
      // ********************************************************

    static void dumpHistogramCache(string cacheName = "");
    static void readHistogramCache(string cacheName = "");


      // ********************************************************
      // INTERACT WITH THE HISTOGRAM CACHE
      // ********************************************************

    static void clearHistogramCache(int histNumber = -1);
    static void showHistogramCache(int histNumber = -1);


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

    static pair<TH1F*,TH2F*> getTHNFBasicFile(int dimension, 
                                     TString fileName, TString histName);

    static pair<TH1F*,TH2F*> getTHNFBasicTree(int dimension,
                                TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts,     double scale);

    static pair<TH1F*,TH2F*> getTHNFBasicEmpty(int dimension, TString bounds, 
                                               TString hname = "");

    static pair<TH1F*,TH2F*> getTHNFBasicFormula(int dimension, TString formula, TString bounds);


    static TTree* getTHNFBasicContents(TTree* histTree, int dimension,
                                TString fileName, TString ntName,
                                TString variable, TString bounds,
                                TString cuts, double scale,
                                vector< pair<TString,TString> > extraTreeContents = 
                                  vector< pair<TString,TString> >());

    static pair< ROOT::RDF::RResultPtr<TH1D>, 
                 ROOT::RDF::RResultPtr<TH2D> >  getTHNFBasicTreeRDF(TString index);


      // ********************************************************
      // GENERALIZED BASIC FUNCTIONS CREATED FROM INDICES
      // ********************************************************

    static TTree* getTHNFBasicContents(TTree* histTree, TString index,
                                vector< pair<TString,TString> > extraTreeContents = 
                                  vector< pair<TString,TString> >());

    static pair<TH1F*,TH2F*> getTHNFBasicIndex(TString index);


      // *******************************************
      // INTERFACE TO THE FSHISTOGRAMINFO CLASS
      // *******************************************

    static vector<TString> expandHistogramIndex(TString index);
    static FSHistogramInfo* getFSHistogramInfo(TString index);


      // ***********************************************
      // helper functions for histogram indices
      // ***********************************************

    static TString getHistogramIndex(map<TString,TString> indexMap);
    static TString getHistogramIndexFile(int dimension, TString fileName, TString histName);
    static TString getHistogramIndexFormula(int dimension, TString formula, TString bounds);
    static TString getHistogramIndexEmpty(int dimension, TString bounds, TString histName);
    static TString getHistogramIndexTree(int dimension,
                            TString fileName, TString ntName,
                            TString variable, TString bounds,
                            TString cuts, double scale = 1.0,
                            bool useCat = false, TString cat = "");

    static map<TString,TString> parseHistogramIndex(TString index);

    static void printIndexInfo(TString index);
    static void printIndexInfo(map<TString,TString> indexMap);

      // make histogram names

    static TString makeFSRootHistName();
    static TString getFSRootHistName(int histNumber);
    static int getFSRootHistNumber(TString hName);
    static TString makeFSRootTempName();


      // global caches

    static map< TString, FSHistogramInfo* >  m_FSHistogramInfoCache;
    static unsigned int m_indexFSRootHistName;
    static unsigned int m_indexFSRootTempName;


      // *******************************************
      // INTERFACE TO RDATAFRAME
      // *******************************************

    static bool m_USEDATAFRAME;
    static bool m_USEDATAFRAMENOW;
    static map< TString, ROOT::RDataFrame* > m_RDataFrameCache;
    static map< TString, TString > m_RDFVariableDefinitions;
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
    TString getName(){ if (m_histPair.first)  return m_histPair.first->GetName();
                       if (m_histPair.second) return m_histPair.second->GetName(); 
                       return TString(""); };
    void show(){ cout << "  NAME = " << getName() << endl; 
                 cout << "  INDEX = " << m_index << endl; };

    TString m_index;
    bool m_waitingForEventLoop;
    vector<FSHistogramInfo*> m_basicHistograms;
    pair<TH1F*,TH2F*> m_histPair;
    pair< ROOT::RDF::RResultPtr<TH1D>, ROOT::RDF::RResultPtr<TH2D> > m_histPairRDF;

};


#endif
