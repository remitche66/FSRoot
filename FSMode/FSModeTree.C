#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>

#include "TROOT.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TChainElement.h"
#include "TObjArray.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "THStack.h"
#include "TLegend.h"
#include "FSBasic/FSCanvas.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSPhysics.h"
#include "FSMode/FSModeString.h"
#include "FSMode/FSModeTree.h"



  // ********************************************************
  // SKIM A TREE AND OUTPUT TO A DIFFERENT FILE
  // ********************************************************


void 
FSModeTree::skimTree(TString fileNameInput, TString ntName, TString category,
                         TString fileNameOutput, TString cuts, 
                	 unsigned int iModeStart, unsigned int iModeEnd){


    // make a list of modes

  vector<FSModeInfo*> modeVector  = FSModeCollection::modeVector(category);


    // check to see if we need to loop over modes

  bool loopOverModes = false;
  if (modeVector.size() > 0){
    if (modeVector[0]->modeString(ntName)         != ntName)         loopOverModes = true;
    if (modeVector[0]->modeString(fileNameInput)  != fileNameInput)  loopOverModes = true;
    if (modeVector[0]->modeString(fileNameOutput) != fileNameOutput) loopOverModes = true;
    if (modeVector[0]->modeCuts(cuts)             != cuts)           loopOverModes = true;
  }


    // loop over modes if we need to

  if (loopOverModes){
    for (unsigned int i = (iModeStart-1); i < modeVector.size() && i < iModeEnd; i++){
      TString ntName_i = modeVector[i]->modeString(ntName);
      TString fileNameInput_i = modeVector[i]->modeString(fileNameInput);
      TString fileNameOutput_i = modeVector[i]->modeString(fileNameOutput);
      TString cuts_i = modeVector[i]->modeCuts(cuts);
      FSTree::skimTree(fileNameInput_i,ntName_i,fileNameOutput_i,cuts_i);
    }
  }

    // otherwise do a simple skim

  if (!loopOverModes){
    FSTree::skimTree(fileNameInput,ntName,fileNameOutput,cuts);
  }

}


void 
FSModeTree::skimTree(TString fileNameInput, TString ntName, TString category,
                            TString variable, TString bounds,
                            TString cuts, TString fileNameOutput, 
                	    unsigned int iModeStart, unsigned int iModeEnd){

    // parse the bounds variable

  TString bounds2(bounds);
  while (bounds2.Contains("(")) bounds2.Replace(bounds2.Index("("),1,"");
  while (bounds2.Contains(")")) bounds2.Replace(bounds2.Index(")"),1,"");
  vector<TString> boundParts = FSString::parseTString(bounds2,",");
  if (boundParts.size() != 3){ cout << "FSModeTree::skimTree -- bad bounds: " << bounds << endl; return; }
  TString upperBound = boundParts[2];
  TString lowerBound = boundParts[1];

    // create a new cuts variable:
    //  OR(((variable)>lowerBound)&&((variable)<upperBound)&&(cuts))

  TString newCuts = "OR(((";
          newCuts += variable;
          newCuts += ")>";
          newCuts += lowerBound;
          newCuts += ")&&((";
          newCuts += variable;
          newCuts += ")<";
          newCuts += upperBound;
          newCuts += ")&&(";
          newCuts += cuts;
          newCuts += "))";

    // send this to the regular skimTree method

  skimTree(fileNameInput,ntName,category,fileNameOutput,newCuts,iModeStart,iModeEnd);

}



void
FSModeTree::createChi2Friends(TString fileName, TString ntName, TString category, bool mc){


    // get a list of modes associated with this category

  vector<FSModeInfo*> modeVector = FSModeCollection::modeVector(category);
  if (modeVector.size() == 0){
    cout << "FSModeTree:  there are no modes associated with this category..." << endl;
    cout << "                ... skipping createChi2Friends" << endl;
  }


  // *** STEP 1 *** 
  //           loop over all trees and record RUN, EVENT, CHI2, IMODE in
  //               map< pair<RUN,EVENT>, map<IMODE, vector<CHI2> > >

  map< pair<int,int>, map<int, vector<float> > >  chi2map;


    // loop over all modes in this category

  for (unsigned int i = 0; i < modeVector.size(); i++){

    TString fileName_i = modeVector[i]->modeString(fileName);
    TString ntName_i   = modeVector[i]->modeString(ntName);


      // set up the TChain and the RUN, EVENT, CHI2, and IMODE variables

    TChain* nt = FSTree::getTChain(fileName_i,ntName_i);
    if (!nt){
      cout << "FSModeTree: trouble creating TChain inside createChi2Friends" << endl;
      continue;
    }

    int IMODE = i + 1;
    Double_t RUN, EVENT, CHI2, MCHELP = 0.0;
    nt->SetBranchAddress("Run",    &RUN);
    nt->SetBranchAddress("Event",  &EVENT);
    nt->SetBranchAddress("Chi2",   &CHI2);
    if (mc) nt->SetBranchAddress("MCDecayParticlePxP1", &MCHELP);


      // loop over events and record information

    cout << "FSModeTree::createChi2Friends:  first loop over " << fileName_i << endl;

    for (int ientry = 0; ientry < nt->GetEntries(); ientry++){
      nt->GetEntry(ientry);
      if (ientry > 0 && ientry % 10000 == 0) cout << "\t" << ientry << endl;

      if (mc){ MCHELP = MCHELP-(int)MCHELP;  MCHELP = (int)(10000*MCHELP);  RUN *= MCHELP;  EVENT += MCHELP; }

      pair<int,int> pRunEvent = pair<int,int>((int)RUN,(int)EVENT);

      map<int, vector<float> > mModeChi2;
      if (chi2map.find(pRunEvent) != chi2map.end()) mModeChi2 = chi2map[pRunEvent];

      vector<float> vChi2;
      if (mModeChi2.find(IMODE) != mModeChi2.end()) vChi2 = mModeChi2[IMODE];

      vChi2.push_back((float)CHI2);
      std::sort(vChi2.begin(), vChi2.end());
      mModeChi2[IMODE] = vChi2;
      chi2map[pRunEvent] = mModeChi2;
    }

  }



  // *** STEP 2 *** 
  //            loop over the trees again and create the friend trees


  for (unsigned int i = 0; i < modeVector.size(); i++){

    TString fileName_i = modeVector[i]->modeString(fileName);
    TString ntName_i   = modeVector[i]->modeString(ntName);


      // set up the TChain and the RUN, EVENT, CHI2, and IMODE variables

    TChain* nt = FSTree::getTChain(fileName_i,ntName_i);
    if (!nt){
      cout << "FSModeTree: trouble creating TChain inside createChi2Friends" << endl;
      continue;
    }

    int IMODE = i + 1;
    Double_t RUN, EVENT, CHI2, MCHELP = 0.0;
    nt->SetBranchAddress("Run",    &RUN);
    nt->SetBranchAddress("Event",  &EVENT);
    nt->SetBranchAddress("Chi2",   &CHI2);
    if (mc) nt->SetBranchAddress("MCDecayParticlePxP1", &MCHELP);


      // create new files to hold the friend trees

    TString fileName_chi2(fileName_i);
    fileName_chi2 += ".chi";

    TFile* chi2TFile = new TFile(fileName_chi2,"recreate");
    chi2TFile->cd();


      // set up the new friend trees

    float EPSILON = 0.0001;

    TString ntName_chi2(ntName_i);
    ntName_chi2 += "_chi";

    TTree* chi2TTree = new TTree(ntName_chi2, ntName_chi2);
    Int_t NCOMBINATIONS;
    Int_t CHI2RANK;
    Int_t NCOMBINATIONSGLOBAL;
    Int_t CHI2RANKGLOBAL;
    chi2TTree->Branch("NCombinations",      &NCOMBINATIONS,      "NCombinations/I");
    chi2TTree->Branch("Chi2Rank",           &CHI2RANK,           "Chi2Rank/I");
    chi2TTree->Branch("NCombinationsGlobal",&NCOMBINATIONSGLOBAL,"NCombinationsGlobal/I");
    chi2TTree->Branch("Chi2RankGlobal",     &CHI2RANKGLOBAL,     "Chi2RankGlobal/I");


      // loop over events and record information

    cout << "FSModeTree::createChi2Friends:  second loop over " << fileName_i << endl;

    for (int ientry = 0; ientry < nt->GetEntries(); ientry++){
      nt->GetEntry(ientry);
      if (ientry > 0 && ientry % 10000 == 0) cout << "\t" << ientry << endl;

      if (mc){ MCHELP = MCHELP-(int)MCHELP;  MCHELP = (int)(10000*MCHELP);  RUN *= MCHELP;  EVENT += MCHELP; }

      pair<int,int> pRunEvent = pair<int,int>((int)RUN,(int)EVENT);

      map<int, vector<float> > mModeChi2;
      if (chi2map.find(pRunEvent) != chi2map.end()) mModeChi2 = chi2map[pRunEvent];

      vector<float> vChi2;
      if (mModeChi2.find(IMODE) != mModeChi2.end()) vChi2 = mModeChi2[IMODE];

      NCOMBINATIONS = vChi2.size();

      CHI2RANK = 1;
      for (unsigned int ichi2 = 0; ichi2 < vChi2.size(); ichi2++){
        if (vChi2[ichi2]+EPSILON < CHI2) CHI2RANK++;
      }

      NCOMBINATIONSGLOBAL = 0;
      CHI2RANKGLOBAL = 1;
      for (map<int, vector<float> >::const_iterator mItr = mModeChi2.begin(); 
           mItr != mModeChi2.end(); mItr++){
        vector<float> vChi2Temp = mItr->second;
        for (unsigned int ichi2 = 0; ichi2 < vChi2Temp.size(); ichi2++){
          NCOMBINATIONSGLOBAL++;
          if (vChi2Temp[ichi2]+EPSILON < CHI2) CHI2RANKGLOBAL++;
        }
      }

      chi2TTree->Fill();
    }

    chi2TFile->cd();
    chi2TTree->Write();
    //nt->AddFriend(chi2TTree);
    //nt->Write();
    delete chi2TFile;


  }

}


