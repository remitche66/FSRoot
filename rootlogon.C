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

TTreeFormula::SetMaxima(10000,1000,1000);
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


cout << "------------------------------------------" << endl;
cout << "--- Finished loading the FSRoot Macros ---" << endl;
cout << "------------------------------------------" << endl;


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

