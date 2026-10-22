{

cout << "--------------------------------------------------" << endl;
cout << "--   Loading the FSRoot Shared Library (v4.0)   --" << endl;
cout << "--------------------------------------------------" << endl;


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


// *****************
// INCLUDE PATHS
// *****************

gROOT->ProcessLine(".include ${FSROOT}");
gInterpreter->AddIncludePath("${FSROOT}");


// *****************
// LOAD HEADER FILES
// *****************

// include all *.h files in the following FSRoot subdirectories
vector<TString> subDirNames = {"FSBasic", "FSMode", "FSData", "FSFit", "FSAnalysis"};
for (const auto& dirName : subDirNames) {
  const char* dirPath = gSystem->ExpandPathName(gSystem->Getenv("FSROOT") + TString("/") + dirName);
  void* directory = gSystem->OpenDirectory(dirPath);
  if (directory) {
    const char* entry;
    while ((entry = gSystem->GetDirEntry(directory))) {
      const TString name(entry);
      if (name == "." or name == "..") {
        continue;
      }
      if (name.EndsWith(".h")) {
				const TString include = dirName + "/" + name;
				cout << "Including " << include << endl;
				gInterpreter->ProcessLine(TString("#include \"") + include + "\"");
      }
    }
    gSystem->FreeDirectory(directory);
  } else {
    cout << "Warning: could not open directory " << dirPath << endl;
  }
}


// // *****************
// // BASIC UTILITIES
// // *****************

// cout << "Including FSBasic/FSControl.h" << endl;
// gInterpreter->ProcessLine("#include \"FSBasic/FSControl.h\"");

// cout << "Including FSBasic/FSCanvas.h" << endl;
// gInterpreter->ProcessLine("#include \"FSBasic/FSCanvas.h\"");

// cout << "Including FSBasic/FSString.h" << endl;
// gInterpreter->ProcessLine("#include \"FSBasic/FSString.h\"");

// cout << "Including FSBasic/FSSystem.h" << endl;
// gInterpreter->ProcessLine("#include \"FSBasic/FSSystem.h\"");

// cout << "Including FSBasic/FSMath.h" << endl;
// gInterpreter->ProcessLine("#include \"FSBasic/FSMath.h\"");

// cout << "Including FSBasic/FSPhysics.h" << endl;
// gInterpreter->ProcessLine("#include \"FSBasic/FSPhysics.h\"");

// cout << "Including FSBasic/FSCut.h" << endl;
// gInterpreter->ProcessLine("#include \"FSBasic/FSCut.h\"");

// cout << "Including FSBasic/FSTree.h" << endl;
// gInterpreter->ProcessLine("#include \"FSBasic/FSTree.h\"");

// cout << "Including FSBasic/FSHistogram.h" << endl;
// gInterpreter->ProcessLine("#include \"FSBasic/FSHistogram.h\"");


// // *****************
// // MODE UTILITIES
// // *****************

// cout << "Including FSMode/FSModeString.h" << endl;
// gInterpreter->ProcessLine("#include \"FSMode/FSModeString.h\"");

// cout << "Including FSMode/FSModeInfo.h" << endl;
// gInterpreter->ProcessLine("#include \"FSMode/FSModeInfo.h\"");

// cout << "Including FSMode/FSModeCollection.h" << endl;
// gInterpreter->ProcessLine("#include \"FSMode/FSModeCollection.h\"");

// cout << "Including FSMode/FSModeTree.h" << endl;
// gInterpreter->ProcessLine("#include \"FSMode/FSModeTree.h\"");

// cout << "Including FSMode/FSModeHistogram.h" << endl;
// gInterpreter->ProcessLine("#include \"FSMode/FSModeHistogram.h\"");


// // *****************
// // DATA UTILITIES
// // *****************

// cout << "Including FSData/FSXYPoint.h" << endl;
// gInterpreter->ProcessLine("#include \"FSData/FSXYPoint.h\"");

// cout << "Including FSData/FSXYPointList.h" << endl;
// gInterpreter->ProcessLine("#include \"FSData/FSXYPointList.h\"");

// cout << "Including FSData/FSEEDataSet.h" << endl;
// gInterpreter->ProcessLine("#include \"FSData/FSEEDataSet.h\"");

// cout << "Including FSData/FSEEDataSetList.h" << endl;
// gInterpreter->ProcessLine("#include \"FSData/FSEEDataSetList.h\"");

// cout << "Including FSData/FSEEXS.h" << endl;
// gInterpreter->ProcessLine("#include \"FSData/FSEEXS.h\"");

// cout << "Including FSData/FSEEXSList.h" << endl;
// gInterpreter->ProcessLine("#include \"FSData/FSEEXSList.h\"");


// // *****************
// // FITTING UTILITIES
// // *****************

// cout << "Including FSFit/FSFitPrivate.h" << endl;
// gInterpreter->ProcessLine("#include \"FSFit/FSFitPrivate.h\"");

// cout << "Including FSFit/FSFitFunctions.h" << endl;
// gInterpreter->ProcessLine("#include \"FSFit/FSFitFunctions.h\"");

// cout << "Including FSFit/FSFitUtilities.h" << endl;
// gInterpreter->ProcessLine("#include \"FSFit/FSFitUtilities.h\"");


// // *****************
// // ANALYSIS UTILITIES
// // *****************

// cout << "Including FSAnalysis/FSHistogramAnalysis.h" << endl;
// gInterpreter->ProcessLine("#include \"FSAnalysis/FSHistogramAnalysis.h\"");


// *****************
// SHARED LIBRARY
// *****************

gSystem->AddDynamicPath("${FSROOT}/lib");
cout << "Loading libFSRoot.so" << endl;
gSystem->Load("libFSRoot.so");


cout << "--------------------------------------------------" << endl;
cout << "-   Finished loading the FSRoot Shared Library   -" << endl;
cout << "--------------------------------------------------" << endl;

}
