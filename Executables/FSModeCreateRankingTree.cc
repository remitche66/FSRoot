#include <iostream>
#include "FSBasic/FSString.h"
#include "FSMode/FSModeTree.h"
#include "FSMode/FSModeInfo.h"

using namespace std;

void printUsage();

int main(int argc, char* argv[]){

  if (argc < 5){ 
    cout << "FSModeCreateRankingTree ERROR: Wrong number of arguments." << endl;
    printUsage();
    exit(0);
  }

  TString fileNameInput = "";
  TString chainName = "";
  vector<TString> modes;
  TString rankVarName = "";
  TString rankVar = "";
  TString cuts = "";
  TString groupVar1 = "Run";
  TString groupVar2 = "Event";
  vector<TString> friendNames;
  for (int i = 1; i < argc-1; i++){
    TString arg(argv[i]);
    if (arg == "-i")    fileNameInput = argv[i+1];
    if (arg == "-nt")   chainName = argv[i+1];
    if (arg == "-mode") modes.push_back(argv[i+1]);
    if (arg == "-rvname") rankVarName = argv[i+1];
    if (arg == "-rv") rankVar = argv[i+1];
    if (arg == "-cuts") cuts = argv[i+1];
    if (arg == "-gv1") groupVar1 = argv[i+1];
    if (arg == "-gv2") groupVar2 = argv[i+1];
    if (arg == "-friend")  friendNames.push_back(argv[i+1]);
  }

  cout << "********************" << endl;
  cout << "RUNNING FSModeCreateRankingTree" << endl;
  cout << "********************" << endl;
  cout << "InputFileName:  " << fileNameInput << endl;
  cout << "TreeName:       " << chainName << endl;
  cout << "RankVarName:    " << rankVarName << endl;
  cout << "VarName:        " << rankVar << endl;
  cout << "CutString:      " << cuts << endl;
  cout << "GroupVar1:      " << groupVar1 << endl;
  cout << "GroupVar2:      " << groupVar2 << endl;
  cout << "Friends:  " << endl;
  for (unsigned int i = 0; i < friendNames.size(); i++){
    FSTree::addFriendTree(friendNames[i]);
    cout << "   " << friendNames[i] << endl;
  }
  cout << "Modes:          " << endl;
  for (unsigned int imode = 0; imode < modes.size(); imode++){
    FSModeInfo* mi = FSModeCollection::addModeInfo(modes[imode]);
    cout << "  " << mi->modeDescription() << endl;
  }
  cout << "********************" << endl << endl;

  if ((fileNameInput == "") || (chainName == "") || (modes.size() == 0)
      || (rankVarName == "") || (rankVar == "") || (groupVar1 == "")
      || (groupVar2 == "")){
    cout << "FSModeCreateRankingTree ERROR: Missing arguments." << endl;
    printUsage();
    exit(0);
  }

  cout << "FSModeCreateRankingTree:  LIST OF MODES" << endl;
  FSModeCollection::display();

  FSModeTree::createRankingTree(fileNameInput, chainName, "", 
                                  rankVarName, rankVar, cuts,
                                  groupVar1, groupVar2);

  cout << "********************" << endl;
  cout << "FINISHED FSModeCreateRankingTree" << endl;
  cout << "********************" << endl;

}


void printUsage(){
  cout << 
     " USAGE:  FSModeCreateRankingTree  \n"
     "           -i       InputFileName \n"
     "           -nt      TreeName  \n"
     "           -mode    FSCode2_FSCode1  (can use many of these) \n"
     "           -rvname  RankVarName \n"
     "           -rv      RankVar \n"
     "          [-cuts    CutString]       (default = \"\") \n"
     "          [-gv1     GroupVar1]       (default = \"Run\") \n"
     "          [-gv2     GroupVar2]       (default = \"Event\") \n"
     "          [-friend  FriendName]      (can use many of these) \n"
  << endl;

}

