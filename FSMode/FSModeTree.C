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

  fileNameInput  = FSString::removeWhiteSpace(fileNameInput);
  ntName         = FSString::removeWhiteSpace(ntName);
  category       = FSString::removeWhiteSpace(category);
  fileNameOutput = FSString::removeWhiteSpace(fileNameOutput);
  cuts           = FSString::removeWhiteSpace(cuts);  if (cuts == "") cuts = "1==1";


    // expand "cuts" using FSCut and check for multidimensional sidebands

  vector< pair<TString,double> > fsCuts = FSCut::expandCuts(cuts);
  if (fsCuts.size() == 1){ cuts = fsCuts[0].first; }
  else{ cout << "FSModeTree::skimTree Error: multidimensional sidebands not allowed" << endl; exit(1); }


    // make a list of modes

  vector<FSModeInfo*> modeVector  = FSModeCollection::modeVector(category);


    // check if we need to loop over modes

  bool loopOverModes = false;
  if (modeVector.size() > 0){
    if (modeVector[0]->modeString(ntName)         != ntName)         loopOverModes = true;
    if (modeVector[0]->modeString(fileNameInput)  != fileNameInput)  loopOverModes = true;
    if (modeVector[0]->modeString(fileNameOutput) != fileNameOutput) loopOverModes = true;
    if (modeVector[0]->modeString(cuts)           != cuts)           loopOverModes = true;
  }


    // loop over modes if we need to

  if (loopOverModes){
    for (unsigned int i = (iModeStart-1); i < modeVector.size() && i < iModeEnd; i++){
      TString ntName_i = modeVector[i]->modeString(ntName);
      TString fileNameInput_i = modeVector[i]->modeString(fileNameInput);
      TString fileNameOutput_i = modeVector[i]->modeString(fileNameOutput);
      TString cuts_i = modeVector[i]->modeString(cuts);
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

  fileNameInput  = FSString::removeWhiteSpace(fileNameInput);
  ntName         = FSString::removeWhiteSpace(ntName);
  category       = FSString::removeWhiteSpace(category);
  variable       = FSString::removeWhiteSpace(variable);
  bounds         = FSString::removeWhiteSpace(bounds);
  cuts           = FSString::removeWhiteSpace(cuts);  if (cuts == "") cuts = "1==1";
  fileNameOutput = FSString::removeWhiteSpace(fileNameOutput);

    // parse the bounds variable

  if (!FSString::checkBounds(1,bounds)){ 
    cout << "FSModeTree::skimTree -- bad bounds: " << bounds << endl; return; }
  TString bounds2(bounds);
  while (bounds2.Contains("(")) bounds2.Replace(bounds2.Index("("),1,"");
  while (bounds2.Contains(")")) bounds2.Replace(bounds2.Index(")"),1,"");
  vector<TString> boundParts = FSString::parseTString(bounds2,",");
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



  // ********************************************************
  // RANK EVENTS ACCORDING TO A GIVEN VARIABLE
  // ********************************************************


void
FSModeTree::createRankingTree(TString fileName, TString ntName, TString category, 
                              TString rankVarName, TString rankVar, TString cuts,
                              TString groupVar1, TString groupVar2){

  fileName    = FSString::removeWhiteSpace(fileName);
  ntName      = FSString::removeWhiteSpace(ntName);
  category    = FSString::removeWhiteSpace(category);
  rankVarName = FSString::removeWhiteSpace(rankVarName);
  rankVar     = FSString::removeWhiteSpace(rankVar);
  cuts        = FSString::removeWhiteSpace(cuts);  if (cuts == "") cuts = "1==1";
  groupVar1   = FSString::removeWhiteSpace(groupVar1);
  groupVar2   = FSString::removeWhiteSpace(groupVar2);

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

      // BOTH LOOPS: loop over all modes in this category 

    for (unsigned int i = 0; i < modeVector.size(); i++){
      int IMODE = i + 1;
      TString fileName_i  = modeVector[i]->modeString(fileName);
      TString ntName_i    = modeVector[i]->modeString(ntName);
      TString cuts_i      = modeVector[i]->modeString(cuts);
      TString rankVar_i   = modeVector[i]->modeString(rankVar);
      TString groupVar1_i = modeVector[i]->modeString(groupVar1);
      TString groupVar2_i = modeVector[i]->modeString(groupVar2);

      //cout << "fileName_i = " << fileName_i << endl;
      //cout << "ntName_i = " << ntName_i << endl;
      //cout << "cuts_i = " << cuts_i << endl;
      //cout << "rankVar_i = " << rankVar_i << endl;
      //cout << "groupVar1_i = " << groupVar1_i << endl;
      //cout << "groupVar2_i = " << groupVar2_i << endl;

        // BOTH LOOPS: prepare to read from the original tree for this mode

      TString treeSTATUS;
      TTree* nt = FSTree::getTChain(fileName_i,ntName_i,treeSTATUS);
      if (!nt || treeSTATUS.Contains("!!")){
        cout << "FSModeTree: trouble creating TChain inside createRankingTree"
             << " (status = " << treeSTATUS << ")" << endl;
        continue;
      }
      Double_t GROUPVAR1, GROUPVAR2, RANKVAR;
      if (cuts_i == "") cuts_i = "(1==1)";
      TObjArray* formulas = new TObjArray();
      TTreeFormula* cutsF = new TTreeFormula("cutsF", cuts_i,      nt); formulas->Add(cutsF);
      TTreeFormula* rvarF = new TTreeFormula("rvarF", rankVar_i,   nt); formulas->Add(rvarF);
      TTreeFormula* grp1F = new TTreeFormula("grp1F", groupVar1_i, nt); formulas->Add(grp1F);
      TTreeFormula* grp2F = new TTreeFormula("grp2F", groupVar2_i, nt); formulas->Add(grp2F);
      nt->SetNotify(formulas);

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
        TString sVARRANK             = rankVarName + "";
        TString sVARRANKGLOBAL       = rankVarName + "Global";
        TString sNCOMBINATIONS       = rankVarName + "Combinations";
        TString sNCOMBINATIONSGLOBAL = rankVarName + "CombinationsGlobal";
        rankTTree->Branch(sVARRANK,            &VARRANK,            sVARRANK+"/I");
        rankTTree->Branch(sVARRANKGLOBAL,      &VARRANKGLOBAL,      sVARRANKGLOBAL+"/I");
        rankTTree->Branch(sNCOMBINATIONS,      &NCOMBINATIONS,      sNCOMBINATIONS+"/I");
        rankTTree->Branch(sNCOMBINATIONSGLOBAL,&NCOMBINATIONSGLOBAL,sNCOMBINATIONSGLOBAL+"/I");
      }

        // BOTH LOOPS: loop over events in the original tree

      if (iLoop == 1) cout << "FSModeTree::createRankingTree:  first loop over " << fileName_i << endl;
      if (iLoop == 2) cout << "FSModeTree::createRankingTree:  second loop over " << fileName_i << endl;
      unsigned int nEvents = nt->GetEntries();
      for (unsigned int ientry = 0; ientry < nEvents; ientry++){
        if (ientry > 0 && ientry % 10000 == 0) cout << "\t" << ientry << endl;
        nt->GetEntry(ientry);
        bool SKIPENTRY = !cutsF->EvalInstance();
        GROUPVAR1 = grp1F->EvalInstance();    
        GROUPVAR2 = grp2F->EvalInstance();    
        RANKVAR   = rvarF->EvalInstance();    
        int IRANKVAR = (int)RANKVAR; 
        pair<int,int> pRunEvent = pair<int,int>((int)GROUPVAR1,(int)GROUPVAR2);
        map<int, vector<int> > mModeVar;
        if (rankMap.find(pRunEvent) != rankMap.end()) mModeVar = rankMap[pRunEvent];
        vector<int> vVarI;  vector<int> vVar0;
        if (mModeVar.find(IMODE) != mModeVar.end()) vVarI = mModeVar[IMODE];
        if (mModeVar.find(0)     != mModeVar.end()) vVar0 = mModeVar[0];

        //cout << "************" << endl;
        //cout << "ientry = " << ientry << endl;
        //cout << "GROUPVAR1 = " << GROUPVAR1 << endl;
        //cout << "GROUPVAR2 = " << GROUPVAR2 << endl;
        //cout << "RANKVAR   = " << RANKVAR << endl;
        //cout << "IRANKVAR  = " << IRANKVAR << endl;
        //cout << "SKIPENTRY = " << SKIPENTRY << endl;

          // FIRST LOOP: just record information

        if (iLoop == 1 && !SKIPENTRY){
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
          //cout << "vVarI = ";
          //for (unsigned int vvv = 0; vvv < vVarI.size(); vvv++){ cout << vVarI[vvv] << " "; }
          //cout << endl;
          NCOMBINATIONS = vVarI.size();
          VARRANK = -1;
          if (SKIPENTRY || vVarI.size() == 0){ VARRANK = -1; }
          else if (vVarI.size() == 1){ VARRANK = 1; }
          else if (vVarI.size() > 1){
            for (unsigned int ix = 0; ix < vVarI.size(); ix++){
              if (IRANKVAR == vVarI[ix]){
                VARRANK = ix+1;
                vVarI[ix] = -10000000;
                mModeVar[IMODE] = vVarI;
                rankMap[pRunEvent] = mModeVar;
                break;
              }
            }
          }
          //cout << "NCOMBINATIONS  = " << NCOMBINATIONS << endl;
          //cout << "VARRANK        = " << VARRANK << endl;
          //cout << "vVar0 = ";
          //for (unsigned int vvv = 0; vvv < vVar0.size(); vvv++){ cout << vVar0[vvv] << " "; }
          //cout << endl;
          NCOMBINATIONSGLOBAL = vVar0.size();
          VARRANKGLOBAL = -1;
          if (SKIPENTRY || vVar0.size() == 0){ VARRANKGLOBAL = -1; }
          else if (vVar0.size() == 1){ VARRANKGLOBAL = 1; }
          else if (vVar0.size() > 1){
            for (unsigned int ix = 0; ix < vVar0.size(); ix++){
              if (IRANKVAR == vVar0[ix]){
                VARRANKGLOBAL = ix+1;
                vVar0[ix] = -10000000;
                mModeVar[0] = vVar0;
                rankMap[pRunEvent] = mModeVar;
                break;
              }
            }
          }
          rankTTree->Fill();
          //cout << "NCOMBINATIONSGLOBAL  = " << NCOMBINATIONSGLOBAL << endl;
          //cout << "VARRANKGLOBAL        = " << VARRANKGLOBAL << endl;
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

        // BOTH LOOPS: clean up the TTreeFormula objects

      if (cutsF) delete cutsF;
      if (rvarF) delete rvarF;
      if (grp1F) delete grp1F;
      if (grp2F) delete grp2F;
      nt->SetNotify(nullptr);
      delete formulas;

    }
  }
}


