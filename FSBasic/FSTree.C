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
                               TString fileNameOutput, TString cuts, TString newChainName){

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
         variable.Contains(COSINE)){

    int index = 0;
    int size = 0;
    bool hasprefixmark = false;
    TString EnN("");  TString EnM("");  TString En("");
    TString PxN("");  TString PxM("");	TString Px("");
    TString PyN("");  TString PyM("");	TString Py("");
    TString PzN("");  TString PzM("");	TString Pz("");
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
    else if (variable.Contains(COSINE))          mark = COSINE;

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
	   hasprefixmark = true;
    }
    else if (variable.Contains(mark)){
	   index = variable.Index(mark);
	   size = (mark).Length()+1;
    }

    vector<TString> pN;
    vector<TString> pM;
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
      }
    }
    if (pIndex.Length() > 0 && !usevectorM) pN.push_back(pIndex);
    if (pIndex.Length() > 0 &&  usevectorM) pM.push_back(pIndex);


    TString CROSSINGANGLE("-0.003");
    if (pN[0] == "B3BEAM") CROSSINGANGLE = "0.011";
    if (pN[0] == "CCBEAM") CROSSINGANGLE = "-0.003";
    unsigned int ipN = 0;
    if (pN[0] == "P1S")     pN[0] = "3.096916";
    if (pN[0] == "P2S")     pN[0] = "3.68609";
    if (pN[0] == "P3770")   pN[0] = "3.774";
    if (pN[0] == "Y1S")     pN[0] = "9.4603";
    if (pN[0] == "Y2S")     pN[0] = "10.02326";
    if (pN[0] == "Y3S")     pN[0] = "10.3552";
    if (pN[0] == "BEAM")    pN[0] = "(2.0*BeamEnergy)";
    if (pN[0] == "B3BEAM")  pN[0] = "(2.0*BeamEnergy)";
    if (pN[0] == "CCBEAM")  pN[0] = "(2.0*BeamEnergy)";
    if (pN[0].Contains(".")){
      EnN += pN[0];		             
      PxN += "1.0*sin("+CROSSINGANGLE+")*"+pN[0];
      PyN += "0.0";
      PzN += "0.0";
      ipN++;
    }
    //if (pN[0] == "GBEAM"){
    //  EnN += "(BeamEnergy+0.938)";		             
    //  PxN += "0.0";
    //  PyN += "0.0";
    //  PzN += "(BeamEnergy)";
    //  ipN++;
    //}
    if (pN[0] == "GBEAM"){
      EnN += "(EnPB+0.938272)";		             
      PxN += "PxPB";
      PyN += "PyPB";
      PzN += "PzPB";
      ipN++;
    }
    if (pN[0] == "RGBEAM"){
      EnN += "(REnPB+0.938272)";		             
      PxN += "RPxPB";
      PyN += "RPyPB";
      PzN += "RPzPB";
      ipN++;
    }


      // sort the pN vector
    for (unsigned int i = ipN; (pN.size() != 0) && (i < pN.size()-1); i++){
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


    TString pre("");
    TString post("");
    TString operation("");
    if (hasprefixmark)   pre =  PREFIXMARK;
    if (pre == "TR") pre = "MCDecayParticle";
    for (unsigned int i = ipN; i < pN.size(); i++) { if (i == 0) operation = "";
                                                     if (i != 0) operation = "+";
                                                     EnN += operation+pre+"EnP"+pN[i]+post;
			  		             PxN += operation+pre+"PxP"+pN[i]+post;
					             PyN += operation+pre+"PyP"+pN[i]+post;
					             PzN += operation+pre+"PzP"+pN[i]+post; }
    for (unsigned int i = 0;   i < pM.size(); i++) { if (i == 0) operation = "";
                                                     if (i != 0) operation = "+";
                                                     EnM += operation+pre+"EnP"+pM[i]+post;
			  		             PxM += operation+pre+"PxP"+pM[i]+post;
					             PyM += operation+pre+"PyP"+pM[i]+post;
					             PzM += operation+pre+"PzP"+pM[i]+post; }


    TString substitute("");

         if (mark == RECOILMASSMARK){
           En = EnN + "-(" + EnM +")";
           Px = PxN + "-(" + PxM +")";
           Py = PyN + "-(" + PyM +")";
           Pz = PzN + "-(" + PzM +")";
	   //substitute += "(sqrt((" + En + ")*(" + En + ")-" +
           //                    "(" + Px + ")*(" + Px + ")-" +
           //                    "(" + Py + ")*(" + Py + ")-" +
           //                    "(" + Pz + ")*(" + Pz + ")))";
	   substitute += "(sqrt((" + En + ")**2-" +
                               "(" + Px + ")**2-" +
                               "(" + Py + ")**2-" +
                               "(" + Pz + ")**2))";
    }
    else if (mark == RECOILMASS2MARK){
           En = EnN + "-(" + EnM +")";
           Px = PxN + "-(" + PxM +")";
           Py = PyN + "-(" + PyM +")";
           Pz = PzN + "-(" + PzM +")";
	   //substitute += "((" + En + ")*(" + En + ")-" +
           //               "(" + Px + ")*(" + Px + ")-" +
           //               "(" + Py + ")*(" + Py + ")-" +
           //               "(" + Pz + ")*(" + Pz + "))";
	   substitute += "((" + En + ")**2-" +
                          "(" + Px + ")**2-" +
                          "(" + Py + ")**2-" +
                          "(" + Pz + ")**2)";
    }
    else if (mark == MASSMARK){
	   //substitute += "(sqrt((" + EnN + ")*(" + EnN + ")-" +
           //                    "(" + PxN + ")*(" + PxN + ")-" +
           //                    "(" + PyN + ")*(" + PyN + ")-" +
           //                    "(" + PzN + ")*(" + PzN + ")))";
	   substitute += "(sqrt((" + EnN + ")**2-" +
                               "(" + PxN + ")**2-" +
                               "(" + PyN + ")**2-" +
                               "(" + PzN + ")**2))";
    }
    else if (mark == MASS2MARK){
	   //substitute += "((" + EnN + ")*(" + EnN + ")-" +
           //               "(" + PxN + ")*(" + PxN + ")-" +
           //               "(" + PyN + ")*(" + PyN + ")-" +
           //               "(" + PzN + ")*(" + PzN + "))";
	   substitute += "((" + EnN + ")**2-" +
                          "(" + PxN + ")**2-" +
                          "(" + PyN + ")**2-" +
                          "(" + PzN + ")**2)";
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
	   //substitute += "(sqrt((" + PxN + ")*(" + PxN + ")+" +
           //                    "(" + PyN + ")*(" + PyN + ")+" +
           //                    "(" + PzN + ")*(" + PzN + ")))";
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
	   //substitute += "((" + PzN + ")/" +
           //         "(sqrt((" + PxN + ")*(" + PxN + ")+" +
           //               "(" + PyN + ")*(" + PyN + ")+" +
           //               "(" + PzN + ")*(" + PzN + "))))"; 
	   substitute += "((" + PzN + ")/" +
                    "(sqrt((" + PxN + ")**2+" +
                          "(" + PyN + ")**2+" +
                          "(" + PzN + ")**2)))"; 
      }
      else{
	   //substitute += "(((" + PxN + ")*(" + PxM + ")+" +
           //                "(" + PyN + ")*(" + PyM + ")+" +
           //                "(" + PzN + ")*(" + PzM + "))/" +
	   //                   "(sqrt((" + PxN + ")*(" + PxN + ")+" +
           //                         "(" + PyN + ")*(" + PyN + ")+" +
           //                         "(" + PzN + ")*(" + PzN + ")))/" + 
	   //                   "(sqrt((" + PxM + ")*(" + PxM + ")+" +
           //                         "(" + PyM + ")*(" + PyM + ")+" +
           //                         "(" + PzM + ")*(" + PzM + "))))";
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

    variable.Replace(index,size,substitute);

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

