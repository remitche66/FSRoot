#if !defined(FSMODETREE_H)
#define FSMODETREE_H

#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <map>
#include "TChain.h"
#include "TFile.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "FSBasic/FSControl.h"
#include "FSBasic/FSTree.h"
#include "FSBasic/FSHistogram.h"
#include "FSMode/FSModeCollection.h"

using namespace std;


class FSModeTree{

  public:

      // ********************************************************
      // SKIM A TREE AND OUTPUT TO A DIFFERENT FILE
      // ********************************************************

    static void skimTree(TString fileNameInput, TString ntName, TString category,
                         TString fileNameOutput, TString cuts, 
                	 unsigned int iModeStart = 1, unsigned int iModeEnd = 1000000,
                         TString printCommandFile = "");

    static void skimTree(TString fileNameInput, TString ntName, TString category,
                            TString variable, TString bounds,
                            TString cuts, TString fileNameOutput, 
                	    unsigned int iModeStart = 1, unsigned int iModeEnd = 1000000,
                            TString printCommandFile = "");


      // ********************************************************
      // CREATE A FRIEND TREE
      //    * The output file will be: fileNameInput+"."+friendName
      //       and will include the output tree: treeNameInput+"_"+friendName
      //    * The output tree will contain variables named: friendTreeContents[i].first
      //       with values given by: friendTreeContents[i].second
      //    * Use addFriendTree(friendName) to access the new tree.
      // ********************************************************

    static void createFriendTree(TString fileNameInput, TString treeNameInput, TString category, 
                                 TString friendName, vector< pair<TString,TString> > friendTreeContents);

      // same as above, except only for a single variable named "friendName"

    static void createFriendTree(TString fileNameInput, TString treeNameInput, TString category, 
                                 TString friendName, TString friendVariable);


      // ********************************************************
      // RANK EVENTS ACCORDING TO A GIVEN VARIABLE
      //    1. Entries in the trees given by fileName and ntName are grouped using 
      //        groupVar1 and groupVar2.
      //         NOTE:  Common choices for groupVar1 and groupVar2 are "Run" 
      //                and "Event", but they can be anything (including formulas).
      //                Internally, groupVar1 and groupVar2 are converted to 
      //                integers, so use variables that have unique integer values.
      //    2. Within each group, tree entries are sorted by the value of rankVar.
      //         NOTE:  A common choice for rankVar is the chi2 of a kinematic fit.
      //                This is hardcoded in the createChi2RankingTree method.
      //                A formula can also be used for rankVar.
      //                Internally, rankVar is converted to an integer and the 
      //                ranking is done based on that integer value.  So use a
      //                variable with unique integer values (e.g. "1000*Chi2").
      //    3. The output of this method is a new tree with variables:
      //          [rankVarName] (e.g. "Chi2Rank")
      //                 = the rank of the entry within a channel (1 is best, etc.)
      //                      (if the entry doesn't pass the cuts, the rank is -1)
      //          [rankVarName]Global (e.g. "Chi2RankGlobal")
      //                 = the rank within all of the channels in this category
      //          [rankVarName]Combinations (e.g. "Chi2RankCombinations")
      //                 = the number of combinations within a channel passing the cuts
      //          [rankVarName]CombinationsGlobal (e.g. "Chi2RankCombinationsGlobal")
      //                 = number of combinations within all channels passing the cuts
      //    4. The output tree(s) have the same name as the original tree with an
      //        appended "_[rankVarName]".  
      //       Similarly, the output file(s) have the same name as the original files
      //        with an appended ".[rankVarName]".
      //    5. To use the new trees, add them using FSTree::addFriendTree([rankVarName]).
      // ********************************************************

    static void createRankingTree(TString fileName, TString ntName, TString category, 
                                  TString rankVarName, TString rankVar, TString cuts = "",
                                  TString groupVar1 = "Run", TString groupVar2 = "Event", TString groupVar3 = "",
                                  TString printCommandFile = "");

    static void createChi2RankingTree(TString fileName, TString ntName, TString category, 
                                      TString cuts = "", TString printCommandFile = ""){
      createRankingTree(fileName, ntName, category, "Chi2Rank", "1000*Chi2", 
                        cuts, "Run", "Event", "", printCommandFile);
    }



      // *********************************************************************
      // CREATE A TREE WITH PHASE SPACE EVENTS
      //    Start with the four-vector of the CM: EnPCM, PxPCM, PyPCM, PzPCM.
      //    Generate phase space for the final state specified by modeString.
      // *********************************************************************

    static void createPHSPTree(double EnPCM = 3.097, double PxPCM = 0.0, double PyPCM = 0.0, double PzPCM = 0.0, 
                               TString modeString = "0_111", int nEvents = 10000, 
                               TString fileNameOutput = "PHSPTree_MODECODE.root",  
                               TString ntName = "PHSPTree_MODECODE",
                               vector<double> extraFSParticleMasses = vector<double>());



};


#endif
