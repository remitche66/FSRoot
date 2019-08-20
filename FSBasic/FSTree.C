#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "TChain.h"
#include "TChainElement.h"
#include "TObjArray.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "FSBasic/FSControl.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSCut.h"
#include "FSBasic/FSTree.h"


  // static member data

map< TString, TChain*> FSTree::m_chainCache;
map< TString, TFile*> FSTree::m_fileCache;

map< TString, TString > FSTree::m_mapDefinedPx;
map< TString, TString > FSTree::m_mapDefinedPy;
map< TString, TString > FSTree::m_mapDefinedPz;
map< TString, TString > FSTree::m_mapDefinedEn;


  // ********************************************************
  // GET A TCHAIN FROM THE CACHE OR CREATE A NEW ONE
  // ********************************************************

TChain*
FSTree::getTChain(TString fileName, TString ntName, 
                              bool addFilesIndividually){
  TChain* nt = NULL;

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
    if (addFilesIndividually){
      vector<TString> fileList;
      {
        TChain ntTemp(ntName); 
        ntTemp.Add(fileName);
        TIter next(ntTemp.GetListOfFiles());
        while ( TChainElement* el = (TChainElement*) next()){
          fileList.push_back(el->GetTitle());
        }
      }
      for (unsigned int i = 0; i < fileList.size(); i++){
         TChain ntTemp(ntName);
         ntTemp.Add(fileList[i]);
         if ((ntTemp.GetEntries() > 0) && (ntTemp.GetNbranches() > 0)){
           if (FSControl::DEBUG) 
             cout << "FSTree: adding file to TChain " << fileList[i] << endl;
           nt->Add(fileList[i]);
        }
      }
    }
    else{
      nt->Add(fileName);
    }
    if ((nt->GetEntries() == 0) || (nt->GetNbranches() == 0)){
      if (FSControl::DEBUG) 
	cout << "FSTree: null TChain with index " << index << endl;
      delete nt;
      nt = NULL;
    }
    else{
      m_chainCache[index] = nt;
    }
  }


  if ((nt) && (FSControl::CHAINFRIEND != "")){

    TString friendFileName(fileName);
    friendFileName += ".";
    friendFileName += FSControl::CHAINFRIEND;

    TString friendNTName(ntName);
    friendNTName += "_";
    friendNTName += FSControl::CHAINFRIEND;

    nt->AddFriend(friendNTName,friendFileName);

  }


  return nt;

}


  // ********************************************************
  // GET A TFILE FROM THE CACHE OR CREATE A NEW ONE
  // ********************************************************

TFile*
FSTree::getTFile(TString fileName){
  TFile* tf;

    // clear the file cache if there is no file caching

  if (!FSControl::FILECACHING) clearFileCache();

    // create an index for this file and search for it

  TString index(fileName);
  map<TString,TFile*>::const_iterator mapItr = m_fileCache.find(index);
  if (mapItr != m_fileCache.end()){
    if (FSControl::DEBUG) 
      cout << "FSTree: found TFile with index " << index << endl;
    tf = m_fileCache[index];
  }

    // create the file from scratch if not found

  else{
    if (FSControl::DEBUG) 
      cout << "FSTree: creating new TFile with index " << index << endl;
    tf = new TFile(fileName);
    m_fileCache[index] = tf;
  }

  return tf;

}



  // ********************************************************
  // SKIM A TREE AND OUTPUT TO A DIFFERENT FILE
  // ********************************************************

void
FSTree::skimTree(TString fileNameInput, TString chainName, 
                 TString fileNameOutput, TString cuts, 
                 TString newChainName, TString printCommandFile){
  cuts = FSString::removeWhiteSpace(cuts);

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

    // expand "cuts" using FSCut and check for multidimensional sidebands

  vector< pair<TString,double> > fsCuts = FSCut::expandCuts(cuts);
  if (fsCuts.size() == 1){ cuts = fsCuts[0].first; }
  else{ cout << "FSTree::skimTree Error: multidimensional sidebands not allowed" << endl; exit(1); }

  // expand variable macros

  TString newCuts = expandVariable(cuts);

  // write out message

  if (FSControl::QUIET){
    cout << "creating " << fileNameOutput << endl;
  }
  if (!FSControl::QUIET){
    cout << endl;
    cout << "Copying File:" << endl;
    cout << "\t" << fileNameInput << endl;
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
      cout << nt->GetEntries() << endl;
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
  if (newChainName != "") tree2->SetName(newChainName);
  if (!FSControl::QUIET){
    cout << "\tNumber of entries kept:" << endl;
    cout << tree2->GetEntries() << endl;
    cout << endl;
    cout << "\tSkim Ratio:" << endl;
    cout << (float) tree2->GetEntries() / (float) nt->GetEntries() << endl;
    cout << endl;
  }


  // write the tree to file2

  tree2->AutoSave();
  //tree2->Write();
  file2->Close();

  clearChainCache();
  delete file2;

}



  // ********************************************************
  // EXPAND VARIABLE MACROS
  // ********************************************************

TString 
FSTree::expandVariable(TString variable){

  TString PREFIXMARK("");

  TString RECOILMASSMARK  ("RECOILMASS(");
  TString RECOILMASS2MARK ("RECOILMASS2(");
  TString MASSMARK        ("MASS(");
  TString MASS2MARK       ("MASS2(");
  TString MOMXMARK        ("MOMENTUMX(");
  TString MOMYMARK        ("MOMENTUMY(");
  TString MOMZMARK        ("MOMENTUMZ(");
  TString MOMRMARK        ("MOMENTUMR(");
  TString MOMMARK         ("MOMENTUM(");
  TString ENMARK          ("ENERGY(");
  TString DOTPRODUCT      ("DOTPRODUCT(");
  TString HELCOSINE       ("HELCOSINE(");
  TString COSINE          ("COSINE(");

  while (variable.Contains(RECOILMASSMARK) ||
         variable.Contains(RECOILMASS2MARK) ||
         variable.Contains(MASSMARK) ||
         variable.Contains(MASS2MARK)||
         variable.Contains(MOMXMARK) ||
         variable.Contains(MOMYMARK) ||
         variable.Contains(MOMZMARK) ||
         variable.Contains(MOMRMARK) ||
         variable.Contains(MOMMARK)  ||
         variable.Contains(ENMARK)   ||
         variable.Contains(DOTPRODUCT) ||
         variable.Contains(HELCOSINE) ||
         variable.Contains(COSINE)){

    int index = 0;
    int size = 0;
    TString EnN("");  TString EnM("");  TString EnD("");
    TString PxN("");  TString PxM("");	TString PxD("");
    TString PyN("");  TString PyM("");	TString PyD("");
    TString PzN("");  TString PzM("");	TString PzD("");
    TString mark("");
         if (variable.Contains(RECOILMASSMARK))  mark = RECOILMASSMARK;
    else if (variable.Contains(RECOILMASS2MARK)) mark = RECOILMASS2MARK;
    else if (variable.Contains(MASSMARK))        mark = MASSMARK;
    else if (variable.Contains(MASS2MARK))       mark = MASS2MARK;
    else if (variable.Contains(MOMXMARK))        mark = MOMXMARK;
    else if (variable.Contains(MOMYMARK))        mark = MOMYMARK;
    else if (variable.Contains(MOMZMARK))        mark = MOMZMARK;
    else if (variable.Contains(MOMRMARK))        mark = MOMRMARK;
    else if (variable.Contains(MOMMARK))         mark = MOMMARK;
    else if (variable.Contains(ENMARK))          mark = ENMARK;
    else if (variable.Contains(DOTPRODUCT))      mark = DOTPRODUCT;
    else if (variable.Contains(HELCOSINE))       mark = HELCOSINE;
    else if (variable.Contains(COSINE))          mark = COSINE;


      // determine if there is a prefix and set markers
    PREFIXMARK = "";
    if (variable.Contains(mark) && 
             variable.Index(mark) >= 1 &&
             TString(variable[variable.Index(mark)-1]).IsAlpha()){
           PREFIXMARK = "";
         if (variable.Index(mark) >= 2 && 
             TString(variable[variable.Index(mark)-2]).IsAlpha())
           PREFIXMARK += variable[variable.Index(mark)-2];
           PREFIXMARK += variable[variable.Index(mark)-1];
	   index = variable.Index(PREFIXMARK+mark);
	   size = (PREFIXMARK+mark).Length()+1;
    }
    else if (variable.Contains(mark)){
	   index = variable.Index(mark);
	   size = (mark).Length()+1;
    }


      // make vectors of indices
    vector<TString> pN;  // indices before the semicolon
    vector<TString> pM;  // indices after the semicolon
    bool usevectorM = false;
    TString pIndex("");
    for (int i = index+size-1; (variable[i] != ')' && i < variable.Length()); i++){
      size++;
      if (variable[i] == ',' || variable[i] == ';'){
        if (!usevectorM)  pN.push_back(pIndex);
        if  (usevectorM)  pM.push_back(pIndex);
        if (variable[i] == ';') usevectorM = true;
        pIndex = "";
      }
      else{
        pIndex += variable[i];
        TString testVariable(variable[i]);
        if (!testVariable.IsAlnum()){
          cout << "FSTtree::expandVariable WARNING: variable contains a special ";
          cout << "character: " << variable << endl;
        }
      }
    }
    if (pIndex.Length() > 0 && !usevectorM) pN.push_back(pIndex);
    if (pIndex.Length() > 0 &&  usevectorM) pM.push_back(pIndex);


      // sort the pN vector
    for (unsigned int i = 0; (pN.size() != 0) && (i < pN.size()-1); i++){
      for (unsigned int j = i+1; j < pN.size(); j++){
        if (FSString::TString2string(pN[j]) < FSString::TString2string(pN[i])){
          TString tmp = pN[i];
          pN[i] = pN[j];
          pN[j] = tmp;
        }
      }
    }

      // sort the pM vector
    for (unsigned int i = 0; (pM.size() != 0) && (i < pM.size()-1); i++){
      for (unsigned int j = i+1; j < pM.size(); j++){
        if (FSString::TString2string(pM[j]) < FSString::TString2string(pM[i])){
          TString tmp = pM[i];
          pM[i] = pM[j];
          pM[j] = tmp;
        }
      }
    }

      // hard-code a few special vectors
    if (m_mapDefinedEn.find("B3BEAM") == m_mapDefinedEn.end()){
      defineFourVector("B3BEAM","(2.0*BeamEnergy)","1.0*sin(0.011)*(2.0*BeamEnergy)","0.0","0.0");
      defineFourVector("CCBEAM","(2.0*BeamEnergy)","1.0*sin(-0.003)*(2.0*BeamEnergy)","0.0","0.0");
      defineFourVector("GBEAM","(EnPB+0.938272)","PxPB","PyPB","PzPB");
    }


      // make substitutions in the pN vector
    vector<TString> EnPN;
    vector<TString> PxPN;
    vector<TString> PyPN;
    vector<TString> PzPN;
    for (unsigned int i = 0; i < pN.size(); i++){
      map<TString,TString>::const_iterator mapItr = m_mapDefinedEn.find(pN[i]);
      if (mapItr != m_mapDefinedEn.end()){
        EnPN.push_back(m_mapDefinedEn[pN[i]]);
        PxPN.push_back(m_mapDefinedPx[pN[i]]);
        PyPN.push_back(m_mapDefinedPy[pN[i]]);
        PzPN.push_back(m_mapDefinedPz[pN[i]]);
      }
      else{
        EnPN.push_back(PREFIXMARK+TString("EnP")+pN[i]);
        PxPN.push_back(PREFIXMARK+TString("PxP")+pN[i]);
        PyPN.push_back(PREFIXMARK+TString("PyP")+pN[i]);
        PzPN.push_back(PREFIXMARK+TString("PzP")+pN[i]);
      }
    }

      // make substitutions in the pM vector
    vector<TString> EnPM;
    vector<TString> PxPM;
    vector<TString> PyPM;
    vector<TString> PzPM;
    for (unsigned int i = 0; i < pM.size(); i++){
      map<TString,TString>::const_iterator mapItr = m_mapDefinedEn.find(pM[i]);
      if (mapItr != m_mapDefinedEn.end()){
        EnPM.push_back(m_mapDefinedEn[pM[i]]);
        PxPM.push_back(m_mapDefinedPx[pM[i]]);
        PyPM.push_back(m_mapDefinedPy[pM[i]]);
        PzPM.push_back(m_mapDefinedPz[pM[i]]);
      }
      else{
        EnPM.push_back(PREFIXMARK+TString("EnP")+pM[i]);
        PxPM.push_back(PREFIXMARK+TString("PxP")+pM[i]);
        PyPM.push_back(PREFIXMARK+TString("PyP")+pM[i]);
        PzPM.push_back(PREFIXMARK+TString("PzP")+pM[i]);
      }
    }



    TString operation("");
      // make the sum of N four-vectors
    for (unsigned int i = 0;   i < pN.size(); i++) { if (i == 0) operation = "";
                                                     if (i != 0) operation = "+";
                                                     EnN += operation+EnPN[i];
			  		             PxN += operation+PxPN[i];
					             PyN += operation+PyPN[i];
					             PzN += operation+PzPN[i]; }
      // make the sum of M four-vectors
    for (unsigned int i = 0;   i < pM.size(); i++) { if (i == 0) operation = "";
                                                     if (i != 0) operation = "+";
                                                     EnM += operation+EnPM[i];
			  		             PxM += operation+PxPM[i];
					             PyM += operation+PyPM[i];
					             PzM += operation+PzPM[i]; }
     // make the difference between N and M four-vectors
    EnD = EnN + "-(" + EnM +")";
    PxD = PxN + "-(" + PxM +")";
    PyD = PyN + "-(" + PyM +")";
    PzD = PzN + "-(" + PzM +")";


    TString substitute("");

         if (mark == RECOILMASSMARK){
	   substitute += "(sqrt((" + EnD + ")**2-" +
                               "(" + PxD + ")**2-" +
                               "(" + PyD + ")**2-" +
                               "(" + PzD + ")**2))";
    }
    else if (mark == RECOILMASS2MARK){
	   substitute += "((" + EnD + ")**2-" +
                          "(" + PxD + ")**2-" +
                          "(" + PyD + ")**2-" +
                          "(" + PzD + ")**2)";
    }
    else if (mark == MASSMARK){
      if (pM.size()==0){
	   substitute += "(sqrt((" + EnN + ")**2-" +
                               "(" + PxN + ")**2-" +
                               "(" + PyN + ")**2-" +
                               "(" + PzN + ")**2))";
      }
      else{
	   substitute += "(sqrt((" + EnD + ")**2-" +
                               "(" + PxD + ")**2-" +
                               "(" + PyD + ")**2-" +
                               "(" + PzD + ")**2))";
      }
    }
    else if (mark == MASS2MARK){
      if (pM.size()==0){
	   substitute += "((" + EnN + ")**2-" +
                          "(" + PxN + ")**2-" +
                          "(" + PyN + ")**2-" +
                          "(" + PzN + ")**2)";
      }
      else{
	   substitute += "((" + EnD + ")**2-" +
                          "(" + PxD + ")**2-" +
                          "(" + PyD + ")**2-" +
                          "(" + PzD + ")**2)";
      }
    }
    else if (mark == MOMXMARK){
	   substitute += "(" + PxN + ")";
    }
    else if (mark == MOMYMARK){
	   substitute += "(" + PyN + ")";
    }
    else if (mark == MOMZMARK){
	   substitute += "(" + PzN + ")";
    }
    else if (mark == MOMRMARK){
	   substitute += "(sqrt((" + PxN + ")**2+" +
                               "(" + PyN + ")**2))";
    }
    else if (mark == MOMMARK){
	   substitute += "(sqrt((" + PxN + ")**2+" +
                               "(" + PyN + ")**2+" +
                               "(" + PzN + ")**2))";
    }
    else if (mark == ENMARK){
	   substitute += "(" + EnN + ")";
    }
    else if (mark == DOTPRODUCT){
	   substitute += "((" + PxN + ")*(" + PxM + ")+" +
                          "(" + PyN + ")*(" + PyM + ")+" +
                          "(" + PzN + ")*(" + PzM + "))";
    }
    else if (mark == COSINE){
      if (pM.size()==0){
	   substitute += "((" + PzN + ")/" +
                    "(sqrt((" + PxN + ")**2+" +
                          "(" + PyN + ")**2+" +
                          "(" + PzN + ")**2)))"; 
      }
      else{
	   substitute += "(((" + PxN + ")*(" + PxM + ")+" +
                           "(" + PyN + ")*(" + PyM + ")+" +
                           "(" + PzN + ")*(" + PzM + "))/" +
	                      "(sqrt((" + PxN + ")**2+" +
                                    "(" + PyN + ")**2+" +
                                    "(" + PzN + ")**2))/" + 
	                      "(sqrt((" + PxM + ")**2+" +
                                    "(" + PyM + ")**2+" +
                                    "(" + PzM + ")**2)))";
      }
    }
    else if (mark == HELCOSINE){
         TString pmag = "(sqrt((" + PxM + ")**2+" +
                              "(" + PyM + ")**2+" +
                              "(" + PzM + ")**2))";
         TString bmag = "(sqrt((" + PxN + ")**2+" +
                              "(" + PyN + ")**2+" +
                              "(" + PzN + ")**2))";
         TString Ep   = "(" + EnM + ")";
         TString Eb   = "(" + EnN + ")";
         TString costheta = "(((" + PxN + ")*(" + PxM + ")+" +
                              "(" + PyN + ")*(" + PyM + ")+" +
                              "(" + PzN + ")*(" + PzM + "))/" +
                                 "(" + pmag + "*" + bmag + "))";
         TString sintheta = "(sqrt(1.0-" + costheta + "**2))";
         TString ppar  = "("+pmag+"*"+costheta+")";
         TString pperp = "("+pmag+"*"+sintheta+")";
         TString beta = "(("+bmag+")/("+Eb+"))";
         TString gamma = "(1.0/sqrt(1.0-"+beta+"**2))";
         TString pparp = "("+gamma+"*"+ppar+ "-" +gamma+"*"+beta+"*"+Ep+")";
         TString pmagp = "(sqrt("+pparp+"**2+"+pperp+"**2))";
         substitute += "("+pparp+"/"+pmagp+")";
    }


    variable.Replace(index,size,substitute);

  }

  return variable;

}


    // ********************************************************
    // DEFINE SPECIAL FOUR-VECTORS
    // ********************************************************

void
FSTree::defineFourVector(TString indexName, TString En, TString Px, TString Py, TString Pz){
  map<TString,TString>::const_iterator mapItr = m_mapDefinedEn.find(indexName);
  if (mapItr != m_mapDefinedEn.end()){
    cout << "FSTree WARNING:  overwriting defined four-vector named " << indexName << endl;
  }
  m_mapDefinedEn[indexName] = En;
  m_mapDefinedPx[indexName] = Px;
  m_mapDefinedPy[indexName] = Py;
  m_mapDefinedPz[indexName] = Pz;
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

void
FSTree::clearFileCache(){
  if (FSControl::DEBUG) 
    cout << "FSTree: clearing file cache" << endl;
  for (map<TString,TFile*>::iterator rmItr = m_fileCache.begin();
       rmItr != m_fileCache.end(); rmItr++){
    if (rmItr->second) delete rmItr->second;
  }
  m_fileCache.clear();
  if (FSControl::DEBUG) 
    cout << "FSTree: done clearing file cache" << endl;
}

