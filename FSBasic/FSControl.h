#if !defined(FSCONTROL_H)
#define FSCONTROL_H

#include <TROOT.h>
#include <TStyle.h>
#include "TString.h"
#include "TTreeFormula.h"
#include "TTree.h"
#include "TGaxis.h"
#include "TH2.h"
#include "TH1.h"

using namespace std;


class FSControl{

  public:

    static bool CHAINCACHING;       // turn on or off chain caching
    static bool HISTOGRAMCACHING;   // turn on or off histogram caching
    static bool DEBUG;              // verbose prints for debugging
    static bool QUIET;              // turns off almost all printing

    static bool DRAWMCCOMPONENTSBETTER;

    static bool REORDERMATH;        // reorder math in FSModeHistogram

    static void globalTweaks(){     // same as rootlogon.FSROOT.C
                                    //   (execute this when making plots
                                    //    in executables, for example)

// **********
// FORMATTING (also see FSHistogram::getTH1F and getTH2F)
// **********

gStyle->SetFillColor(10);
gStyle->SetCanvasColor(10);
gStyle->SetPadColor(10);
gStyle->SetFillStyle(1001);
gStyle->SetCanvasBorderMode(0);
gStyle->SetLineColor(kBlack);

gStyle->SetMarkerStyle(20);
gStyle->SetMarkerSize(0.5);

//gStyle->SetPadLeftMargin(0.145);
//gStyle->SetPadBottomMargin(0.15);
gStyle->SetPadTopMargin(0.100);
gStyle->SetPadBottomMargin(0.160);
gStyle->SetPadLeftMargin(0.145);
gStyle->SetPadRightMargin(0.115);
gStyle->SetCanvasDefH(600.0);
gStyle->SetCanvasDefW(600.0);

gStyle->SetStripDecimals(false);
gStyle->SetLabelSize(0.03, "Z");
gStyle->SetTitleOffset(1.5,"X");
gStyle->SetTitleOffset(1.8,"Y");

// ***********
// ROOT TWEAKS
// ***********

TTreeFormula::SetMaxima(100000,10000,10000);
TTree::SetMaxTreeSize(2e12);
TH1::SetDefaultSumw2(true);
TH2::SetDefaultSumw2(true);
TGaxis::SetMaxDigits(4);

    }

};



#endif
