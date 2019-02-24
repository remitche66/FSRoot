#if !defined(FSTREE_H)
#define FSTREE_H

#include <utility>
#include <vector>
#include <string>
#include <map>
#include "TChain.h"
#include "TChainElement.h"
#include "TFile.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"

using namespace std;


class FSTree{

  public:

      // ********************************************************
      // SET UP A TCHAIN AND CACHE IT
      // ********************************************************

    static TChain* getTChain(TString fileName, TString ntName, 
                             bool addFilesIndividually = true);


      // ********************************************************
      // SET UP A TFILE AND CACHE IT
      // ********************************************************

    static TFile* getTFile(TString fileName);


      // ********************************************************
      // SKIM A TREE AND OUTPUT TO A DIFFERENT FILE
      // ********************************************************

    static void skimTree(TString fileNameInput, TString chainName, 
                         TString fileNameOutput, TString cuts, 
                         TString newChainName = "");


      // ********************************************************
      // EXPAND VARIABLE MACROS
      //   For example:
      //     "XXMASS(2,3)"
      //   --> "(sqrt((XXEnP2+XXEnP3)*(XXEnP2+XXEnP3)
      //             -(XXPxP2+XXPxP3)*(XXPxP2+XXPxP3)
      //             -(XXPyP2+XXPyP3)*(XXPyP2+XXPyP3)
      //             -(XXPzP2+XXPzP3)*(XXPzP2+XXPzP3)))"
      // ********************************************************

    static TString expandVariable(TString variable);


      // ********************************************************
      // CLEAR GLOBAL CACHES
      // ********************************************************

    static void clearChainCache();
    static void clearFileCache();


  private:

      // global caches

    static map< TString, TChain*> m_chainCache;
    static map< TString, TFile*> m_fileCache;


};



#endif
