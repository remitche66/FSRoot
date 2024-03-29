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
#include "TLorentzVector.h"
#include "TGenPhaseSpace.h"
#include "TRandom.h"
#include "FSBasic/FSCanvas.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSPhysics.h"
#include "FSBasic/FSCut.h"
#include "FSBasic/FSSystem.h"
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


    // make a list of modes

  vector<FSModeInfo*> modeVector  = FSModeCollection::modeVector(category);


    // check if we need to loop over modes

  bool loopOverModes = false;
  if (modeVector.size() > 0) loopOverModes = true;


    // loop over modes if we need to

  if (loopOverModes){
    for (unsigned int i = (iModeStart-1); i < modeVector.size() && i < iModeEnd; i++){
      TString ntName_i = modeVector[i]->modeString(ntName);
      TString fileNameInput_i = modeVector[i]->modeString(fileNameInput);
      TString fileNameOutput_i = modeVector[i]->modeString(fileNameOutput);
      TString cuts_i = modeVector[i]->modeString(cuts);
        vector< pair<TString,double> > fsCuts = FSCut::expandCuts(cuts_i);
        if (fsCuts.size() == 1){ cuts_i = modeVector[i]->modeString(fsCuts[0].first); }
        else{ cout << "FSModeTree::skimTree Error: multidimensional sidebands not allowed" << endl; exit(1); }
        vector<TString> modeCuts = modeVector[i]->modeCombinatorics(cuts_i);
        if (modeCuts.size() == 1){ cuts_i = modeVector[i]->modeString(modeCuts[0]); }
        else{ cout << "FSModeTree::skimTree Error: multiple combinations not allowed in cuts" << endl; 
              modeVector[i]->modeCombinatorics(cuts_i,true);  exit(1); }
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
  // CREATE A FRIEND TREE
  // ********************************************************

void
FSModeTree::createFriendTree(TString fileNameInput, TString treeNameInput, TString category,
                             TString friendName, TString friendVariable){
  vector< pair<TString,TString> > friendTreeContents;
  friendTreeContents.push_back(pair<TString,TString>(friendName,friendVariable));
  createFriendTree(fileNameInput,treeNameInput,category,friendName,friendTreeContents);
}


void
FSModeTree::createFriendTree(TString fileNameInput, TString treeNameInput, TString category,
                             TString friendName, vector< pair<TString,TString> > friendTreeContents){
  fileNameInput = FSString::removeWhiteSpace(fileNameInput);
  treeNameInput = FSString::removeWhiteSpace(treeNameInput);
  category      = FSString::removeWhiteSpace(category);
  friendName    = FSString::removeWhiteSpace(friendName);
  for (unsigned int i = 0; i < friendTreeContents.size(); i++){
    friendTreeContents[i].first = FSString::removeWhiteSpace(friendTreeContents[i].first);
    friendTreeContents[i].second = FSString::removeWhiteSpace(friendTreeContents[i].second);
  }

    // make a list of modes and check if a loop is necessary 
  vector<FSModeInfo*> modeVector  = FSModeCollection::modeVector(category);
  bool loopOverModes = false;
  if (modeVector.size() > 0) loopOverModes = true;

    // loop over modes
  if (loopOverModes){
    for (unsigned int i = 0; i < modeVector.size(); i++){
      TString fileNameInput_i = modeVector[i]->modeString(fileNameInput);
      TString treeNameInput_i = modeVector[i]->modeString(treeNameInput);
      TString friendName_i    = modeVector[i]->modeString(friendName);
      vector< pair<TString,TString> > friendTreeContents_i = friendTreeContents;
      for (unsigned int j = 0; j < friendTreeContents_i.size(); j++){
        friendTreeContents_i[j].first  = modeVector[i]->modeString(friendTreeContents_i[j].first);
        friendTreeContents_i[j].second = modeVector[i]->modeString(friendTreeContents_i[j].second);
        vector< pair<TString,double> > fsCuts = FSCut::expandCuts(friendTreeContents_i[j].second);
        if (fsCuts.size() == 1){ friendTreeContents_i[j].second = modeVector[i]->modeString(fsCuts[0].first); }
        else{ cout << "FSModeTree::createFriendTree ERROR: multidimensional sidebands not allowed" << endl; exit(1); }
        vector<TString> modeCombos = modeVector[i]->modeCombinatorics(friendTreeContents_i[j].second);
        if (modeCombos.size() == 1){ friendTreeContents_i[j].second = modeVector[i]->modeString(modeCombos[0]); }
        else{ cout << "FSModeTree::createFriendTree ERROR: multiple combinations not allowed here" << endl; 
              modeVector[i]->modeCombinatorics(friendTreeContents_i[j].second,true);  exit(1); }
      }
      FSTree::createFriendTree(fileNameInput_i,treeNameInput_i,friendName_i,friendTreeContents_i);
    }
  }

    // don't loop over modes
  if (!loopOverModes){
    FSTree::createFriendTree(fileNameInput,treeNameInput,friendName,friendTreeContents);
  }
}



  // ********************************************************
  // RANK EVENTS ACCORDING TO A GIVEN VARIABLE
  // ********************************************************


void
FSModeTree::createRankingTree(TString fileName, TString ntName, TString category, 
                              TString rankVarName, TString rankVar, TString cuts,
                              TString groupVar1, TString groupVar2, TString groupVar3, 
                              TString printCommandFile){

  fileName    = FSString::removeWhiteSpace(fileName);
  ntName      = FSString::removeWhiteSpace(ntName);
  if (ntName == "") ntName = FSTree::getTreeNameFromFile(fileName);
  category    = FSString::removeWhiteSpace(category);
  rankVarName = FSString::removeWhiteSpace(rankVarName);
  rankVar     = FSString::removeWhiteSpace(rankVar);
  cuts        = FSString::removeWhiteSpace(cuts);  if (cuts == "") cuts = "1==1";
  groupVar1   = FSString::removeWhiteSpace(groupVar1);
  groupVar2   = FSString::removeWhiteSpace(groupVar2);
  groupVar3   = FSString::removeWhiteSpace(groupVar3);
  if (groupVar2 == "") groupVar2 = groupVar1;
  if (groupVar3 == "") groupVar3 = groupVar2;

  if (!FSControl::QUIET){
    cout << "******************************************\n"
            "  BEGIN FSModeTree::createRankingTree\n"
            "******************************************" << endl;
    cout << "  fileName    = " << fileName << endl;
    cout << "  ntName      = " << ntName << endl;
    cout << "  category    = " << category << endl;
    cout << "  rankVarName = " << rankVarName << endl;
    cout << "  rankVar     = " << rankVar << endl;
    cout << "  cuts        = " << cuts << endl;
    cout << "  groupVar1   = " << groupVar1 << endl;
    cout << "  groupVar2   = " << groupVar2 << endl;
    cout << "  groupVar3   = " << groupVar3 << endl;
    cout << "******************************************" << endl;
  }


    // get a list of modes associated with this category

  vector<FSModeInfo*> modeVector = FSModeCollection::modeVector(category);
  if (modeVector.size() == 0){
    modeVector.push_back(new FSModeInfo(""));
    // cout << "FSModeTree::createRankingTree WARNING:  " <<
    // endl <<           "there are no modes associated with this category..." <<
    // endl << "                ... skipping createRankingTree" << endl;
    // return;
  }


  // just write the command to a file and return

  if (printCommandFile != ""){
    vector<TString> friendNames = FSTree::getFriendNames(1);
    TString friends("");
    for (unsigned int i = 0; i < friendNames.size(); i++){ 
      friends += " -friend \"" + friendNames[i] + "\"";
    }
    TString modes("");
    for (unsigned int i = 0; i < modeVector.size(); i++){ 
      modes += " -mode \"" + modeVector[i]->modeString() + "\"";
    }
    vector< pair<TString,double> > fsCuts = FSCut::expandCuts(cuts);
    if (fsCuts.size() == 1){ cuts = fsCuts[0].first; }
    else{ cout << "FSModeTree::createRankingTree Error: "
                  "multidimensional sidebands not allowed" << endl; exit(1); }
    FSString::writeTStringToFile(printCommandFile,
          "$FSROOT/Executables/FSModeCreateRankingTree "
          " -i \""      + fileName    + "\""
          " -nt \""     + ntName      + "\""
          " -rvname \"" + rankVarName + "\""
          " -rv \""     + rankVar     + "\""
          " -cuts \""   + cuts        + "\""
          " -gv1 \""    + groupVar1   + "\""
          " -gv2 \""    + groupVar2   + "\""
          " -gv3 \""    + groupVar3   + "\""
          + friends + modes,
          false);
    return;
  }


    // *** LOOP 1 *** 
    //           loop over all trees and record GROUPVAR1, GROUPVAR2, GROUPVAR3, RANKVAR, IMODE in
    //               map< tuple<GROUPVAR1,GROUPVAR2,GROUPVAR3>, map<IMODE, vector<RANKVAR> > >

    // *** LOOP 2 *** 
    //            loop over the trees again and create the friend trees
    //               using that map to rank entries

  map< tuple<int,int,int>, map<int, vector<int> > >  rankMap;

  for (unsigned int iLoop = 1; iLoop <= 2; iLoop++){
    TString sLoop = "FIRST"; if (iLoop == 2) sLoop = "SECOND";
    if (!FSControl::QUIET) cout << "\n---------- STARTING " << sLoop << " LOOP ----------\n" << endl;

      // SECOND LOOP: (FOR DEBUGGING ONLY) find the event with the most combinations

    int DEBUGRUN = 0;
    int DEBUGEVT = 0;
    int DEBUGMAXCOMBO = 0;
    if (iLoop == 2){
      for (map< tuple<int,int,int>, map<int, vector<int> > >::iterator mRankItr = rankMap.begin();
            mRankItr != rankMap.end(); ++mRankItr) {
        tuple<int,int,int> pRE = mRankItr->first;
        map<int, vector<int> > mMV = mRankItr->second;
        if ((mMV.find(0) != mMV.end())  && ((int)mMV[0].size() > DEBUGMAXCOMBO)){
          DEBUGMAXCOMBO = mMV[0].size();
          DEBUGRUN = get<0>(pRE);
          DEBUGEVT = get<1>(pRE);
        }
      }
    }

      // BOTH LOOPS: loop over all modes and files in this category 

    for (unsigned int i = 0; i < modeVector.size(); i++){
     vector<TString> fileNames_i = FSSystem::getAbsolutePaths(modeVector[i]->modeString(fileName));
     for (unsigned int ifile = 0; ifile < fileNames_i.size(); ifile++){
      int IMODE = i + 1;
      TString fileName_i  = fileNames_i[ifile];
      TString ntName_i    = modeVector[i]->modeString(ntName);
        if (ntName_i == "") ntName_i = FSTree::getTreeNameFromFile(fileName_i);
      TString cuts_i      = modeVector[i]->modeString(cuts);
        vector< pair<TString,double> > fsCuts = FSCut::expandCuts(cuts_i);
        if (fsCuts.size() == 1){ cuts_i = modeVector[i]->modeString(fsCuts[0].first); }
        else{ cout << "FSModeTree::createRankingTree Error: "
                      "multidimensional sidebands not allowed" << endl; exit(1); }
        vector<TString> modeCuts = modeVector[i]->modeCombinatorics(cuts_i);
        if (modeCuts.size() == 1){ cuts_i = modeVector[i]->modeString(modeCuts[0]); }
        else{ cout << "FSModeTree::createRankingTree Error: "
                      "multiple combinations not allowed in cuts" << endl; 
                       modeVector[i]->modeCombinatorics(cuts_i,true);  exit(1); }
      TString rankVar_i   = modeVector[i]->modeString(rankVar);
      TString groupVar1_i = modeVector[i]->modeString(groupVar1);
      TString groupVar2_i = modeVector[i]->modeString(groupVar2);
      TString groupVar3_i = modeVector[i]->modeString(groupVar3);
        vector<TString> modeTmp0 = modeVector[i]->modeCombinatorics(rankVar_i);
        vector<TString> modeTmp1 = modeVector[i]->modeCombinatorics(groupVar1_i);
        vector<TString> modeTmp2 = modeVector[i]->modeCombinatorics(groupVar2_i);
        vector<TString> modeTmp3 = modeVector[i]->modeCombinatorics(groupVar3_i);
        if (modeTmp0.size() == 1 && modeTmp1.size() == 1 && modeTmp2.size() == 1 && modeTmp3.size() == 1){
          rankVar_i   = modeVector[i]->modeString(modeTmp0[0]);
          groupVar1_i = modeVector[i]->modeString(modeTmp1[0]);
          groupVar2_i = modeVector[i]->modeString(modeTmp2[0]);
          groupVar3_i = modeVector[i]->modeString(modeTmp3[0]); }
        else{ cout << "FSModeTree::createRankingTree Error: "
                      "multiple combinations not allowed in rankVar and groupVars" << endl; 
                       modeVector[i]->modeCombinatorics(rankVar_i,  true);
                       modeVector[i]->modeCombinatorics(groupVar1_i,true);
                       modeVector[i]->modeCombinatorics(groupVar2_i,true);
                       modeVector[i]->modeCombinatorics(groupVar3_i,true);  exit(1); }
      if (ifile == 0 && !FSControl::QUIET){
        cout << "---------- " << sLoop << " LOOP OVER " << modeVector[i]->modeDescription() 
             << " (" << fileNames_i.size() << " FILE(S)) ----------" << endl;
        if (FSControl::DEBUG){
          cout << "  INFO FOR THE FIRST FILE: " << endl;
          cout << "        fileName  = " << fileName_i << endl;
          cout << "        ntName    = " << ntName_i << endl;
          cout << "        variables before expanding:" << endl;
          cout << "          cuts      = " << cuts_i << endl;
          cout << "          rankVar   = " << rankVar_i << endl;
          cout << "          groupVar1 = " << groupVar1_i << endl;
          cout << "          groupVar2 = " << groupVar2_i << endl;
          cout << "          groupVar3 = " << groupVar3_i << endl;
        }
      }
      cuts_i      = FSTree::expandVariable(cuts_i);
      rankVar_i   = FSTree::expandVariable(rankVar_i);
      groupVar1_i = FSTree::expandVariable(groupVar1_i);
      groupVar2_i = FSTree::expandVariable(groupVar2_i);
      groupVar3_i = FSTree::expandVariable(groupVar3_i);
      if (ifile == 0 && FSControl::DEBUG){
        cout << "        variables after expanding:" << endl;
        cout << "          cuts      = " << cuts_i << endl;
        cout << "          rankVar   = " << rankVar_i << endl;
        cout << "          groupVar1 = " << groupVar1_i << endl;
        cout << "          groupVar2 = " << groupVar2_i << endl;
        cout << "          groupVar3 = " << groupVar3_i << endl << endl;
      }

        // BOTH LOOPS: prepare to read from the original tree for this mode

      TString treeSTATUS;
      TTree* nt = FSTree::getTChain(fileName_i,ntName_i,treeSTATUS);
      if (!nt || treeSTATUS.Contains("!!")){
        cout << "FSModeTree::createRankingTree WARNING:  " <<
        endl <<           "trouble creating TChain  (status = " << treeSTATUS << ")" << endl;
        continue;
      }
      Double_t GROUPVAR1, GROUPVAR2, GROUPVAR3, RANKVAR;
      int NSKIPPED = 0;
      if (cuts_i == "") cuts_i = "(1==1)";
      TObjArray* formulas = new TObjArray();
      TTreeFormula* cutsF = new TTreeFormula("cutsF", cuts_i,      nt); formulas->Add(cutsF);
      TTreeFormula* rvarF = new TTreeFormula("rvarF", rankVar_i,   nt); formulas->Add(rvarF);
      TTreeFormula* grp1F = new TTreeFormula("grp1F", groupVar1_i, nt); formulas->Add(grp1F);
      TTreeFormula* grp2F = new TTreeFormula("grp2F", groupVar2_i, nt); formulas->Add(grp2F);
      TTreeFormula* grp3F = new TTreeFormula("grp3F", groupVar3_i, nt); formulas->Add(grp3F);
      nt->SetNotify(formulas);

        // SECOND LOOP: create friend trees

      TFile* rankTFile = NULL;
      TTree* rankTTree = NULL;
      Int_t VARRANK;
      Int_t VARRANKGLOBAL;
      Int_t NCOMBINATIONS;
      Int_t NCOMBINATIONSGLOBAL;
      Int_t VARRANKVAR;
      Int_t RANKVARBEST = -1;
      Int_t RANKVARBESTOTHER = -1;
      TString sVARRANK             = rankVarName + "";
      TString sVARRANKGLOBAL       = rankVarName + "Global";
      TString sNCOMBINATIONS       = rankVarName + "Combinations";
      TString sNCOMBINATIONSGLOBAL = rankVarName + "CombinationsGlobal";
      TString sVARRANKVAR          = rankVarName + "Var";
      TString sRANKVARBEST         = rankVarName + "VarBest";
      TString sRANKVARBESTOTHER    = rankVarName + "VarBestOther";
      TString fileName_rank(fileName_i);  fileName_rank += ".";  fileName_rank += rankVarName;
      TString ntName_rank(ntName_i);  ntName_rank += "_";  ntName_rank += rankVarName;
      if (iLoop == 2){
        rankTFile = new TFile(fileName_rank,"recreate");
        if (!rankTFile || rankTFile->IsZombie()) {
          cout << "FSModeTree::createRankingTree Error: could not create ranking file " << fileName_rank << endl; exit(1);
        }
        rankTFile->cd();
        rankTTree = new TTree(ntName_rank, ntName_rank);
        rankTTree->Branch(sVARRANK,            &VARRANK,            sVARRANK+"/I");
        rankTTree->Branch(sVARRANKGLOBAL,      &VARRANKGLOBAL,      sVARRANKGLOBAL+"/I");
        rankTTree->Branch(sNCOMBINATIONS,      &NCOMBINATIONS,      sNCOMBINATIONS+"/I");
        rankTTree->Branch(sNCOMBINATIONSGLOBAL,&NCOMBINATIONSGLOBAL,sNCOMBINATIONSGLOBAL+"/I");
        rankTTree->Branch(sVARRANKVAR,         &VARRANKVAR,         sVARRANKVAR+"/I");
        rankTTree->Branch(sRANKVARBEST,        &RANKVARBEST,        sRANKVARBEST+"/I");
        rankTTree->Branch(sRANKVARBESTOTHER,   &RANKVARBESTOTHER,   sRANKVARBESTOTHER+"/I");
      }

        // BOTH LOOPS: loop over events in the original tree

      unsigned int nEvents = nt->GetEntries();
      if (!FSControl::QUIET){
        cout << " LOOP OVER ORIGINAL TREE WITH " 
             << FSString::int2TString(nEvents,0,true) << " ENTRIES" << endl;
        cout << "  File = " << fileName_i << endl;
      }
      for (unsigned int ientry = 0; ientry < nEvents; ientry++){
        if (ientry > 0 && ientry % 100000 == 0)
          cout << "\tentry = " << FSString::int2TString(ientry,0,true) << endl;
        nt->GetEntry(ientry);
        bool SKIPENTRY = !cutsF->EvalInstance();
        GROUPVAR1 = grp1F->EvalInstance();    
        GROUPVAR2 = grp2F->EvalInstance();    
        GROUPVAR3 = grp3F->EvalInstance();    
        RANKVAR   = rvarF->EvalInstance();    
        int IRANKVAR = (int)RANKVAR; 
        tuple<int,int,int> pRunEvent = tuple<int,int,int>((int)GROUPVAR1,(int)GROUPVAR2,(int)GROUPVAR3);
        map<int, vector<int> > mModeVar;
        if (rankMap.find(pRunEvent) != rankMap.end()) mModeVar = rankMap[pRunEvent];
        vector<int> vVarI;  vector<int> vVar0;
        if (mModeVar.find(IMODE) != mModeVar.end()) vVarI = mModeVar[IMODE];
        if (mModeVar.find(0)     != mModeVar.end()) vVar0 = mModeVar[0];
        if (FSControl::DEBUG && ifile == 0 && ientry < 3){
          if (ientry == 0)
            cout << "  INFO FOR THE FIRST THREE ENTRIES " << endl;
          if (ientry < 3){
            cout << "    entry = " << ientry << endl;
            cout << "      GROUPVAR1 = " << GROUPVAR1 << endl;
            cout << "      GROUPVAR2 = " << GROUPVAR2 << endl;
            cout << "      GROUPVAR3 = " << GROUPVAR3 << endl;
            cout << "      RANKVAR   = " << RANKVAR << endl;
            cout << "      IRANKVAR  = " << IRANKVAR << endl;
            cout << "      SKIPENTRY = " << SKIPENTRY << endl;
          }
        }

          // FIRST LOOP: just record information

        if (iLoop == 1 && !SKIPENTRY){
          if (vVarI.size() == 0) vVarI.push_back(IRANKVAR);
          if (vVar0.size() == 0) vVar0.push_back(IRANKVAR);
          vVarI.push_back(IRANKVAR);
          vVar0.push_back(IRANKVAR);
          std::sort(vVarI.begin()+1, vVarI.end());
          std::sort(vVar0.begin()+1, vVar0.end());
          vVarI[0] = vVarI[1];
          vVar0[0] = vVar0[1];
          mModeVar[IMODE] = vVarI;
          mModeVar[0]     = vVar0;
          rankMap[pRunEvent] = mModeVar;
        }

          // SECOND LOOP: fill the friend tree

        if (iLoop == 2){
          if (SKIPENTRY) NSKIPPED++;
          if (FSControl::DEBUG){
            if (DEBUGRUN == get<0>(pRunEvent) && DEBUGEVT == get<1>(pRunEvent)){
              cout << "DEBUG INFO FOR THE EVENT WITH THE MOST COMBINATIONS ("
                   << DEBUGRUN << "," << DEBUGEVT << ")" << endl;
              cout << "IRANKVAR = " << IRANKVAR << endl;
              cout << "channel list = ";
              for (unsigned int vvv = 0; vvv < vVarI.size(); vvv++){ cout << vVarI[vvv] << " "; }
              cout << endl;
            }
          }
          VARRANKVAR = IRANKVAR;
          RANKVARBEST = -1;  if (vVarI.size() > 0) RANKVARBEST = vVarI[0];
          NCOMBINATIONS = 0; if (vVarI.size() > 0) NCOMBINATIONS = vVarI.size()-1;
          VARRANK = -1;
          if (SKIPENTRY || vVarI.size() <= 1){ VARRANK = -1; }
          else if (vVarI.size() == 2){ VARRANK = 1; }
          else if (vVarI.size() > 2){
            for (unsigned int ix = 1; ix < vVarI.size(); ix++){
              if (IRANKVAR == vVarI[ix]){
                VARRANK = ix;
                vVarI[ix] = -10000000;
                mModeVar[IMODE] = vVarI;
                rankMap[pRunEvent] = mModeVar;
                break;
              }
            }
          }
          if (FSControl::DEBUG){
            if (DEBUGRUN == get<0>(pRunEvent) && DEBUGEVT == get<1>(pRunEvent)){
              cout << sNCOMBINATIONS       << "       = "             << NCOMBINATIONS << endl;
              cout << sVARRANK             << "                   = " << VARRANK << endl;
              cout << "global list = ";
              for (unsigned int vvv = 0; vvv < vVar0.size(); vvv++){ cout << vVar0[vvv] << " "; }
              cout << endl;
            }
          }
          NCOMBINATIONSGLOBAL = 0; if (vVar0.size() > 0) NCOMBINATIONSGLOBAL = vVar0.size()-1;
          VARRANKGLOBAL = -1;
          if (SKIPENTRY || vVar0.size() <= 1){ VARRANKGLOBAL = -1; }
          else if (vVar0.size() == 2){ VARRANKGLOBAL = 1; }
          else if (vVar0.size() > 2){
            for (unsigned int ix = 1; ix < vVar0.size(); ix++){
              if (IRANKVAR == vVar0[ix]){
                VARRANKGLOBAL = ix;
                vVar0[ix] = -10000000;
                mModeVar[0] = vVar0;
                rankMap[pRunEvent] = mModeVar;
                break;
              }
            }
          }
          RANKVARBESTOTHER = -1;
          vector<int> vHelp;
          for (int iHelp = 1; iHelp < 1+(int)modeVector.size(); iHelp++){
            if (IMODE != iHelp && mModeVar.find(iHelp) != mModeVar.end() && mModeVar[iHelp].size() > 0) 
              vHelp.push_back(mModeVar[iHelp][0]); }
          if (vHelp.size() > 0){ std::sort(vHelp.begin(), vHelp.end()); RANKVARBESTOTHER = vHelp[0]; }
          if (FSControl::DEBUG){
            if (DEBUGRUN == get<0>(pRunEvent) && DEBUGEVT == get<1>(pRunEvent)){
              cout << sNCOMBINATIONSGLOBAL << " = "                   << NCOMBINATIONSGLOBAL << endl;
              cout << sVARRANKGLOBAL       << "             = "       << VARRANKGLOBAL << endl;
            }
          }
          rankTTree->Fill();
        }

      }

        // SECOND LOOP: write out the friend tree

      if (iLoop == 2){
        rankTFile->cd();
        rankTTree->Write();
        delete rankTFile;
        //delete rankTTree; (seg fault)
        if (!FSControl::QUIET && ifile == fileNames_i.size()-1){
          cout << "******************************************\n"
                  "  END OF FILE FSModeTree::createRankingTree\n"
                  "******************************************" << endl;
          cout << "  new fileName      = " << fileName_rank << endl;
          cout << "  new ntName        = " << ntName_rank << endl;
          cout << "  most combos       = " << DEBUGMAXCOMBO << endl;
          cout << "  skipped events    = " << NSKIPPED << endl;
          if (nEvents > 0)
          cout << "  fraction skipped  = " << (100.0*NSKIPPED)/nEvents << " percent" << endl;
          cout << "******************************************" << endl;
        }
      }

        // BOTH LOOPS: clean up the TTreeFormula objects

      if (cutsF) delete cutsF;
      if (rvarF) delete rvarF;
      if (grp1F) delete grp1F;
      if (grp2F) delete grp2F;
      if (grp3F) delete grp3F;
      nt->SetNotify(nullptr);
      delete formulas;

     }
    }
  }
}




void
FSModeTree::createPHSPTree(double EnPCM, double PxPCM, double PyPCM, double PzPCM, 
                               TString modeString, int nEvents, 
                               TString fileNameOutput,  TString ntName){
  FSModeInfo modeInfo(modeString);
  fileNameOutput = modeInfo.modeString(fileNameOutput);
  ntName = modeInfo.modeString(ntName);
  vector<TString> vParticles = modeInfo.particles();
  vector<double> vMasses = modeInfo.modeMasses();
  cout << endl;
  cout << "----------------" << endl;
  cout << "MAKING PHSP TREE" << endl;
  cout << "----------------" << endl;
  cout << "  FINAL STATE:   " << modeInfo.modeDescription() << endl;
  cout << "    FILE NAME:   " << fileNameOutput << endl;
  cout << "    TREE NAME:   " << ntName << endl;
  cout << "     BRANCHES: " << endl;
  cout << "           PxPCM PyPCM PzPCM EnPCM:  four-vector of the CM system" << endl;
  for (unsigned int i = 0; i < vParticles.size(); i++){
    TString si = FSString::int2TString(i+1);
    cout << "           PxP"+si+"  PyP"+si+"  PzP"+si+"  EnP"+si+":   "
            "four-vector of " << vParticles[i] << endl;
    if ((vParticles[i] == "pi0") || (vParticles[i] == "eta") 
       || (vParticles[i] == "Ks") || (vParticles[i].Contains("Lambda"))){
      cout << "           PxP"+si+"a PyP"+si+"a PzP"+si+"a EnP"+si+"a:  "
            "four-vector of 1st " << vParticles[i] << " daughter" << endl;
      cout << "           PxP"+si+"b PyP"+si+"b PzP"+si+"b EnP"+si+"b:  "
            "four-vector of 2nd " << vParticles[i] << " daughter" << endl; } }
  cout << "----------------" << endl;
  cout << "Creating " << nEvents << " events..." << endl;
  TLorentzVector pParent(PxPCM, PyPCM, PzPCM, EnPCM);
  double masses[100];
  double PxPN[100]; double PxPNa[100]; double PxPNb[100];
  double PyPN[100]; double PyPNa[100]; double PyPNb[100];
  double PzPN[100]; double PzPNa[100]; double PzPNb[100];
  double EnPN[100]; double EnPNa[100]; double EnPNb[100];
  TLorentzVector* pPN[100]; TLorentzVector* pPNa[100]; TLorentzVector* pPNb[100];
  TFile file(fileNameOutput,"recreate");
  TTree tree(ntName,ntName);
  tree.Branch("PxPCM",  &PxPCM,    "PxPCM/D");
  tree.Branch("PyPCM",  &PyPCM,    "PyPCM/D");
  tree.Branch("PzPCM",  &PzPCM,    "PzPCM/D");
  tree.Branch("EnPCM",  &EnPCM,    "EnPCM/D");
  for (unsigned int i = 0; i < vMasses.size(); i++){
    masses[i] = vMasses[i];
    TString si = FSString::int2TString(i+1);
    tree.Branch("PxP"+si,   &PxPN[i],     "PxP"+si+"/D");
    tree.Branch("PyP"+si,   &PyPN[i],     "PyP"+si+"/D");
    tree.Branch("PzP"+si,   &PzPN[i],     "PzP"+si+"/D");
    tree.Branch("EnP"+si,   &EnPN[i],     "EnP"+si+"/D");
    if ((vParticles[i] == "pi0") || (vParticles[i] == "eta") 
       || (vParticles[i] == "Ks") || (vParticles[i].Contains("Lambda"))){
      tree.Branch("PxP"+si+"a",   &PxPNa[i],     "PxP"+si+"a/D");
      tree.Branch("PyP"+si+"a",   &PyPNa[i],     "PyP"+si+"a/D");
      tree.Branch("PzP"+si+"a",   &PzPNa[i],     "PzP"+si+"a/D");
      tree.Branch("EnP"+si+"a",   &EnPNa[i],     "EnP"+si+"a/D");
      tree.Branch("PxP"+si+"b",   &PxPNb[i],     "PxP"+si+"b/D");
      tree.Branch("PyP"+si+"b",   &PyPNb[i],     "PyP"+si+"b/D");
      tree.Branch("PzP"+si+"b",   &PzPNb[i],     "PzP"+si+"b/D");
      tree.Branch("EnP"+si+"b",   &EnPNb[i],     "EnP"+si+"b/D");
    }
  }
  TGenPhaseSpace generator;    TGenPhaseSpace generator2;
  generator.SetDecay(pParent, vMasses.size(), masses);
  double maxWt = generator.GetWtMax();
  int count = 0;
  while (count < nEvents){
    while (generator.Generate() < gRandom->Uniform() * maxWt){}
    for (unsigned int i = 0; i < vMasses.size(); i++){
      pPN[i] = generator.GetDecay(i);
      PxPN[i] = pPN[i]->Px();  PyPN[i] = pPN[i]->Py();  PzPN[i] = pPN[i]->Pz();  EnPN[i] = pPN[i]->E();
      if ((vParticles[i] == "pi0") || (vParticles[i] == "eta") 
         || (vParticles[i] == "Ks") || (vParticles[i].Contains("Lambda"))){
        double tmpmass[2];
        if (vParticles[i] == "pi0")           { tmpmass[0] = 0.0;             tmpmass[1] = 0.0; }
        if (vParticles[i] == "eta")           { tmpmass[0] = 0.0;             tmpmass[1] = 0.0; }
        if (vParticles[i] == "Ks")            { tmpmass[0] = FSPhysics::XMpi; tmpmass[1] = FSPhysics::XMpi; }
        if (vParticles[i].Contains("Lambda")) { tmpmass[0] = FSPhysics::XMp;  tmpmass[1] = FSPhysics::XMpi; }
        generator2.SetDecay(*pPN[i], 2, tmpmass);
        double maxWt2 = generator2.GetWtMax(); while (generator2.Generate() < gRandom->Uniform() * maxWt2){}
        pPNa[i] = generator2.GetDecay(0);
        pPNb[i] = generator2.GetDecay(1);
        PxPNa[i] = pPNa[i]->Px();  PyPNa[i] = pPNa[i]->Py();  PzPNa[i] = pPNa[i]->Pz();  EnPNa[i] = pPNa[i]->E();
        PxPNb[i] = pPNb[i]->Px();  PyPNb[i] = pPNb[i]->Py();  PzPNb[i] = pPNb[i]->Pz();  EnPNb[i] = pPNb[i]->E();
      }
    }
    count++;
    tree.Fill();
  }
  tree.Write();
  file.Close();
}




