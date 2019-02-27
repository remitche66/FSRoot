{

cout << "---------------------------------" << endl;
cout << "--- Loading the FSRoot Macros ---" << endl;
cout << "---------------------------------" << endl;


// **********
// FORMATTING
// **********

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


// ***********
// ROOT TWEAKS
// ***********

TTreeFormula::SetMaxima(100000,10000,10000);
TTree::SetMaxTreeSize(100e9);


// *****************
// INCLUDE FILES
// *****************

gROOT->ProcessLine(".include ${FSROOT}");


// *****************
// BASIC UTILITIES
// *****************

cout << "Loading FSBasic/FSControl.h..." << endl;
gROOT->LoadMacro("FSBasic/FSControl.C+");

cout << "Loading FSBasic/FSCanvas.C..." << endl;
gROOT->LoadMacro("FSBasic/FSCanvas.C+");

cout << "Loading FSBasic/FSString.h..." << endl;
gROOT->LoadMacro("FSBasic/FSString.C+");

cout << "Loading FSBasic/FSPhysics.C..." << endl;
gROOT->LoadMacro("FSBasic/FSPhysics.C+");

cout << "Loading FSBasic/FSTree.C..." << endl;
gROOT->LoadMacro("FSBasic/FSTree.C+");

cout << "Loading FSBasic/FSHistogram.C..." << endl;
gROOT->LoadMacro("FSBasic/FSHistogram.C+");


// *****************
// MODE UTILITIES
// *****************

cout << "Loading FSMode/FSModeString.C..." << endl;
gROOT->LoadMacro("FSMode/FSModeString.C+");

cout << "Loading FSMode/FSModeInfo.C..." << endl;
gROOT->LoadMacro("FSMode/FSModeInfo.C+");

cout << "Loading FSMode/FSModeCollection.C..." << endl;
gROOT->LoadMacro("FSMode/FSModeCollection.C+");

cout << "Loading FSMode/FSModeTree.C..." << endl;
gROOT->LoadMacro("FSMode/FSModeTree.C+");

cout << "Loading FSMode/FSModeHistogram.C..." << endl;
gROOT->LoadMacro("FSMode/FSModeHistogram.C+");


// *****************
// FITTING UTILITIES
// *****************

cout << "Loading FSFit/FSFitPrivate.C..." << endl;
gROOT->LoadMacro("FSFit/FSFitPrivate.C+");

cout << "Loading FSFit/FSFitFunctions.C..." << endl;
gROOT->LoadMacro("FSFit/FSFitFunctions.C+");

cout << "Loading FSFit/FSFitUtilities.C..." << endl;
gROOT->LoadMacro("FSFit/FSFitUtilities.C+");


cout << "------------------------------------------" << endl;
cout << "--- Finished loading the FSRoot Macros ---" << endl;
cout << "------------------------------------------" << endl;


/*  

// ********
// old things
// ********


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

