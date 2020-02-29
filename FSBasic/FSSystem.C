#include <iostream>
#include "TSystem.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSSystem.h"



  // **********************************
  //   MAKE AN ABSOLUTE PATH NAME
  // **********************************

TString
FSSystem::makeAbsolutePathName(TString path){
  TString newPath = path;
  if (newPath == "") newPath = ".";
  newPath = gSystem->ExpandPathName(FSString::TString2string(newPath).c_str());
  if (newPath == ""){
    cout << "FSSystem::makeAbsolutePathName: WARNING undefined environment variable in: "
         << endl << "  " << path << endl;
    return TString("");
  }
  if (!gSystem->IsAbsoluteFileName(newPath))
    newPath = gSystem->PrependPathName(gSystem->GetWorkingDirectory().c_str(),newPath);
  return newPath;
}

  // **********************************
  //   GET AN ABSOLUTE PATH TO A FILE OR DIRECTORY
  // **********************************

TString
FSSystem::getAbsolutePath(TString path){
  path = makeAbsolutePathName(path); if (path == "") return TString("");
  if (!(path.Contains("*")||path.Contains("?"))){
    if (gSystem->AccessPathName(path)) return TString("");
  }
  else{
    if (getAbsolutePaths(path).size() == 0) return TString("");
  }
  return path;
}

  // **********************************
  //   GET A VECTOR OF ABSOLUTE PATHS TO FILES OR DIRECTORIES
  // **********************************

vector<TString>
FSSystem::getAbsolutePaths(TString path, bool show){
  vector<TString> paths;
  if (!(path.Contains("*") || path.Contains("?"))){
    path = getAbsolutePath(path);
    if (path != "") paths.push_back(path);
  }
  else{
    paths = getAbsolutePathsWildcards(path);
  }
  if (show){
    cout << "------ ABSOLUTE PATHS ---------" << endl;
    for (unsigned int i = 0; i < paths.size(); i++){
      cout << "  (" << i+1 << "):  " << paths[i] << endl;
    }
    cout << "-------------------------------" << endl;
  }
  return paths;
}


  // ****************************
  //   HELPER FUNCTIONS
  // ****************************

vector<TString>
FSSystem::getDirectoryContents(TString directory, TString pattern, bool show){
  directory = makeAbsolutePathName(directory);
  if (directory == "") return vector<TString>();
  if (pattern == "") pattern = "*";
  vector<TString> paths;
  TSystemDirectory dir("",directory);
  TList *files = dir.GetListOfFiles();
  if (files){
    TSystemFile *file;
    TIter next(files);
    while ((file=(TSystemFile*)next())){
      TString foundName = file->GetName();
      if ((foundName != ".") && (foundName != "..") && 
          FSString::compareTStrings(foundName,pattern))
        paths.push_back(foundName);
    }
  }
  for (unsigned int i = 0; i < paths.size(); i++){
    paths[i] = gSystem->PrependPathName(directory,paths[i]);
  }
  if (show){
    cout << "------ DIRECTORY CONTENTS ---------" << endl;
    for (unsigned int i = 0; i < paths.size(); i++){
      cout << "  (" << i+1 << "):  " << paths[i] << endl;
    }
    cout << "-----------------------------------" << endl;
  }
  return paths;
}


vector<TString>
FSSystem::getDirectoryStructure(TString path, bool show){
  path = makeAbsolutePathName(path);
  if (path == "") return vector<TString>();
  if (FSString::string2TString(gSystem->BaseName(path)) == "") path += ".";
  vector<TString> parts;
  TString directory = path;
  TString fileName = "";
  while (directory != fileName){
    fileName = gSystem->BaseName(directory);
    directory = gSystem->DirName(directory);
    parts.push_back(fileName);
  }
  if (show){
    cout << "------ DIRECTORY STRUCTURE ---------" << endl;
    for (unsigned int i = 0; i < parts.size(); i++){
      cout << "  (" << i+1 << "):  " << parts[i] << endl;
    }
    cout << "------------------------------------" << endl;
  }
  return parts;
}


vector<TString>
FSSystem::getAbsolutePathsWildcards(TString path, bool show){
  vector<TString> partsTmp = getDirectoryStructure(path,show);
  if (partsTmp.size() == 0) return vector<TString>();
  vector<TString> parts;
  for (int i = partsTmp.size()-1; i >= 0; i--){
    if (FSString::removeWhiteSpace(partsTmp[i]) != "") parts.push_back(partsTmp[i]);
  }
  if (parts.size() == 0) return vector<TString>();
  if (parts[0].Contains("*") || parts[0].Contains("?") || parts.size() < 2){
    cout << "FSSystem::getAbsolutePathsWildcards WARNING: bad path: " << path;
    return vector<TString>();
  }
  vector<TString> paths;
  paths.push_back(parts[0]);
  for (unsigned int i = 1; i < parts.size(); i++){
    if (parts[i].Contains("*") || parts[i].Contains("?")){
      vector<TString> pathsTmp1;
      for (unsigned int j = 0; j < paths.size(); j++){
        vector<TString> pathsTmp2 = getDirectoryContents(paths[j],parts[i]);
        for (unsigned int k = 0; k < pathsTmp2.size(); k++){
          pathsTmp1.push_back(pathsTmp2[k]); }
      }
      paths = pathsTmp1;
    }
    else{
      for (unsigned int j = 0; j < paths.size(); j++){
        paths[j] = gSystem->PrependPathName(paths[j],parts[i]);
      }
    }
  }
  if (show){
    cout << "------ ABSOLUTE PATHS WILDCARDS ---------" << endl;
    for (unsigned int i = 0; i < paths.size(); i++){
      cout << "  (" << i+1 << "):  " << paths[i] << endl;
    }
    cout << "-----------------------------------------" << endl;
  }
  return paths;
}

