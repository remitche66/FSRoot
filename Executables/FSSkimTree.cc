#include <iostream>
#include "FSBasic/FSString.h"
#include "FSBasic/FSTree.h"

using namespace std;

void printUsage();

int main(int argc, char* argv[]){

  if (argc < 5){ 
    cout << "FSSkimTree ERROR: Wrong number of arguments." << endl;
    printUsage();
    exit(0);
  }

  TString fileNameInput = "";
  TString chainName = "";
  TString fileNameOutput = "";
  TString cuts = "";
  vector<TString> friendNames;
  for (int i = 1; i < argc-1; i++){
    TString arg(argv[i]);
    if (arg == "-i")    fileNameInput = argv[i+1];
    if (arg == "-o")    fileNameOutput = argv[i+1];
    if (arg == "-cuts") cuts = argv[i+1];
    if (arg == "-nt")   chainName = argv[i+1];
    if (arg == "-friend")  friendNames.push_back(argv[i+1]);
  }
  if (chainName == "") chainName = FSTree::getTreeNameFromFile(fileNameInput);
  if (chainName == ""){
    cout << "FSSkimTree ERROR: No tree name found." << endl;
    exit(0);
  }

  cout << "********************" << endl;
  cout << "RUNNING FSSkimTree" << endl;
  cout << "********************" << endl;
  cout << "InputFileName:  " << fileNameInput << endl;
  cout << "OutputFileName: " << fileNameOutput << endl;
  cout << "CutString:      " << cuts << endl;
  cout << "TreeName:       " << chainName << endl;
  cout << "Friends:  " << endl;
  for (unsigned int i = 0; i < friendNames.size(); i++){
    FSTree::addFriendTree(friendNames[i]);
    cout << "   " << friendNames[i] << endl;
  }
  cout << "********************" << endl;

  if ((fileNameInput == "") || (fileNameOutput == "")){
    cout << "FSSkimTree ERROR: Wrong input parameters." << endl;
    printUsage();
    exit(0);
  }

  FSTree::skimTree(fileNameInput, chainName, fileNameOutput, cuts);

  cout << "********************" << endl;
  cout << "FINISHED FSSkimTree" << endl;
  cout << "********************" << endl;

}


void printUsage(){
  cout << "USAGE:  FSSkimTree -i InputFileName -o OutputFileName [-cuts CutString] [-nt TreeName]" 
          " [-friend FriendName1] [-friend FriendName2] ... " << endl;
}