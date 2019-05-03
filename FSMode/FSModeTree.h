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
      // RANK EVENTS ACCORDING TO Chi2DOF
      //    To Use:
      //      1.  First create skims of events that are to be ranked.
      //             (For better organization, put these in a new directory.)
      //      2.  Run createChi2Friends on those events.
      //             New friend trees are created (in the same directory) with the variables:
      //                NCombinations:  number of combinations within a final state
      //                Chi2Rank: rank of this combination within a final state
      //                NCombinationsGlobal:  number of combinations in all final states
      //                Chi2RankGlobal:  rank of this combination in all final states
      //      3.  To access these new variables, set FSControl::CHAINFRIEND = "chi"
      // ********************************************************

    static void createChi2Friends(TString fileName, TString ntName, TString category, bool mc = false);


};


#endif
