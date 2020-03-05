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

    static TChain* getTChain(TString fileName, TString ntName); 
    static TChain* getTChain(TString fileName, TString ntName, TString& STATUS); 


      // ********************************************************
      // SET UP A TFILE AND CACHE IT
      // ********************************************************

    static TFile* getTFile(TString fileName);


      // ********************************************************
      // SKIM A TREE AND OUTPUT TO A DIFFERENT FILE
      // ********************************************************

    static void skimTree(TString fileNameInput, TString chainName, 
                         TString fileNameOutput, TString cuts, 
                         TString newChainName = "", 
                         TString printCommandFile = "");


      // ********************************************************
      // EXPAND VARIABLE MACROS
      //   For example:
      //     FSTree::expandVariable("1+XXMASS(2,3)")
      //   --> "1+(sqrt((XXEnP2+XXEnP3)**2
      //               -(XXPxP2+XXPxP3)**2
      //               -(XXPyP2+XXPyP3)**2
      //               -(XXPzP2+XXPzP3)**2))"
      // ********************************************************

    static TString reorderVariable(TString variable, bool show = false);
    static TString expandVariable(TString variable, bool show = false);


      // ********************************************************
      // DEFINE AND SHOW VARIABLE MACROS
      // ********************************************************

    static void defineMacro(TString macroName, int numFourVectors, TString macro);
    static void showDefinedMacros();
    static void testMacro(TString variable){ cout << expandVariable(variable,true) << endl; }


      // ********************************************************
      // DEFINE SPECIAL FOUR-VECTORS
      //   These are used by expandVariable.
      //   For example:
      //     FSTree::defineFourVector("SPECIAL","spe","spx","spy","spz")
      //   then 
      //     FSTree::expandVariable("1+XXMASS(SPECIAL,2,3)")
      //   --> "1+(sqrt((spe+XXEnP2+XXEnP3)**2
      //               -(spx+XXPxP2+XXPxP3)**2
      //               -(spy+XXPyP2+XXPyP3)**2
      //               -(spz+XXPzP2+XXPzP3)**2))"
      // ********************************************************

    static void defineFourVector(TString indexName, TString En, TString Px, TString Py, TString Pz);
    static void showDefinedFourVectors();
  

      // ********************************************************
      // CLEAR GLOBAL CACHES
      // ********************************************************

    static void clearChainCache();
    static void clearFileCache();


  private:

    static vector< vector<TString> > parseVariable(TString variable, bool show = false);
    static void makeStandardDefinitions();
    static bool m_madeStandardDefinitions;

      // global caches

    static map< TString, TChain*> m_chainCache;
    static map< TString, TFile*> m_fileCache;

      // defined four-vectors

    static map< TString, TString > m_mapDefinedPx;
    static map< TString, TString > m_mapDefinedPy;
    static map< TString, TString > m_mapDefinedPz;
    static map< TString, TString > m_mapDefinedEn;

      // defined macros

    static map< TString, TString > m_mapDefined1VMacros;
    static map< TString, TString > m_mapDefined2VMacros;
    static vector< TString > m_vectorDefinedMacroKeywords;

};



#endif
