#if !defined(FSHISTOGRAMANALYSIS_H)
#define FSHISTOGRAMANALYSIS_H

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

using namespace std;

class FSHistogramAnalysis{

  public:

             FSHistogramAnalysis(){}
    virtual ~FSHistogramAnalysis(){}

      // Do a Histogram Analysis

    void analyze(TChain* nt, string outputfile);


  protected:

      // Maps to Hold Histograms

    map<TString,TH1F*> h1map;
    map<TString,TH2F*> h2map;


      // Counters to Count Cut Combinations

    int n1cuts;
    int n2cuts;
    int n3cuts;
    int n4cuts;
    int n5cuts;
    int n6cuts;
    int n7cuts;
    int n8cuts;
    int n9cuts;


      // Booleans for Cuts

    map<int,bool> l1;  
    map<int,bool> l2;  
    map<int,bool> l3;  
    map<int,bool> l4;  
    map<int,bool> l5;  
    map<int,bool> l6;  
    map<int,bool> l7;  
    map<int,bool> l8;  
    map<int,bool> l9;


      // User Function Definitions

    virtual void setNCuts() = 0;
    virtual void bookHistograms(TString key) = 0;
    virtual void fillHistograms(TString key) = 0;
    virtual void setCuts() = 0;
    virtual void setupBranches(TChain* nt) = 0;


      // Standard Function Definitions

    void writeHistograms(string filename);


      // helper functions

    int cutFromKey(int cutNumber, TString key);

};


#endif
