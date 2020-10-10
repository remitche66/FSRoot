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
#include "FSBasic/FSCut.h"
#include "FSMode/FSModeString.h"
#include "FSMode/FSModeTree.h"



  // ********************************************************
  // SKIM A TREE AND OUTPUT TO A DIFFERENT FILE
  // ********************************************************


void 
FSModeTree::skimTree(TString fileNameInput, TString ntName, TString category,
                         TString fileNameOutput, TString cuts, 
                	 unsigned int iModeStart, unsigned int iModeEnd,
                         TString printCommandFile){

    // expand "cuts" using FSCut and check for multidimensional sidebands

  vector< pair<TString,double> > fsCuts = FSCut::expandCuts(cuts);
  if (fsCuts.size() == 1){ cuts = fsCuts[0].first; }
  else{ cout << "FSModeTree::skimTree Error: multidimensional sidebands not allowed" << endl; exit(1); }


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
      TString printCommandFile_i = modeVector[i]->modeString(printCommandFile);
        if (printCommandFile == "") printCommandFile_i = "";
      FSTree::skimTree(fileNameInput_i,ntName_i,fileNameOutput_i,
                       cuts_i,"",printCommandFile_i);
    }
  }

    // otherwise do a simple skim

  if (!loopOverModes){
    FSTree::skimTree(fileNameInput,ntName,fileNameOutput,cuts,"",printCommandFile);
  }

}


void 
FSModeTree::skimTree(TString fileNameInput, TString ntName, TString category,
                            TString variable, TString bounds,
                            TString cuts, TString fileNameOutput, 
                	    unsigned int iModeStart, unsigned int iModeEnd,
                            TString printCommandFile){

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

  skimTree(fileNameInput,ntName,category,fileNameOutput,newCuts,
           iModeStart,iModeEnd,printCommandFile);

}



void
FSModeTree::createChi2Friends(TString fileName, TString ntName, TString category, bool mc){

    // get a list of modes associated with this category

  vector<FSModeInfo*> modeVector = FSModeCollection::modeVector(category);
  if (modeVector.size() == 0){
    cout << "FSModeTree:  there are no modes associated with this category..." << endl;
    cout << "                ... skipping createChi2Friends" << endl;
    return;
  }

    // *** LOOP 1 *** 
    //           loop over all trees and record RUN, EVENT, CHI2, IMODE in
    //               map< pair<RUN,EVENT>, map<IMODE, vector<CHI2> > >

    // *** LOOP 2 *** 
    //            loop over the trees again and create the friend trees
    //               using that map to rank entries

  map< pair<int,int>, map<int, vector<int> > >  chi2map;

  for (unsigned int iLoop = 1; iLoop <= 2; iLoop++){

      // loop over all modes in this category 

    for (unsigned int i = 0; i < modeVector.size(); i++){
      int IMODE = i + 1;
      TString fileName_i = modeVector[i]->modeString(fileName);
      TString ntName_i   = modeVector[i]->modeString(ntName);

        // BOTH LOOPS: prepare to read from the original tree for this mode

      TChain* nt = FSTree::getTChain(fileName_i,ntName_i);
      if (!nt){
        cout << "FSModeTree: trouble creating TChain inside createChi2Friends" << endl;
        continue;
      }
      Double_t RUN, EVENT, CHI2, MCHELP = 0.0;
      nt->SetBranchAddress("Run",    &RUN);
      nt->SetBranchAddress("Event",  &EVENT);
      nt->SetBranchAddress("Chi2",   &CHI2);
      if (mc) nt->SetBranchAddress("MCDecayParticlePxP1", &MCHELP);

        // SECOND LOOP: create friend trees

      TFile* chi2TFile = NULL;
      TTree* chi2TTree = NULL;
      Int_t NCOMBINATIONS;
      Int_t CHI2RANK;
      Int_t NCOMBINATIONSGLOBAL;
      Int_t CHI2RANKGLOBAL;
      if (iLoop == 2){
        TString fileName_chi2(fileName_i);  fileName_chi2 += ".chi";
        TString ntName_chi2(ntName_i);  ntName_chi2 += "_chi";
        chi2TFile = new TFile(fileName_chi2,"recreate");  chi2TFile->cd();
        chi2TTree = new TTree(ntName_chi2, ntName_chi2);
        chi2TTree->Branch("NCombinations",      &NCOMBINATIONS,      "NCombinations/I");
        chi2TTree->Branch("Chi2Rank",           &CHI2RANK,           "Chi2Rank/I");
        chi2TTree->Branch("NCombinationsGlobal",&NCOMBINATIONSGLOBAL,"NCombinationsGlobal/I");
        chi2TTree->Branch("Chi2RankGlobal",     &CHI2RANKGLOBAL,     "Chi2RankGlobal/I");
      }

        // loop over events in the original tree

      if (iLoop == 1) cout << "FSModeTree::createChi2Friends:  first loop over " << fileName_i << endl;
      if (iLoop == 2) cout << "FSModeTree::createChi2Friends:  second loop over " << fileName_i << endl;
      for (int ientry = 0; ientry < nt->GetEntries(); ientry++){
        nt->GetEntry(ientry);
        if (ientry > 0 && ientry % 10000 == 0) cout << "\t" << ientry << endl;
        if (CHI2 < 0 || CHI2 > 10000.0) CHI2 = 10000.0;  int ICHI2 = (int)(1000.0*CHI2);
        if (mc){ MCHELP = MCHELP-(int)MCHELP;  MCHELP = (int)(10000*MCHELP);  RUN += MCHELP;  EVENT += MCHELP; }
        pair<int,int> pRunEvent = pair<int,int>((int)RUN,(int)EVENT);
        map<int, vector<int> > mModeChi2;
        if (chi2map.find(pRunEvent) != chi2map.end()) mModeChi2 = chi2map[pRunEvent];
        vector<int> vChi2I;  vector<int> vChi20;
        if (mModeChi2.find(IMODE) != mModeChi2.end()) vChi2I = mModeChi2[IMODE];
        if (mModeChi2.find(0)     != mModeChi2.end()) vChi20 = mModeChi2[0];

          // FIRST LOOP: just record information

        if (iLoop == 1){
          vChi2I.push_back(ICHI2);
          vChi20.push_back(ICHI2);
          std::sort(vChi2I.begin(), vChi2I.end());
          std::sort(vChi20.begin(), vChi20.end());
          mModeChi2[IMODE] = vChi2I;
          mModeChi2[0]     = vChi20;
          chi2map[pRunEvent] = mModeChi2;
        }

          // SECOND LOOP: fill the friend tree

        if (iLoop == 2){
          NCOMBINATIONS = vChi2I.size();
          CHI2RANK = 1;
          if (vChi2I.size() > 1){
            for (unsigned int ix = 0; ix < vChi2I.size()-1; ix++){
              if (ICHI2 >= vChi2I[ix] && ICHI2 <= vChi2I[ix+1]){
                vChi2I[ix+1] = -10000000;
                mModeChi2[IMODE] = vChi2I;
                chi2map[pRunEvent] = mModeChi2;
                break;
              }
              CHI2RANK++;
            }
          }
          NCOMBINATIONSGLOBAL = vChi20.size();
          CHI2RANKGLOBAL = 1;
          if (vChi20.size() > 1){
            for (unsigned int ix = 0; ix < vChi20.size()-1; ix++){
              if (ICHI2 >= vChi20[ix] && ICHI2 <= vChi20[ix+1]){
                vChi20[ix+1] = -10000000;
                mModeChi2[0] = vChi20;
                chi2map[pRunEvent] = mModeChi2;
                break;
              }
              CHI2RANKGLOBAL++;
            }
          }
          chi2TTree->Fill();
        }

      }

        // SECOND LOOP: write out the friend tree

      if (iLoop == 2){
        chi2TFile->cd();
        chi2TTree->Write();
        //nt->AddFriend(chi2TTree);
        //nt->Write();
        delete chi2TFile;
      }

    }
  }
}




void
FSModeTree::createRankingTree(TString fileName, TString ntName, TString category, TString rankVar,
                              TString groupVar1, TString groupVar2, TString rankVarName){


    // get a list of modes associated with this category

  vector<FSModeInfo*> modeVector = FSModeCollection::modeVector(category);
  if (modeVector.size() == 0){
    cout << "FSModeTree:  there are no modes associated with this category..." << endl;
    cout << "                ... skipping createRankingTree" << endl;
    return;
  }

    // *** LOOP 1 *** 
    //           loop over all trees and record GROUPVAR1, GROUPVAR2, RANKVAR, IMODE in
    //               map< pair<GROUPVAR1,GROUPVAR2>, map<IMODE, vector<RANKVAR> > >

    // *** LOOP 2 *** 
    //            loop over the trees again and create the friend trees
    //               using that map to rank entries

  map< pair<int,int>, map<int, vector<int> > >  rankMap;

  for (unsigned int iLoop = 1; iLoop <= 2; iLoop++){

      // loop over all modes in this category 

    for (unsigned int i = 0; i < modeVector.size(); i++){
      int IMODE = i + 1;
      TString fileName_i = modeVector[i]->modeString(fileName);
      TString ntName_i   = modeVector[i]->modeString(ntName);

        // BOTH LOOPS: prepare to read from the original tree for this mode

      TChain* nt = FSTree::getTChain(fileName_i,ntName_i);
      if (!nt){
        cout << "FSModeTree: trouble creating TChain inside createRankingTree" << endl;
        continue;
      }
      Double_t GROUPVAR1, GROUPVAR2, RANKVAR;
      nt->SetBranchAddress(groupVar1,  &GROUPVAR1);
      nt->SetBranchAddress(groupVar2,  &GROUPVAR2);
      nt->SetBranchAddress(rankVar,    &RANKVAR);

        // SECOND LOOP: create friend trees

      TFile* rankTFile = NULL;
      TTree* rankTTree = NULL;
      Int_t VARRANK;
      Int_t VARRANKGLOBAL;
      Int_t NCOMBINATIONS;
      Int_t NCOMBINATIONSGLOBAL;
      if (iLoop == 2){
        TString fileName_var(fileName_i);  fileName_var += ".";  fileName_var += rankVarName;
        TString ntName_var(ntName_i);  ntName_var += "_";  ntName_var += rankVarName;
        rankTFile = new TFile(fileName_var,"recreate");  rankTFile->cd();
        rankTTree = new TTree(ntName_var, ntName_var);
        TString sVARRANK             = rankVarName + "Rank";
        TString sVARRANKGLOBAL       = rankVarName + "RankGlobal";
        TString sNCOMBINATIONS       = rankVarName + "RankCombinations";
        TString sNCOMBINATIONSGLOBAL = rankVarName + "RankCombinationsGlobal";
        rankTTree->Branch(sVARRANK,            &VARRANK,            sVARRANK+"/I");
        rankTTree->Branch(sVARRANKGLOBAL,      &VARRANKGLOBAL,      sVARRANKGLOBAL+"/I");
        rankTTree->Branch(sNCOMBINATIONS,      &NCOMBINATIONS,      sNCOMBINATIONS+"/I");
        rankTTree->Branch(sNCOMBINATIONSGLOBAL,&NCOMBINATIONSGLOBAL,sNCOMBINATIONSGLOBAL+"/I");
      }

        // loop over events in the original tree

      if (iLoop == 1) cout << "FSModeTree::createRankingTree:  first loop over " << fileName_i << endl;
      if (iLoop == 2) cout << "FSModeTree::createRankingTree:  second loop over " << fileName_i << endl;
      for (int ientry = 0; ientry < nt->GetEntries(); ientry++){
        nt->GetEntry(ientry);
        if (ientry > 0 && ientry % 10000 == 0) cout << "\t" << ientry << endl;
        int IRANKVAR = 0; int n = 0; while(n < 20 && abs(IRANKVAR) < 10000){IRANKVAR = (int)(pow(10,n)*RANKVAR);}
        pair<int,int> pRunEvent = pair<int,int>((int)GROUPVAR1,(int)GROUPVAR2);
        map<int, vector<int> > mModeVar;
        if (rankMap.find(pRunEvent) != rankMap.end()) mModeVar = rankMap[pRunEvent];
        vector<int> vVarI;  vector<int> vVar0;
        if (mModeVar.find(IMODE) != mModeVar.end()) vVarI = mModeVar[IMODE];
        if (mModeVar.find(0)     != mModeVar.end()) vVar0 = mModeVar[0];

          // FIRST LOOP: just record information

        if (iLoop == 1){
          vVarI.push_back(IRANKVAR);
          vVar0.push_back(IRANKVAR);
          std::sort(vVarI.begin(), vVarI.end());
          std::sort(vVar0.begin(), vVar0.end());
          mModeVar[IMODE] = vVarI;
          mModeVar[0]     = vVar0;
          rankMap[pRunEvent] = mModeVar;
        }

          // SECOND LOOP: fill the friend tree

        if (iLoop == 2){
          NCOMBINATIONS = vVarI.size();
          VARRANK = 1;
          if (vVarI.size() > 1){
            for (unsigned int ix = 0; ix < vVarI.size()-1; ix++){
              if (IRANKVAR >= vVarI[ix] && IRANKVAR <= vVarI[ix+1]){
                vVarI[ix+1] = -10000000;
                mModeVar[IMODE] = vVarI;
                rankMap[pRunEvent] = mModeVar;
                break;
              }
              VARRANK++;
            }
          }
          NCOMBINATIONSGLOBAL = vVar0.size();
          VARRANKGLOBAL = 1;
          if (vVar0.size() > 1){
            for (unsigned int ix = 0; ix < vVar0.size()-1; ix++){
              if (IRANKVAR >= vVar0[ix] && IRANKVAR <= vVar0[ix+1]){
                vVar0[ix+1] = -10000000;
                mModeVar[0] = vVar0;
                rankMap[pRunEvent] = mModeVar;
                break;
              }
              VARRANKGLOBAL++;
            }
          }
          rankTTree->Fill();
        }

      }

        // SECOND LOOP: write out the friend tree

      if (iLoop == 2){
        rankTFile->cd();
        rankTTree->Write();
        //nt->AddFriend(rankTTree);
        //nt->Write();
        delete rankTFile;
      }

    }
  }
}
