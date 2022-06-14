#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "TChain.h"
#include "TChainElement.h"
#include "TObjArray.h"
#include "TString.h"
#include "TKey.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TFriendElement.h"
#include "FSBasic/FSControl.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSSystem.h"
#include "FSBasic/FSPhysics.h"
#include "FSBasic/FSCut.h"
#include "FSBasic/FSTree.h"


  // static member data


map< TString, TChain*> FSTree::m_chainCache;
vector< pair<TString,bool> > FSTree::m_friendTrees;

map< TString, TString > FSTree::m_mapDefinedPx;
map< TString, TString > FSTree::m_mapDefinedPy;
map< TString, TString > FSTree::m_mapDefinedPz;
map< TString, TString > FSTree::m_mapDefinedEn;
map< pair<TString,int>, TString > FSTree::m_mapDefinedMacros;
vector< TString > FSTree::m_vectorDefinedMacroKeywords;
bool FSTree::m_madeStandardDefinitions = false;
bool FSTree::m_useFriendTrees = true;


  // ********************************************************
  // GET A TCHAIN FROM THE CACHE OR CREATE A NEW ONE
  // ********************************************************

TChain*
FSTree::getTChain(TString fileName, TString ntName){
  TString STATUS;
  return getTChain(fileName,ntName,STATUS);
}

TChain*
FSTree::getTChain(TString fileName, TString ntName, TString& STATUS){
  fileName = FSString::removeWhiteSpace(fileName);
  ntName   = FSString::removeWhiteSpace(ntName);
  TChain* nt = NULL;  STATUS = "OKAY";

    // clear the chain cache if there is no chain caching

  if (!FSControl::CHAINCACHING) clearChainCache();

    // create an index for this chain and search for it

  TString index(fileName);  index += ":";  index += ntName;
  map<TString,TChain*>::const_iterator mapItr = m_chainCache.find(index);
  if (mapItr != m_chainCache.end()){
    if (FSControl::DEBUG) 
      cout << "FSTree: found TChain with index " << index << endl;
    nt = m_chainCache[index];
  }

    // create the chain from scratch if not found

  else{
    if (FSControl::DEBUG) 
      cout << "FSTree: creating new TChain with index " << index << endl;
    nt = new TChain(ntName);
    vector<TString> fileList = FSSystem::getAbsolutePaths(fileName,true,FSControl::DEBUG);
    int nFiles = 0;
    for (unsigned int i = 0; i < fileList.size(); i++){
      if (!FSSystem::checkRootFormat(fileList[i])) continue;
      TFile file(fileList[i]);
      TTree* tree = (TTree*)file.Get(ntName);
      if ((tree) && (tree->GetEntries() > 0) && (tree->GetNbranches() > 0)){
        if (FSControl::DEBUG) 
          cout << "FSTree: adding file to TChain " << fileList[i] << endl;
        nt->Add(fileList[i]);  nFiles++;
      }
    }
    if (nFiles == 0){
      STATUS = "!!NO_FILE!!";
      if (FSControl::DEBUG) 
        cout << "FSTree: null TChain with index " << index << endl;
      delete nt;
      nt = NULL;
    }
    else if ((nt->GetEntries() == 0) || (nt->GetNbranches() == 0)){
      STATUS = "!!NO_TREE!!";
      if (FSControl::DEBUG) 
        cout << "FSTree: null TChain with index " << index << endl;
      delete nt;
      nt = NULL;
    }
    else{
      m_chainCache[index] = nt;
    }
  }

    // add friends to the tree

  if (m_useFriendTrees){
    bool saveCHAINCACHING = FSControl::CHAINCACHING;
    FSControl::CHAINCACHING = true;
    useFriendTrees(false);
    for (unsigned int i = 0; i < m_friendTrees.size(); i++){
      if (m_friendTrees[i].second){
        TString friendFileName(fileName);
        while (friendFileName.Contains(",")){
          friendFileName.Replace(friendFileName.Index(","),1,"!!!"); }
        while (friendFileName.Contains("!!!")){
          friendFileName.Replace(friendFileName.Index("!!!"),3,"."+m_friendTrees[i].first+","); }
        friendFileName += ".";
        friendFileName += m_friendTrees[i].first;
        TString friendNTName(ntName);
        friendNTName += "_";
        friendNTName += m_friendTrees[i].first;
        TChain* ntFriend = getTChain(friendFileName,friendNTName);
        if (!ntFriend) continue;
        if (nt->GetEntries() != ntFriend->GetEntries()){
          cout << "FSTree::getTChain ERROR:  problem with friend tree " 
               << m_friendTrees[i].first << endl;
          cout << "  friend file name = \n     " << friendFileName << endl;  
          cout << "  friend tree name = " << friendNTName << endl;  
          cout << "  entries in original tree = " << nt->GetEntries() << endl;
          cout << "  entries in friend tree   = " << ntFriend->GetEntries() << endl;
          exit(0);
        }
        if (FSControl::DEBUG){
          cout << "FSTree: adding friend tree... " << endl;
          cout << "            friend tree name = " << friendNTName << endl;
          cout << "            friend tree file = " << friendFileName << endl;
        }
        nt->AddFriend(ntFriend);
      }
    }
    FSControl::CHAINCACHING = saveCHAINCACHING;
    useFriendTrees(true);
  }

  return nt;

}



  // *********************************************************
  // ADD OR REMOVE FRIEND TREES FROM A LIST OF FRIEND TREES
  // *********************************************************


void
FSTree::addFriendTree(TString friendName, bool use){
  friendName = FSString::removeWhiteSpace(friendName);
  if (friendName == "") return;
  for (unsigned int i = 0; i < m_friendTrees.size(); i++){ 
    if (m_friendTrees[i].first == friendName){
      m_friendTrees[i].second = use;
      return;
    }
  }
  m_friendTrees.push_back(pair<TString,bool>(friendName,use));
}

void
FSTree::removeFriendTree(TString friendName){
  friendName = FSString::removeWhiteSpace(friendName);
  if (friendName == "") return;
  vector< pair<TString,bool> > newFriendTrees;
  for (unsigned int i = 0; i < m_friendTrees.size(); i++){
    if (!FSString::compareTStrings(m_friendTrees[i].first,friendName))
      newFriendTrees.push_back(m_friendTrees[i]);
  }
  if (m_friendTrees.size() != newFriendTrees.size()){ 
    m_friendTrees.clear(); 
    m_friendTrees = newFriendTrees;
  } 
}

void
FSTree::useFriendTree(TString friendName, bool use){
  friendName = FSString::removeWhiteSpace(friendName);
  if (friendName == "") return;
  for (unsigned int i = 0; i < m_friendTrees.size(); i++){
    if (FSString::compareTStrings(m_friendTrees[i].first,friendName))
      m_friendTrees[i].second = use;
  }
}

vector<TString>
FSTree::getFriendNames(int used){ // (-1,0,1) = (unused,all,used)
  vector<TString> names;
  for (unsigned int i = 0; i < m_friendTrees.size(); i++){
    if (used == 1 && m_friendTrees[i].second && m_useFriendTrees)
      names.push_back(m_friendTrees[i].first);
    if (used == -1 && (!m_friendTrees[i].second || !m_useFriendTrees))
      names.push_back(m_friendTrees[i].first);
    if (used == 0)
      names.push_back(m_friendTrees[i].first);
  }
  return names;
}


void
FSTree::showFriendTrees(){
  cout << "FRIEND TREES:" << endl;
  for (unsigned int i = 0; i < m_friendTrees.size(); i++){
    cout << "  (" << (i+1) << ")  " << m_friendTrees[i].first << endl;
    if (m_friendTrees[i].second && m_useFriendTrees){
    cout << "        USED" << endl; }
    else{
    cout << "        NOT USED" << endl; }
    cout << "          treeName = treeName_" << m_friendTrees[i].first << endl;
    cout << "          fileName = fileName." << m_friendTrees[i].first << endl;
  }
}


  // ********************************************************
  // GET NAMES OF OBJECTS FROM FILES AND BRANCH NAMES
  // ********************************************************

TString
FSTree::getTreeNameFromFile(TString fileName, TString match){
  vector<TString> treeNames = getTObjNamesFromFile(fileName, "TTree", match);
  if (treeNames.size() > 0) return treeNames[0];
  return TString("");
}


vector<TString>
FSTree::getTObjNamesFromFile(TString fileName, TString objType, TString match, bool show){
  vector<TString> objNames;
  vector<TString> fileNames = FSSystem::getAbsolutePaths(fileName, false);
  if (fileNames.size() == 0) return objNames;
  fileName = fileNames[0];
  if (!FSSystem::checkRootFormat(fileName,false)) return objNames;
  TFile* inFile = new TFile(fileName);
  if (!inFile) return objNames;
  TIter nextkey(inFile->GetListOfKeys());
  while (TKey* key = (TKey*)nextkey() ){
    TObject* obj = key->ReadObj();
    if (obj->IsA()->InheritsFrom(objType)){
      TString objName = obj->GetName();
      if (match == "" || FSString::compareTStrings(objName,match))
        objNames.push_back(objName);
    }
  }
  inFile->Close();
  delete inFile;
  if (show){
    TString objTYPE = objType; objTYPE.ToUpper();
    cout << "**********************************" << endl;
    cout << objTYPE << " FROM FILE: " << fileName << endl;
    cout << "**********************************" << endl;
    for (unsigned int i = 0; i < objNames.size(); i++){
      cout << "(" << (i+1) << ")  " << objNames[i] << endl; }
    cout << "**********************************" << endl;
  }
  return objNames;
}

vector<TString>
FSTree::getBranchNamesFromTree(TString fileName, TString ntName, TString match, bool show){
  vector<TString> branches;
  TString STATUS;
  TChain* nt = getTChain(fileName,ntName,STATUS);
  if (STATUS.Contains("!!") || (!nt)){ 
    cout << "FSTree::getBranchesFromTree WARNING: problem finding tree: " << STATUS << endl;
    return branches;
  }
  TObjArray* branchArray = nt->GetListOfBranches();
  int branchSize = nt->GetNbranches();
  for (int i = 0; i < branchSize; i++){
    TString branch = branchArray->At(i)->GetName();
    if (match == "" || FSString::compareTStrings(branch,match)) branches.push_back(branch);
  }
  if (show){
    cout << "LIST OF BRANCHES:" << endl;
    for (unsigned int i = 0; i < branches.size(); i++){
      cout << "  " << branches[i] << endl;
    }
  }
  return branches;
}


  // ********************************************************
  // SKIM A TREE AND OUTPUT TO A DIFFERENT FILE
  // ********************************************************

void
FSTree::skimTree(TString fileNameInput, TString chainName, 
                 TString fileNameOutput, TString cuts, 
                 TString newChainName, TString printCommandFile){
  fileNameInput = FSString::removeWhiteSpace(fileNameInput);
  chainName = FSString::removeWhiteSpace(chainName);
  fileNameOutput = FSString::removeWhiteSpace(fileNameOutput);
  cuts = FSString::removeWhiteSpace(cuts);
  newChainName = FSString::removeWhiteSpace(newChainName);
  printCommandFile = FSString::removeWhiteSpace(printCommandFile);
  if (chainName == "") chainName = getTreeNameFromFile(fileNameInput);
  int maxEntries = -1;
  if (cuts.Contains("MAXENTRIES=")){
    pair<int,TString> pairMaxEntries = FSTree::processMaxEntries(cuts);
    maxEntries = pairMaxEntries.first;
    cuts = pairMaxEntries.second;
  }

    // expand "cuts" using FSCut and check for multidimensional sidebands

  vector< pair<TString,double> > fsCuts = FSCut::expandCuts(cuts);
  if (fsCuts.size() == 1){ cuts = fsCuts[0].first; }
  else{ cout << "FSTree::skimTree ERROR: multidimensional sidebands not allowed" << endl; return; }

  // just write the command to a file and return

  if (printCommandFile != ""){
    vector<TString> friendNames = FSTree::getFriendNames(1);
    TString friends("");
    for (unsigned int i = 0; i < friendNames.size(); i++){ 
      friends += " -friend \"" + friendNames[i] + "\"";
    }
    FSString::writeTStringToFile(printCommandFile,
          "$FSROOT/Executables/FSSkimTree "
          " -i \""    + fileNameInput  + "\""
          " -o \""    + fileNameOutput + "\""
          " -cuts \"" + cuts           + "\""
          " -nt \""   + chainName      + "\""
          + friends,
          false);
    return;
  }

  // create an output file first (further set up later)

  TFile* file2 = new TFile(fileNameOutput,"recreate");

  // retrieve tree 1

  TChain* nt = getTChain(fileNameInput,chainName);

  // expand variable macros

  TString newCuts = expandVariable(cuts);

  // write out message

  if (FSControl::QUIET){
    cout << "creating " << fileNameOutput << endl;
  }
  if (!FSControl::QUIET){
    cout << endl;
    cout << "Copying File:" << endl;
    cout << "\t" << fileNameInput << " (tree = " << chainName << ")" << endl;
    cout << "To File:" << endl;
    cout << "\t" << fileNameOutput << endl;
    cout << endl;
    cout << "Original selection criteria:" << endl;
    cout << "\t" << cuts << endl;
    cout << endl;
    cout << "Modified selection criteria:" << endl;
    cout << "\t" << newCuts << endl;
    cout << endl;
    if (nt){
      cout << "Number of entries to skim:" << endl;
      cout << "\t" << FSString::int2TString(nt->GetEntries(),0,true) << endl;
      cout << endl;
    }
  }

  // check if there are entries to skim

  if ((!nt) || (nt->GetEntries() == 0) || (nt->GetNbranches() == 0)){
    cout << "Could not find any entries...  skipping..." << endl;
    return;
  }


  // set up the output file

  file2->cd();
  //vector<TString> dirs = FSString::parseTString(chainName,"/");
  //if (dirs.size() > 2){
  //  for (unsigned int i = 0; i < dirs.size()-1; i++){
  //    file2->mkdir(dirs[i]);
  //    file2->cd(dirs[i]);
  //  }
  //}


  // copy the tree with selection criteria

  TTree* tree2 = NULL;
  if (maxEntries < 0)  tree2 = nt->CopyTree(newCuts);
  if (maxEntries >= 0) tree2 = nt->CopyTree(newCuts,"",maxEntries);
  if (!tree2){
    cout << "Could not copy the tree....  skipping..." << endl;
    return;
  }
  if (newChainName != ""){ tree2->SetName(newChainName); tree2->SetTitle(newChainName); }
  if (!FSControl::QUIET){
    cout << "Number of entries kept:" << endl;
    cout << "\t" << FSString::int2TString(tree2->GetEntries(),0,true) << endl;
    cout << endl;
    cout << "Skim Ratio:" << endl;
    cout << "\t" << (float) tree2->GetEntries() / (float) nt->GetEntries() << endl;
    cout << endl;
  }


  // write the tree to file2

  if (tree2->GetListOfFriends()) tree2->GetListOfFriends()->Clear();
  file2->Write();
  file2->Close();

  clearChainCache();
  delete file2;


  // ***** also skim friend trees *****

  if (FSTree::getFriendNames(1).size() == 0) return;


  // 1. find the friend trees

  nt = getTChain(fileNameInput,chainName);
  vector<TTree*> friendTrees;
  TIter next(nt->GetListOfFriends());
  while (TFriendElement *obj = (TFriendElement*) next()){
    friendTrees.push_back(obj->GetTree());
  }

  // 2. add friends to each friend so they can be used separately
  //     and still access all branches

  for (unsigned int i = 0; i < friendTrees.size(); i++){
    friendTrees[i]->AddFriend(nt);
    for (unsigned int j = 0; j < friendTrees.size(); j++){
      if (i==j) continue;
      friendTrees[i]->AddFriend(friendTrees[j]);
    }
  }

  // 3. get the friend names

  vector<TString> friendNames;
  for (unsigned int i = 0; i < friendTrees.size(); i++){
    TString name = friendTrees[i]->GetName();
    vector<TString> parts = FSString::parseTString(name,"_");
    friendNames.push_back(parts[parts.size()-1]);
  }

  // 4. loop over friend trees and skim them

  for (unsigned int i = 0; i < friendTrees.size(); i++){
    TString fileNameFriend = fileNameOutput + "." + friendNames[i];
    TString chainNameFriend = chainName + "_" + friendNames[i];
    cout << "Copying Friend: \n\t" << chainNameFriend
         << "\nTo File: \n\t" << fileNameFriend << endl;
    TFile* fileFriend = new TFile(fileNameFriend,"recreate");
    fileFriend->cd();
    TTree* treeFriend = NULL;
    if (maxEntries < 0)  treeFriend = friendTrees[i]->CopyTree(newCuts);
    if (maxEntries >= 0) treeFriend = friendTrees[i]->CopyTree(newCuts,"",maxEntries);
    if (!treeFriend){
      cout << "Could not copy the friend tree....  skipping..." << endl;
      return;
    }
    cout << "\nNumber of entries kept:  \n\t"
         << FSString::int2TString(treeFriend->GetEntries(),0,true) << endl << endl;
    if (treeFriend->GetListOfFriends()) treeFriend->GetListOfFriends()->Clear();
    fileFriend->Write();
    fileFriend->Close();
    delete fileFriend;
  }
  clearChainCache();

}



  // ********************************************************
  // CREATE A FRIEND TREE
  // ********************************************************

void
FSTree::createFriendTree(TString fileNameInput, TString treeNameInput,  TString friendName,
                                  TString friendVariable){
  vector< pair<TString,TString> > friendTreeContents;
  friendTreeContents.push_back(pair<TString,TString>(friendName,friendVariable));
  createFriendTree(fileNameInput,treeNameInput,friendName,friendTreeContents);
}


void
FSTree::createFriendTree(TString fileNameInput, TString treeNameInput,  TString friendName,
                                vector< pair<TString,TString> > friendTreeContents){

    // expand cuts and macros

  fileNameInput = FSString::removeWhiteSpace(fileNameInput);
  treeNameInput = FSString::removeWhiteSpace(treeNameInput);
  friendName = FSString::removeWhiteSpace(friendName);
  for (unsigned int i = 0; i < friendTreeContents.size(); i++){
    friendTreeContents[i].first = FSString::removeWhiteSpace(friendTreeContents[i].first);
    friendTreeContents[i].second = FSString::removeWhiteSpace(friendTreeContents[i].second);
    vector< pair<TString,double> > fsCuts = FSCut::expandCuts(friendTreeContents[i].second);
    if (fsCuts.size() == 1){ friendTreeContents[i].second = fsCuts[0].first; }
    else{ cout << "FSTree::createFriendTree ERROR: multidimensional sidebands not allowed" << endl; return; }
    friendTreeContents[i].second = expandVariable(friendTreeContents[i].second);
  }

    // loop over files

  vector<TString> fileNames = FSSystem::getAbsolutePaths(fileNameInput);
  for (unsigned int ifile = 0; ifile < fileNames.size(); ifile++){
    TString fileNameInput_i = fileNames[ifile];
    TString treeNameInput_i = treeNameInput;
    if (treeNameInput_i == "") treeNameInput_i = FSTree::getTreeNameFromFile(fileNameInput_i);

        // prepare to read from the input tree (set up formulas, etc.)

    TString treeSTATUS;
    TTree* ntInput = FSTree::getTChain(fileNameInput_i,treeNameInput_i,treeSTATUS);
    if (!ntInput || treeSTATUS.Contains("!!")){
      cout << "FSTree::createFriendTree WARNING:  " <<
      endl << "trouble creating TChain  (status = " << treeSTATUS << ")" << endl;
      continue;
    }
    TObjArray* objFormulas = new TObjArray();
    vector<TTreeFormula*> vecFormulas;
    for (unsigned int i = 0; i < friendTreeContents.size(); i++){
      TString formulaName("formulaNum"); formulaName += i;
      TTreeFormula* formula_i = new TTreeFormula(formulaName,friendTreeContents[i].second,ntInput);
      objFormulas->Add(formula_i);
      vecFormulas.push_back(formula_i);
    }
    ntInput->SetNotify(objFormulas);

        // set up the friend tree

    TString fileNameFriend(fileNameInput_i);  fileNameFriend += ".";  fileNameFriend += friendName;
    TString treeNameFriend(treeNameInput_i);  treeNameFriend += "_";  treeNameFriend += friendName;
    cout << "creating friend tree in file: " << "  " << fileNameFriend << endl;
    TFile* friendTFile = new TFile(fileNameFriend,"recreate");  friendTFile->cd();
    TTree* friendTTree = new TTree(treeNameFriend, treeNameFriend);
    Double_t friendVariables[1000];
    if (friendTreeContents.size() > 1000){ cout << "FSTree::createFriendTree ERROR: too many branches" << endl; exit(0); }
    for (unsigned int i = 0; i < friendTreeContents.size(); i++){ 
      friendVariables[i] = 0.0;
      friendTTree->Branch(friendTreeContents[i].first, &(friendVariables[i]), friendTreeContents[i].first+"/D");
    }

      // read from the input tree and write to the friend tree

    unsigned int nEvents = ntInput->GetEntries();
    for (unsigned int ientry = 0; ientry < nEvents; ientry++){
      ntInput->GetEntry(ientry);
      for (unsigned int i = 0; i < vecFormulas.size(); i++){
        friendVariables[i] = vecFormulas[i]->EvalInstance();
      }
      friendTTree->Fill();
    }

      // write out the friend tree and remove formulas

    friendTFile->cd();
    friendTTree->Write();
    delete friendTFile;
    //delete friendTTree; (seg fault)
    for (unsigned int i = 0; i < vecFormulas.size(); i++){
      if (vecFormulas[i]) delete vecFormulas[i];
    }
    ntInput->SetNotify(nullptr);
    delete objFormulas;

  }

}



    // ********************************************************
    // DEFINE SPECIAL FOUR-VECTORS AND MACROS
    // ********************************************************

void
FSTree::makeStandardDefinitions(){
  if (!m_madeStandardDefinitions){
    m_madeStandardDefinitions = true;

      // Special four-vectors for BESIII
      // (B3BEAM included for backwards compatibility)
    defineFourVector("BES3BEAM","BeamEnergy","BeamEnergy*sin(0.011)","0.0","BeamEnergy*cos(0.011)");
    defineFourVector("BES3CMS","2.0*BeamEnergy","2.0*BeamEnergy*sin(0.011)","0.0","0.0");
    defineFourVector("B3BEAM", "2.0*BeamEnergy","2.0*BeamEnergy*sin(0.011)","0.0","0.0");

      // Special four-vectors for CLEO
    defineFourVector("CLEOBEAM","BeamEnergy","BeamEnergy*sin(-0.003)","0.0","BeamEnergy*cos(-0.003)");
    defineFourVector("CLEOCMS","2.0*BeamEnergy","2.0*BeamEnergy*sin(-0.003)","0.0","0.0");

      // Special four-vectors for GlueX
    defineFourVector("GLUEXBEAM","EnPB","PxPB","PyPB","PzPB");
    defineFourVector("GLUEXTARGET","0.938272","0.0","0.0","0.0");
    defineFourVector("GLUEXCMS","(EnPB+0.938272)","PxPB","PyPB","PzPB");

    defineMacro("MOMENTUMX",   1,    "(PxP[I])" );

    defineMacro("MOMENTUMY",   1,    "(PyP[I])" );

    defineMacro("MOMENTUMZ",   1,    "(PzP[I])" );

    defineMacro("ENERGY",      1,    "(EnP[I])" );

    defineMacro("MOMENTUMR",   1,    "(sqrt(pow((PxP[I]),2)+"
                                           "pow((PyP[I]),2)))" );

    defineMacro("MOMENTUM",    1,    "(sqrt(pow((PxP[I]),2)+"
                                           "pow((PyP[I]),2)+"
                                           "pow((PzP[I]),2)))" );

    defineMacro("MASS",        1, "(sqrt(pow((EnP[I]),2)-"
                                        "pow((PxP[I]),2)-"
                                        "pow((PyP[I]),2)-"
                                        "pow((PzP[I]),2)))" );

    defineMacro("MASS",        2, "(sqrt(pow((EnP[I]-EnP[J]),2)-"
                                        "pow((PxP[I]-PxP[J]),2)-"
                                        "pow((PyP[I]-PyP[J]),2)-"
                                        "pow((PzP[I]-PzP[J]),2)))" );

    defineMacro("MASS2",       1,      "(pow((EnP[I]),2)-"
                                        "pow((PxP[I]),2)-"
                                        "pow((PyP[I]),2)-"
                                        "pow((PzP[I]),2))" );

    defineMacro("MASS2",       2,      "(pow((EnP[I]-EnP[J]),2)-"
                                        "pow((PxP[I]-PxP[J]),2)-"
                                        "pow((PyP[I]-PyP[J]),2)-"
                                        "pow((PzP[I]-PzP[J]),2))" );

    defineMacro("RECOILMASS",  2, "(sqrt(pow((EnP[I]-EnP[J]),2)-"
                                        "pow((PxP[I]-PxP[J]),2)-"
                                        "pow((PyP[I]-PyP[J]),2)-"
                                        "pow((PzP[I]-PzP[J]),2)))" );

    defineMacro("RECOILMASS2", 2,      "(pow((EnP[I]-EnP[J]),2)-"
                                        "pow((PxP[I]-PxP[J]),2)-"
                                        "pow((PyP[I]-PyP[J]),2)-"
                                        "pow((PzP[I]-PzP[J]),2))" );

    defineMacro("DOTPRODUCT",  2,      "((PxP[I])*(PxP[J])+"
                                        "(PyP[I])*(PyP[J])+"
                                        "(PzP[I])*(PzP[J]))" );

    defineMacro("COSINE",      1,            "((PzP[I])/" 
                                    "(sqrt(pow((PxP[I]),2)+" 
                                          "pow((PyP[I]),2)+" 
                                          "pow((PzP[I]),2))))" );

    defineMacro("COSINE",      2,      "(((PxP[I])*(PxP[J])+" 
                                         "(PyP[I])*(PyP[J])+" 
                                         "(PzP[I])*(PzP[J]))/" 
                                    "(sqrt(pow((PxP[I]),2)+" 
                                          "pow((PyP[I]),2)+" 
                                          "pow((PzP[I]),2)))/"  
                                    "(sqrt(pow((PxP[J]),2)+" 
                                          "pow((PyP[J]),2)+" 
                                          "pow((PzP[J]),2))))" );

        {TString pmag = "(sqrt(pow((PxP[J]),2)+" 
                              "pow((PyP[J]),2)+" 
                              "pow((PzP[J]),2)))";
         TString bmag = "(sqrt(pow((PxP[I]),2)+"
                              "pow((PyP[I]),2)+"
                              "pow((PzP[I]),2)))";
         TString Ep   = "(EnP[J])";
         TString Eb   = "(EnP[I])";
         TString costheta = "(((PxP[I])*(PxP[J])+"
                              "(PyP[I])*(PyP[J])+"
                              "(PzP[I])*(PzP[J]))/"
                                 "(" + pmag + "*" + bmag + "))";
         TString sintheta = "(sqrt(1.0-pow(" + costheta + ",2)))";
         TString ppar  = "("+pmag+"*"+costheta+")";
         TString pperp = "("+pmag+"*"+sintheta+")";
         TString beta = "(("+bmag+")/("+Eb+"))";
         TString gamma = "(1.0/sqrt(1.0-pow("+beta+",2)))";
         TString pparp = "("+gamma+"*"+ppar+ "-" +gamma+"*"+beta+"*"+Ep+")";
         TString pmagp = "(sqrt(pow("+pparp+",2)+pow("+pperp+",2)))";
         TString helcosine = "("+pparp+"/"+pmagp+")";

    defineMacro("HELCOSINE", 2,  helcosine );}

    defineMacro("HELCOSTHETA", 3, "FSMath::helcostheta("
                                  "PxP[I],PyP[I],PzP[I],EnP[I],"
                                  "PxP[J],PyP[J],PzP[J],EnP[J],"
                                  "PxP[M],PyP[M],PzP[M],EnP[M])");

    defineMacro("HELCOSTHETA", 4, "FSMath::helcostheta("
                                  "PxP[I],PyP[I],PzP[I],EnP[I],"
                                  "PxP[J],PyP[J],PzP[J],EnP[J],"
                                  "PxP[M],PyP[M],PzP[M],EnP[M])"
                                  "+EnP[N]*0.0");

    defineMacro("HELPHI", 4,      "FSMath::helphi("
                                  "PxP[I],PyP[I],PzP[I],EnP[I],"
                                  "PxP[J],PyP[J],PzP[J],EnP[J],"
                                  "PxP[M],PyP[M],PzP[M],EnP[M],"
                                  "PxP[N],PyP[N],PzP[N],EnP[N])");

    defineMacro("GJCOSTHETA", 3, "FSMath::gjcostheta("
                                  "PxP[I],PyP[I],PzP[I],EnP[I],"
                                  "PxP[J],PyP[J],PzP[J],EnP[J],"
                                  "PxP[M],PyP[M],PzP[M],EnP[M])");

    defineMacro("GJCOSTHETA", 4, "FSMath::gjcostheta("
                                  "PxP[I],PyP[I],PzP[I],EnP[I],"
                                  "PxP[J],PyP[J],PzP[J],EnP[J],"
                                  "PxP[N],PyP[N],PzP[N],EnP[N])"
                                  "+EnP[M]*0.0");

    defineMacro("GJPHI", 4,       "FSMath::gjphi("
                                  "PxP[I],PyP[I],PzP[I],EnP[I],"
                                  "PxP[J],PyP[J],PzP[J],EnP[J],"
                                  "PxP[M],PyP[M],PzP[M],EnP[M],"
                                  "PxP[N],PyP[N],PzP[N],EnP[N])");

    defineMacro("PRODCOSTHETA",3, "FSMath::prodcostheta("
                                  "PxP[I],PyP[I],PzP[I],EnP[I],"
                                  "PxP[J],PyP[J],PzP[J],EnP[J],"
                                  "PxP[M],PyP[M],PzP[M],EnP[M])");

    defineMacro("PLANEPHI", 3,    "FSMath::planephi("
                                  "PxP[I],PyP[I],PzP[I],EnP[I],"
                                  "PxP[J],PyP[J],PzP[J],EnP[J],"
                                  "PxP[M],PyP[M],PzP[M],EnP[M])");

    defineMacro("ENERGY",  2,    "FSMath::boostEnergy("
                                  "PxP[I],PyP[I],PzP[I],EnP[I],"
                                  "PxP[J],PyP[J],PzP[J],EnP[J])");

  }
}


void
FSTree::defineFourVector(TString indexName, TString En, TString Px, TString Py, TString Pz){
  indexName = FSString::removeWhiteSpace(indexName);
  En = FSString::removeWhiteSpace(En);
  Px = FSString::removeWhiteSpace(Px);
  Py = FSString::removeWhiteSpace(Py);
  Pz = FSString::removeWhiteSpace(Pz);
  makeStandardDefinitions();
  map<TString,TString>::const_iterator mapItr = m_mapDefinedEn.find(indexName);
  if (mapItr != m_mapDefinedEn.end()){
    cout << "FSTree WARNING:  overwriting four-vector named " << indexName << endl;
  }
  m_mapDefinedEn[indexName] = En;
  m_mapDefinedPx[indexName] = Px;
  m_mapDefinedPy[indexName] = Py;
  m_mapDefinedPz[indexName] = Pz;
}

void
FSTree::showDefinedFourVectors(){
  makeStandardDefinitions();
  cout << "----------------------------" << endl;
  cout << "    DEFINED FOUR-VECTORS " << endl;
  cout << "----------------------------" << endl;
  for (map<TString,TString>::iterator mapItr = m_mapDefinedEn.begin(); 
       mapItr != m_mapDefinedEn.end(); mapItr++){
    TString name = mapItr->first;
    cout << name << ":" << endl;
    cout << "  En = " << m_mapDefinedEn[name] << endl;
    cout << "  Px = " << m_mapDefinedPx[name] << endl;
    cout << "  Py = " << m_mapDefinedPy[name] << endl;
    cout << "  Pz = " << m_mapDefinedPz[name] << endl;
  }
  cout << "----------------------------" << endl;
}


void
FSTree::defineMacro(TString macroName, int numFourVectors, TString macro){
  macroName = FSString::removeWhiteSpace(macroName);
  macro     = FSString::removeWhiteSpace(macro);
  makeStandardDefinitions();
  if ((macroName == "") || (macro == "")) return;
  if ((numFourVectors < 1) || (numFourVectors > 4)){
    cout << "FSTree::defineMacro WARNING: numFourVectors should be 1 to 4, nothing defined" << endl;
    return;
  }
  pair<TString,int> macroNamePair(macroName,numFourVectors);
  if (m_mapDefinedMacros.find(macroNamePair) != m_mapDefinedMacros.end()){
    cout << "FSTree::defineMacro WARNING: overwriting macro named " << macroName << endl;
  }
  if ((numFourVectors >= 1) && (!macro.Contains("[I]"))){
    cout << "FSTree::defineMacro WARNING: macro named " << macroName << " should contain [I]" << endl;
    return;
  }
  if ((numFourVectors >= 2) && (!macro.Contains("[J]"))){
    cout << "FSTree::defineMacro WARNING: macro named " << macroName << " should contain [J]" << endl;
    return;
  }
  if ((numFourVectors >= 3) && (!macro.Contains("[M]"))){
    cout << "FSTree::defineMacro WARNING: macro named " << macroName << " should contain [M]" << endl;
    return;
  }
  if ((numFourVectors >= 4) && (!macro.Contains("[N]"))){
    cout << "FSTree::defineMacro WARNING: macro named " << macroName << " should contain [N]" << endl;
    return;
  }
  m_mapDefinedMacros[macroNamePair] = macro;
  bool inVector = false;
  for (unsigned int i = 0; i < m_vectorDefinedMacroKeywords.size(); i++){
    if (m_vectorDefinedMacroKeywords[i] == macroName){ inVector = true; break; }
  }
  if (!inVector){
    m_vectorDefinedMacroKeywords.push_back(macroName);
    for (unsigned int i = 0; i < m_vectorDefinedMacroKeywords.size()-1; i++){
    for (unsigned int j = i+1; j < m_vectorDefinedMacroKeywords.size(); j++){
      if (m_vectorDefinedMacroKeywords[i].Length() < m_vectorDefinedMacroKeywords[j].Length()){
        TString temp = m_vectorDefinedMacroKeywords[i];
        m_vectorDefinedMacroKeywords[i] = m_vectorDefinedMacroKeywords[j];
        m_vectorDefinedMacroKeywords[j] = temp;
      }
    }}
  }
}


void
FSTree::showDefinedMacros(){
  makeStandardDefinitions();
  cout << "----------------------------" << endl;
  cout << "       DEFINED MACROS " << endl;
  cout << "----------------------------" << endl << endl;
  for (map< pair<TString,int>, TString>::iterator it = m_mapDefinedMacros.begin();
       it != m_mapDefinedMacros.end(); it++){
    TString macroName = it->first.first;
    int numFourVectors = it->first.second;
    TString macro = it->second;
    if (numFourVectors == 1) cout << macroName << "([I])" << endl << endl; 
    if (numFourVectors == 2) cout << macroName << "([I];[J])" << endl << endl; 
    if (numFourVectors == 3) cout << macroName << "([I];[J];[M])" << endl << endl; 
    if (numFourVectors == 4) cout << macroName << "([I];[J];[M];[N])" << endl << endl; 
    cout << "          " << macro << endl << endl;
  }
  cout << "----------------------------" << endl;
}



  // ********************************************************
  // EXPAND VARIABLE MACROS
  // ********************************************************


TString
FSTree::reorderVariable(TString variable, bool show){
  variable = FSString::removeWhiteSpace(variable);
  makeStandardDefinitions();
  vector< vector<TString> > expressions = parseVariable(variable,show);
  for (unsigned int i = 0; i < expressions.size(); i++){
    TString expression = expressions[i][0];
    TString parentheses = expressions[i][3];
    TString newExp = expressions[i][expressions[i].size()-1];
    variable.Replace(variable.Index(expression),expression.Length(),newExp);
  }
  return variable;
}


vector< vector<TString> >
FSTree::parseVariable(TString variable, bool show){
  variable = FSString::removeWhiteSpace(variable);
  TString tempVar = variable;
  vector <vector<TString> > parsedVariable;
  vector<TString> keywords = m_vectorDefinedMacroKeywords;
  for (unsigned int i = 0; i < keywords.size(); i++){
    while (tempVar.Contains(keywords[i]+"(")){
      vector<TString> parsedExpression;  parsedExpression.resize(4);
      TString pars = 
        FSString::captureParentheses(tempVar,tempVar.Index(keywords[i]+"("));
      TString expression = keywords[i]+"("+pars+")";
      TString prefix("");
      int index = tempVar.Index(expression);
      if (index >= 1 && TString(tempVar[index-1]).IsAlpha()){
        prefix = TString(tempVar[index-1]) + prefix;
        if (index >= 2 && TString(tempVar[index-2]).IsAlpha())
          prefix = TString(tempVar[index-2]) + prefix;}
      expression = prefix + expression;
      tempVar.Replace(tempVar.Index(expression),expression.Length(),"");
      parsedExpression[0] = expression;
      parsedExpression[1] = prefix;
      parsedExpression[2] = keywords[i];
      parsedExpression[3] = pars;
      vector<TString> semiParts = FSString::parseTString(pars,";");
      for (unsigned int j = 0; j < semiParts.size(); j++){
        vector<TString> commaParts = FSString::parseTString(semiParts[j],",");
        if (commaParts.size() == 0) continue;
        for (unsigned int ic = 0; ic < commaParts.size()-1; ic++){
        for (unsigned int jc = ic+1; jc < commaParts.size(); jc++){
          if (FSString::TString2string(commaParts[jc]) < 
              FSString::TString2string(commaParts[ic])){
            TString tempComma = commaParts[ic];
            commaParts[ic] = commaParts[jc];
            commaParts[jc] = tempComma;
          }
        }}
        TString newSemiPart("");
        for (unsigned int ic = 0; ic < commaParts.size(); ic++){
          if (commaParts[ic] == "") continue;
          newSemiPart += commaParts[ic];
          if (ic != commaParts.size()-1) newSemiPart += ",";
        }
        if (newSemiPart != "") parsedExpression.push_back(newSemiPart);
      }
      TString newPars("");
      for (unsigned int j = 4; j < parsedExpression.size(); j++){
        newPars += parsedExpression[j];
        if (j != parsedExpression.size()-1) newPars += ";";
      }
      parsedExpression.push_back(newPars);
      TString newExpression = expression;
      newExpression.Replace(expression.Index(pars),pars.Length(),newPars);
      parsedExpression.push_back(newExpression);
      parsedVariable.push_back(parsedExpression);
    }
  }
  if (show){
    cout << "------ PARSING VARIABLE -----" << endl;
    cout << variable << endl;
    cout << "-----------------------------" << endl;
    for (unsigned int i = 0; i < parsedVariable.size(); i++){
      cout << "expression  = " << parsedVariable[i][0] << endl;
      cout << "prefix      = " << parsedVariable[i][1] << endl;
      cout << "keyword     = " << parsedVariable[i][2] << endl;
      cout << "parentheses = " << parsedVariable[i][3] << endl;
      for (unsigned int j = 4; j < parsedVariable[i].size()-2; j++){
      cout << "    part " << j-3 << "  = " << parsedVariable[i][j] << endl;}
      cout << "new parentheses = " << parsedVariable[i][parsedVariable[i].size()-2] << endl;
      cout << "new expression  = " << parsedVariable[i][parsedVariable[i].size()-1] << endl;
      cout << "-----------------------------" << endl;
    }
  }
  return parsedVariable;
}


TString
FSTree::expandVariable(TString variable, bool show){
  variable = FSString::removeWhiteSpace(variable);
  makeStandardDefinitions();
  vector< vector<TString> > expressions = parseVariable(variable,show);
  for (unsigned int i = 0; i < expressions.size(); i++){
    vector<TString> expInfo = expressions[i];
    int numParts = expInfo.size()-6;
    if ((numParts < 1) || (numParts > 4)){
      cout << "FSTree::expandVariable ERROR: problem parsing variable..." << endl;
      parseVariable(variable,true);
      return TString("");
    }
    TString expression = expInfo[0];
    TString prefix = expInfo[1];
    TString keyword = expInfo[2];
    TString macro = "";
    pair<TString,int> macroNamePair(keyword,numParts);
    if (m_mapDefinedMacros.find(macroNamePair) != m_mapDefinedMacros.end())
      macro = m_mapDefinedMacros[macroNamePair];
    if (macro == ""){
      cout << "FSTree::expandVariable ERROR: no macro named " << keyword 
           << " with number of parts = " << numParts << endl;
      parseVariable(variable,true);
      return TString("");
    }
    TString newMacro = macro;
    for (int np = 0; np < numParts; np++){
      TString IJ = "[I]"; if (np == 1) IJ = "[J]";
                          if (np == 2) IJ = "[M]";
                          if (np == 3) IJ = "[N]";
      TString macroEn = "EnP"+IJ;  TString newEn = "(";
      TString macroPx = "PxP"+IJ;  TString newPx = "(";
      TString macroPy = "PyP"+IJ;  TString newPy = "(";
      TString macroPz = "PzP"+IJ;  TString newPz = "(";
      TString commaList = expInfo[4+np];
      vector<TString> commaParts = FSString::parseTString(commaList,",");
      int commaCount = 0;
      for (unsigned int ic = 0; ic < commaParts.size(); ic++){
        if (commaParts[ic].Length() == 0) continue;
        TString subMass = "";
        if (FSString::subString(commaParts[ic],commaParts[ic].Length()-2,commaParts[ic].Length()) == "pi"){
          commaParts[ic] = FSString::subString(commaParts[ic],0,commaParts[ic].Length()-2);
          subMass = FSString::double2TString(FSPhysics::XMpi,8); }
        if (FSString::subString(commaParts[ic],commaParts[ic].Length()-1,commaParts[ic].Length()) == "K"){
          commaParts[ic] = FSString::subString(commaParts[ic],0,commaParts[ic].Length()-1);
          subMass = FSString::double2TString(FSPhysics::XMK,8); }
        if (FSString::subString(commaParts[ic],commaParts[ic].Length()-1,commaParts[ic].Length()) == "p"){
          commaParts[ic] = FSString::subString(commaParts[ic],0,commaParts[ic].Length()-1);
          subMass = FSString::double2TString(FSPhysics::XMp,8); }
        if (FSString::subString(commaParts[ic],commaParts[ic].Length()-1,commaParts[ic].Length()) == "e"){
          commaParts[ic] = FSString::subString(commaParts[ic],0,commaParts[ic].Length()-1);
          subMass = FSString::double2TString(FSPhysics::XMe,8); }
        if (FSString::subString(commaParts[ic],commaParts[ic].Length()-2,commaParts[ic].Length()) == "mu"){
          commaParts[ic] = FSString::subString(commaParts[ic],0,commaParts[ic].Length()-2);
          subMass = FSString::double2TString(FSPhysics::XMmu,8); }
        TString plusMinus("+");
        TString firstDigit = FSString::subString(commaParts[ic],0,1);
        if (firstDigit == "+" || firstDigit == "-"){
          plusMinus = firstDigit;
          commaParts[ic] = FSString::subString(commaParts[ic],1,commaParts[ic].Length());
        }
        if (commaParts[ic].Length() == 0) continue;
        commaCount++;
        if (commaCount > 0 && plusMinus == "-"){ 
          newEn += "-"; newPx += "-"; newPy += "-"; newPz += "-"; }
        if (commaCount > 1 && plusMinus == "+"){ 
          newEn += "+"; newPx += "+"; newPy += "+"; newPz += "+"; }
        if (m_mapDefinedEn.find(commaParts[ic]) != m_mapDefinedEn.end()){
          newEn += m_mapDefinedEn[commaParts[ic]];
          newPx += m_mapDefinedPx[commaParts[ic]];
          newPy += m_mapDefinedPy[commaParts[ic]];
          newPz += m_mapDefinedPz[commaParts[ic]];
        }
        else{
          if (subMass != ""){
            newEn += "sqrt("+prefix+"PxP"+commaParts[ic]+"*"+prefix+"PxP"+commaParts[ic]+"+"
                            +prefix+"PyP"+commaParts[ic]+"*"+prefix+"PyP"+commaParts[ic]+"+"
                            +prefix+"PzP"+commaParts[ic]+"*"+prefix+"PzP"+commaParts[ic]+"+"
                            +subMass+"*"+subMass+")";
          }
          else{
            newEn += (prefix+"EnP"+commaParts[ic]);
          }
          newPx += (prefix+"PxP"+commaParts[ic]);
          newPy += (prefix+"PyP"+commaParts[ic]);
          newPz += (prefix+"PzP"+commaParts[ic]);
        }
      }
      newEn += ")"; newPx += ")"; newPy += ")"; newPz += ")";
      while (newMacro.Contains(macroEn)){ 
        newMacro.Replace(newMacro.Index(macroEn),macroEn.Length(),newEn); }
      while (newMacro.Contains(macroPx)){ 
        newMacro.Replace(newMacro.Index(macroPx),macroPx.Length(),newPx); }
      while (newMacro.Contains(macroPy)){ 
        newMacro.Replace(newMacro.Index(macroPy),macroPy.Length(),newPy); }
      while (newMacro.Contains(macroPz)){ 
        newMacro.Replace(newMacro.Index(macroPz),macroPz.Length(),newPz); }
    }
    variable.Replace(variable.Index(expression),expression.Length(),newMacro);
  }
  return variable;
}



  // ********************************************************
  // CLEAR GLOBAL CACHES
  // ********************************************************

void
FSTree::clearChainCache(){
  if (FSControl::DEBUG) 
    cout << "FSTree: clearing chain cache" << endl;
  for (map<TString,TChain*>::iterator rmItr = m_chainCache.begin();
       rmItr != m_chainCache.end(); rmItr++){
    if (rmItr->second) delete rmItr->second;
  }
  m_chainCache.clear();
  if (FSControl::DEBUG) 
    cout << "FSTree: done clearing chain cache" << endl;
}


  // ********************************************************
  // HELPER FUNCTION FOR MAXENTRIES
  // ********************************************************

pair<int,TString> 
FSTree::processMaxEntries(TString input){
  if (!input.Contains("MAXENTRIES=")) return pair<int,TString>(-1,input);
  TString subInput = FSString::subString(input,input.Index("MAXENTRIES="),input.Length());
  int nMaxEntries = FSString::TString2int(subInput);
  TString modifiedInput = input;
  if (modifiedInput.Contains("MAXENTRIES=="))
    modifiedInput.Replace(modifiedInput.Index("MAXENTRIES=="),12,"0!=");
  if (modifiedInput.Contains("MAXENTRIES="))
    modifiedInput.Replace(modifiedInput.Index("MAXENTRIES="),11,"0!=");
  return pair<int,TString> (nMaxEntries,modifiedInput);
}

