#include <iostream>
#include "FSBasic/FSString.h"
#include "FSMode/FSModeTree.h"
#include "FSMode/FSModeInfo.h"

using namespace std;

void printUsage();

int main(int argc, char* argv[]){

  if (argc != 11){ 
    cout << "FSModeSkimTree ERROR: Wrong number of arguments." << endl;
    printUsage();
    exit(0);
  }

  TString fileNameInput = "";
  TString chainName = "";
  TString fileNameOutput = "";
  TString mode = "";
  TString cuts = "";
  for (int i = 1; i <= 9; i++){
    TString arg(argv[i]);
    if (arg == "-i")    fileNameInput = argv[i+1];
    if (arg == "-o")    fileNameOutput = argv[i+1];
    if (arg == "-mode") mode = argv[i+1];
    if (arg == "-cuts") cuts = argv[i+1];
    if (arg == "-nt")   chainName = argv[i+1];
  }

  if ((fileNameInput == "") || (fileNameOutput == "") || (mode == "") || (cuts == "") || (chainName == "")){
    cout << "FSModeSkimTree ERROR: Wrong input parameters." << endl;
    printUsage();
    exit(0);
  }

  cout << "********************" << endl;
  cout << "RUNNING FSModeSkimTree" << endl;
  cout << "********************" << endl;
  cout << "InputFileName:  " << fileNameInput << endl;
  cout << "OutputFileName: " << fileNameOutput << endl;
  cout << "CutString:      " << cuts << endl;
  cout << "TreeName:       " << chainName << endl;
  cout << "Mode:           " << mode << endl;
  cout << "********************" << endl << endl;

  cout << "FSModeSkimTree: mode to skim (make sure this shows the right particles)..." << endl << endl;
  FSModeCollection::addModeInfo(mode);
  FSModeCollection::display();
  cout << endl << endl;

  FSModeTree::skimTree(fileNameInput, chainName, "", fileNameOutput, cuts);

  cout << "********************" << endl;
  cout << "FINISHED FSModeSkimTree" << endl;
  cout << "********************" << endl;

}


void printUsage(){
  cout << "USAGE:  FSModeSkimTree -i InputFileName -o OutputFileName -mode FSCode2_FSCode1 -cuts CutString -nt TreeName" << endl;
}