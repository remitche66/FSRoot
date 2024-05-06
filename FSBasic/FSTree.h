#if !defined(FSTREE_H)
#define FSTREE_H

#include <iostream>
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


      // *********************************************************
      // ADD OR REMOVE FRIEND TREES FROM A LIST OF FRIEND TREES
      //   When using getTChain: 
      //     * the friend tree is named ntName_friendName
      //     * the corresponding file is named fileName.friendName
      // *********************************************************

    static void addFriendTree(TString friendName, bool use = true);
    static void removeFriendTree(TString friendName = "*");
    static void useFriendTree(TString friendName = "*", bool use = true);
    static void useFriendTrees(bool use = true){ m_useFriendTrees = use; }
    static vector<TString> getFriendNames(int used = 0); // (-1,0,1) = (unused,all,used)
    static void showFriendTrees();


      // ********************************************************
      // GET NAMES OF OBJECTS FROM FILES AND BRANCH NAMES
      // ********************************************************

    static TString getTreeNameFromFile(TString fileName, TString match = "");
    static vector<TString> getTObjNamesFromFile(TString fileName, TString objType = "TTree",
                                                TString match = "", bool show = false);
    static vector<TString> getBranchNamesFromTree(TString fileName, TString ntName, 
                                                TString match = "", bool show = false);


      // ********************************************************
      // SKIM A TREE AND OUTPUT TO A DIFFERENT FILE
      // ********************************************************

    static void skimTree(TString fileNameInput, TString chainName, 
                         TString fileNameOutput, TString cuts, 
                         TString newChainName = "", 
                         TString printCommandFile = "");


      // ********************************************************
      // CREATE A FRIEND TREE
      //    * The output file will be: fileNameInput+"."+friendName
      //       and will include the output tree: treeNameInput+"_"+friendName
      //    * The output tree will contain variables named: friendTreeContents[i].first
      //       with values given by: friendTreeContents[i].second
      //    * Use addFriendTree(friendName) to access the new tree.
      // ********************************************************

    static void createFriendTree(TString fileNameInput, TString treeNameInput,  TString friendName,
                                  vector< pair<TString,TString> > friendTreeContents);

      // same as above, except only for a single variable named "friendName"

    static void createFriendTree(TString fileNameInput, TString treeNameInput,  TString friendName,
                                  TString friendVariable);

      // ********************************************************
      // EXPAND VARIABLE MACROS
      //   For example:
      //     FSTree::expandVariable("1+XXMASS(2,3)")
      //   --> "1+(sqrt((XXEnP2+XXEnP3)**2
      //               -(XXPxP2+XXPxP3)**2
      //               -(XXPyP2+XXPyP3)**2
      //               -(XXPzP2+XXPzP3)**2))"
      // ********************************************************

    static TString expandVariable(TString variable, bool show = false);
    static TString reorderVariable(TString variable, bool show = false);


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


    friend class FSHistogram;

  private:

    static pair<int,TString> processMaxEntries(TString input);

    static vector< vector<TString> > parseVariable(TString variable, bool show = false);
    static void makeStandardDefinitions();
    static bool m_madeStandardDefinitions;

      // global caches

    static map< TString, TChain*> m_chainCache;
    static vector< pair<TString,bool> > m_friendTrees;
    static bool m_useFriendTrees;

      // defined four-vectors

    static map< TString, TString > m_mapDefinedPx;
    static map< TString, TString > m_mapDefinedPy;
    static map< TString, TString > m_mapDefinedPz;
    static map< TString, TString > m_mapDefinedEn;

      // defined macros

    static map< pair<TString,int>, TString > m_mapDefinedMacros;
    static vector< TString > m_vectorDefinedMacroKeywords;

};



#endif
