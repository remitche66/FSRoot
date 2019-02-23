{

cout << "--- Loading the FSRoot Macros ---" << endl;

gStyle->SetFillColor(10);
gStyle->SetCanvasColor(10);
gStyle->SetPadColor(10);
gStyle->SetFillStyle(1001);
gStyle->SetCanvasBorderMode(0);

gStyle->SetMarkerStyle(20);
gStyle->SetMarkerSize(0.5);

gStyle->SetPadLeftMargin(0.145);
gStyle->SetPadBottomMargin(0.15);
gStyle->SetCanvasDefH(600.0);
gStyle->SetCanvasDefW(600.0);

gStyle->SetTitleOffset(1.5,"Y");

  //gStyle->SetPalette(1);
 // {int MAXCOLS = 250;  int COLSTART = 0;
 // int NCOLS = MAXCOLS - COLSTART;
 // TColor* pcols[MAXCOLS];
 // for (int i = 0; i < MAXCOLS; i++){
 //   float grey = i / (float) MAXCOLS;
 //   pcols[i] = new TColor(1000+(MAXCOLS-i-1),grey,grey,grey,"mycol"+(MAXCOLS-i-1));
 // }   
 // int cols[NCOLS];
 // for (int i = 0; i < NCOLS; i++){
 //   cols[i] = 1000+COLSTART+i;
 // }
 // gStyle->SetPalette(NCOLS,cols);}


TTreeFormula::SetMaxima(10000,1000,1000);
TTree::SetMaxTreeSize(100e9);


// *****************
// INCLUDE FILES
// *****************

gROOT->ProcessLine(".include ${FSROOT}");



/*  

// ********
// old things
// ********

// *****************
// BASIC UTILITIES
// *****************

cout << "Loading Utilities/ControlVariables.h..." << endl;
gROOT->LoadMacro("Utilities/ControlVariables.C+");

cout << "Loading Utilities/CanvasUtilities.C..." << endl;
gROOT->LoadMacro("Utilities/CanvasUtilities.C+");

cout << "Loading Utilities/StringUtilities.C..." << endl;
gROOT->LoadMacro("Utilities/StringUtilities.C+");

cout << "Loading Utilities/SystemUtilities.C..." << endl;
gROOT->LoadMacro("Utilities/SystemUtilities.C+");

cout << "Loading Utilities/PhysicsUtilities.C..." << endl;
gROOT->LoadMacro("Utilities/PhysicsUtilities.C+");

cout << "Loading Utilities/HistogramUtilities.C..." << endl;
gROOT->LoadMacro("Utilities/HistogramUtilities.C+");


// *****************
// FITTING UTILITIES
// *****************

cout << "Loading Utilities/FitPrivate.C..." << endl;
gROOT->LoadMacro("Utilities/FitPrivate.C+");

cout << "Loading Utilities/FitFunctions.C..." << endl;
gROOT->LoadMacro("Utilities/FitFunctions.C+");

cout << "Loading Utilities/FitUtilities.C..." << endl;
gROOT->LoadMacro("Utilities/FitUtilities.C+");


// *****************
// MODE UTILITIES
// *****************

cout << "Loading ModeMacros/ModeInfo.C..." << endl;
gROOT->LoadMacro("ModeMacros/ModeInfo.C+");

cout << "Loading ModeMacros/ModeCollection.C..." << endl;
gROOT->LoadMacro("ModeMacros/ModeCollection.C+");

cout << "Loading ModeMacros/ModeHistogram.C..." << endl;
gROOT->LoadMacro("ModeMacros/ModeHistogram.C+");


// *****************
// HISTOGRAM ANALYSIS
// *****************

cout << "Loading Analysis/HistogramAnalysis.C..." << endl;
gROOT->LoadMacro("Analysis/HistogramAnalysis.C+");


// *****************
// BES UTILITIES
// *****************

cout << "Loading BESUtilities/BESLuminosities.C..." << endl;
gROOT->LoadMacro("BESUtilities/BESLuminosities.C+");


cout << "Finished rootlogon" << endl;

*/


}

