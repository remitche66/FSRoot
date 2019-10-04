#if !defined(FSHISTOGRAMANALYSISEXAMPLE_H)
#define FSHISTOGRAMANALYSISEXAMPLE_H

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
#include "FSAnalysis/FSHistogramAnalysis.h"

using namespace std;

class FSHistogramAnalysisExample : public FSHistogramAnalysis{

  public:

  private:

    void setNCuts();
    void bookHistograms(TString key);
    void fillHistograms(TString key);
    void setCuts();
    void setupBranches(TChain* nt);

};


#endif
