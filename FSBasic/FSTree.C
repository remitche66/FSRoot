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
#include "FSBasic/FSControl.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSSystem.h"
#include "FSBasic/FSCut.h"
#include "FSBasic/FSTree.h"


  // static member data


map< TString, TChain*> FSTree::m_chainCache;
vector< pair<TString,bool> > FSTree::m_friendTrees;

map< TString, TString > FSTree::m_mapDefinedPx;
map< TString, TString > FSTree::m_mapDefinedPy;
map< TString, TString > FSTree::m_mapDefinedPz;
map< TString, TString > FSTree::m_mapDefinedEn;
map< TString, TString > FSTree::m_mapDefined1VMacros;
map< TString, TString > FSTree::m_mapDefined2VMacros;
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


void
FSTree::showFriendTrees(){
  cout << "FRIEND TREES:" << endl;
  for (unsigned int i = 0; i < m_friendTrees.size(); i++){
    cout << "  (" << (i+1) << ")  " << m_friendTrees[i].first << endl;
    if (m_friendTrees[i].second)
    cout << "        USED" << endl;
    if (!m_friendTrees[i].second)
    cout << "        NOT USED" << endl;
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

    // expand "cuts" using FSCut and check for multidimensional sidebands

  vector< pair<TString,double> > fsCuts = FSCut::expandCuts(cuts);
  if (fsCuts.size() == 1){ cuts = fsCuts[0].first; }
  else{ cout << "FSTree::skimTree ERROR: multidimensional sidebands not allowed" << endl; return; }

  // just write the command to a file and return

  if (printCommandFile != ""){
    FSString::writeTStringToFile(printCommandFile,
          "$FSROOT/Executables/FSSkimTree "
          " -i \""    + fileNameInput  + "\""
          " -o \""    + fileNameOutput + "\""
          " -cuts \"" + cuts           + "\""
          " -nt \""   + chainName      + "\"",
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
    cout << "\tOriginal selection criteria:" << endl;
    cout << cuts << endl;
    cout << endl;
    cout << "\tModified selection criteria:" << endl;
    cout << newCuts << endl;
    cout << endl;
    if (nt){
      cout << "\tNumber of entries to skim:" << endl;
      cout << FSString::int2TString(nt->GetEntries(),0,true) << endl;
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
  while (chainName.Contains("/")){
    TString dirName("");
    for (int i = 0; i < chainName.Index("/"); i++) { dirName += chainName[i]; }
    chainName.Replace(0,chainName.Index("/")+1,"");
    file2->mkdir(dirName);
    file2->cd(dirName);
  }


  // copy the tree with selection criteria

  TTree* tree2 = nt->CopyTree(newCuts);
  if (newChainName != ""){ tree2->SetName(newChainName); tree2->SetTitle(newChainName); }
  if (!FSControl::QUIET){
    cout << "\tNumber of entries kept:" << endl;
    cout << FSString::int2TString(tree2->GetEntries(),0,true) << endl;
    cout << endl;
    cout << "\tSkim Ratio:" << endl;
    cout << (float) tree2->GetEntries() / (float) nt->GetEntries() << endl;
    cout << endl;
  }


  // write the tree to file2

  tree2->AutoSave();
  file2->Write();
  file2->Close();

  clearChainCache();
  delete file2;

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
  if ((numFourVectors < 1) || (numFourVectors > 2)){
    cout << "FSTree::defineMacro WARNING: numFourVectors should be 1 or 2, nothing defined" << endl;
    return;
  }
  if (((numFourVectors == 1) && (m_mapDefined1VMacros.find(macroName) != m_mapDefined1VMacros.end())) ||
      ((numFourVectors == 2) && (m_mapDefined2VMacros.find(macroName) != m_mapDefined2VMacros.end()))){
    cout << "FSTree::defineMacro WARNING: overwriting macro named " << macroName << endl;
  }
  if ((numFourVectors == 1) && (!macro.Contains("[I]"))){
    cout << "FSTree::defineMacro WARNING: for numFourVectors = 1, macro should contain [I]" << endl;
    return;
  }
  if ((numFourVectors == 2) && (!macro.Contains("[I]") || !macro.Contains("[J]"))){
    cout << "FSTree::defineMacro WARNING: for numFourVectors = 2, macro should contain [I] and [J]" << endl;
    return;
  }
  if (numFourVectors == 1) m_mapDefined1VMacros[macroName] = macro;
  if (numFourVectors == 2) m_mapDefined2VMacros[macroName] = macro;
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
  for (unsigned int i = 0; i < m_vectorDefinedMacroKeywords.size(); i++){
    TString name = m_vectorDefinedMacroKeywords[i];
    if (m_mapDefined1VMacros.find(name) != m_mapDefined1VMacros.end()){
      cout << name << "([I])" << endl << endl; 
      cout << "          " << m_mapDefined1VMacros[name] << endl << endl;
    }
    if (m_mapDefined2VMacros.find(name) != m_mapDefined2VMacros.end()){
      cout << name << "([I];[J])" << endl << endl; 
      cout << "          " << m_mapDefined2VMacros[name] << endl << endl;
    }
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
    if ((numParts != 1) && (numParts != 2)){
      cout << "FSTree::expandVariable ERROR: problem parsing variable..." << endl;
      parseVariable(variable,true);
      return TString("");
    }
    TString expression = expInfo[0];
    TString prefix = expInfo[1];
    TString keyword = expInfo[2];
    TString macro = "";
    if (numParts == 1 && m_mapDefined1VMacros.find(keyword) != m_mapDefined1VMacros.end())
      macro = m_mapDefined1VMacros[keyword];
    if (numParts == 2 && m_mapDefined2VMacros.find(keyword) != m_mapDefined2VMacros.end())
      macro = m_mapDefined2VMacros[keyword];
    if (macro == ""){
      cout << "FSTree::expandVariable ERROR: no macro named " << keyword 
           << " with number of parts = " << numParts << endl;
      parseVariable(variable,true);
      return TString("");
    }
    TString newMacro = macro;
    for (int np = 0; np < numParts; np++){
      TString IJ = "[I]"; if (np == 1) IJ = "[J]";
      TString macroEn = "EnP"+IJ;  TString newEn = "(";
      TString macroPx = "PxP"+IJ;  TString newPx = "(";
      TString macroPy = "PyP"+IJ;  TString newPy = "(";
      TString macroPz = "PzP"+IJ;  TString newPz = "(";
      TString commaList = expInfo[4+np];
      vector<TString> commaParts = FSString::parseTString(commaList,",");
      for (unsigned int ic = 0; ic < commaParts.size(); ic++){
        if (m_mapDefinedEn.find(commaParts[ic]) != m_mapDefinedEn.end()){
          newEn += m_mapDefinedEn[commaParts[ic]];
          newPx += m_mapDefinedPx[commaParts[ic]];
          newPy += m_mapDefinedPy[commaParts[ic]];
          newPz += m_mapDefinedPz[commaParts[ic]];
        }
        else{
          newEn += (prefix+"EnP"+commaParts[ic]);
          newPx += (prefix+"PxP"+commaParts[ic]);
          newPy += (prefix+"PyP"+commaParts[ic]);
          newPz += (prefix+"PzP"+commaParts[ic]);
        }
        if (ic != commaParts.size()-1){ 
          newEn += "+"; newPx += "+"; newPy += "+"; newPz += "+"; }
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

