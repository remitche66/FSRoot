#include <iostream>
#include <fstream>
#include "TSystem.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TFile.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSSystem.h"


map< TString, TString > FSSystem::m_cachePath;
map< TString, vector<TString> > FSSystem::m_cachePaths;
map< TString, bool > FSSystem::m_cacheRoot;


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
  if (!gSystem->IsAbsoluteFileName(newPath)){
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,18,0)
    TString tempPath = newPath;
    newPath = gSystem->PrependPathName(gSystem->GetWorkingDirectory().c_str(),tempPath);
#else
    TString tempDir = "$PWD";
    TString tempPath = newPath;
    newPath = makeAbsolutePathName(gSystem->PrependPathName(tempDir,tempPath));
    if (newPath == ""){
      cout << "makeAbsolutePathName(" << path << ")" << endl;
      cout << "ROOT version too old: no relative paths allowed" << endl;
      exit(0);
    }
#endif
  }
  return newPath;
}

  // **********************************
  //   GET AN ABSOLUTE PATH TO A FILE OR DIRECTORY
  // **********************************

TString
FSSystem::getAbsolutePath(TString path, bool useCache){
  if (useCache){ if (m_cachePath.find(path) != m_cachePath.end()) return m_cachePath[path]; }
  TString newPath = path;
  newPath = makeAbsolutePathName(newPath);
  if (newPath == ""){}
  else if (!(newPath.Contains("*")||newPath.Contains("?")||newPath.Contains(",")||newPath.Contains(" "))){
    if (gSystem->AccessPathName(newPath)) newPath = "";
  }
  else{
    if (getAbsolutePaths(newPath,useCache).size() == 0) newPath = "";
  }
  if (useCache) m_cachePath[path] = newPath;
  return newPath;
}

  // **********************************
  //   GET A VECTOR OF ABSOLUTE PATHS TO FILES OR DIRECTORIES
  // **********************************

vector<TString>
FSSystem::getAbsolutePaths(TString path, bool useCache, bool show){
  if (useCache){ if (m_cachePaths.find(path) != m_cachePaths.end()) return m_cachePaths[path]; }
  if (path.Contains(" ") || path.Contains(",")){
    vector<TString> allPaths;
    vector<TString> spacers; spacers.push_back(" "); spacers.push_back(",");
    vector<TString> multiplePaths = FSString::parseTString(path,spacers);
    if (show) cout << "----- FOUND " << multiplePaths.size() << " SETS OF PATHS ---" << endl;
    for (unsigned int i = 0; i < multiplePaths.size(); i++){
      vector<TString> paths = getAbsolutePaths(multiplePaths[i],useCache,show);
      for (unsigned int j = 0; j < paths.size(); j++){ allPaths.push_back(paths[j]); }
    }
    return allPaths;
  }
  TString newPath = path;
  vector<TString> paths;
  if (!(newPath.Contains("*") || newPath.Contains("?"))){
    newPath = getAbsolutePath(newPath);
    if (newPath != "") paths.push_back(newPath);
  }
  else{
    paths = getAbsolutePathsWildcards(newPath);
  }
  if (paths.size() > 1){
  for (unsigned int i = 0; i < paths.size()-1; i++){
  for (unsigned int j = i+1; j < paths.size(); j++){
    if (paths[i] > paths[j]){
      TString temp = paths[i];
      paths[i] = paths[j];
      paths[j] = temp;
    }
  }}}
  if (show){
    cout << "------ ABSOLUTE PATHS ---------" << endl;
    for (unsigned int i = 0; i < paths.size(); i++){
      cout << "  (" << i+1 << "):  " << paths[i] << endl;
    }
    cout << "-------------------------------" << endl;
  }
  if (useCache) m_cachePaths[path] = paths;
  return paths;
}


  // **********************************
  //   CHECK IF PATH POINTS TO A PROPER ROOT FILE (OR FILES)
  // **********************************

bool
FSSystem::checkRootFormat(TString path, bool useCache){
  if (useCache){ if (m_cacheRoot.find(path) != m_cacheRoot.end()) return m_cacheRoot[path]; }
  vector<TString> newPaths = getAbsolutePaths(path,useCache);
  bool rootFormat = true;
  if (newPaths.size() == 0) rootFormat = false;
  for (unsigned int i = 0; rootFormat && i < newPaths.size(); i++){
    TString newPath = newPaths[i];
    if (newPath == ""){ rootFormat = false; break; }
    ifstream infile(newPath.Data()); string instring;
    if (!getline(infile,instring)){ infile.close(); rootFormat = false; break; }
    TString inTString(FSString::string2TString(instring));
    if (!inTString.Contains("root")){ infile.close(); rootFormat = false; break; }
    if (inTString.Index("root") != 0){ infile.close(); rootFormat = false; break; }
    if (!getline(infile,instring)){ infile.close(); rootFormat = false; break; }
    infile.close(); 
    //if (objectName != ""){
      //TFile tfile(newPath); tfile.cd(); if (!tfile.FindObjectAny(objectName)) return false; }
  }
  if (useCache) m_cacheRoot[path] = rootFormat;
  return rootFormat;
}


  // **********************************
  //   REPLACE THE BASE OF A PATH WITH A NEW BASE
  // **********************************

TString 
FSSystem::replaceBaseName(TString newBaseName, TString oldPath, bool useCache){
  newBaseName = makeAbsolutePathName(newBaseName);
  oldPath = makeAbsolutePathName(oldPath);
  vector<TString> oldPathParts = FSString::parseTString(oldPath,"/");
  for (unsigned int i = 0; i < oldPathParts.size(); i++){
    TString checkPath = newBaseName;
    for (unsigned int j = i; j < oldPathParts.size(); j++){
      checkPath += "/";  checkPath += oldPathParts[j];
    }
    checkPath = getAbsolutePath(checkPath,useCache);
    if (checkPath != "") return checkPath;
  }
  return TString("");
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
    TString tempPath = paths[i];
    paths[i] = gSystem->PrependPathName(directory,tempPath);
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
  if (!(path.Contains("*") || path.Contains("?"))){
    cout << "getAbsolutePathsWildcards ERROR:  only use this if there are wildcards" << endl;
    exit(0);
  }
  vector<TString> partsTmp = getDirectoryStructure(path,show);
  if (partsTmp.size() == 0) return vector<TString>();
  vector<TString> parts;
  for (int i = partsTmp.size()-1; i >= 0; i--){
    if (FSString::removeWhiteSpace(partsTmp[i]) != "") parts.push_back(partsTmp[i]);
  }
  if (parts.size() == 0) return vector<TString>();
  if (parts[0].Contains("*") || parts[0].Contains("?") || parts.size() < 2){
    cout << "FSSystem::getAbsolutePathsWildcards WARNING: bad path: " << path << endl;
    return vector<TString>();
  }
  vector<TString> paths;
  paths.push_back(parts[0]);
  bool foundWildcards = false;
  for (unsigned int i = 1; i < parts.size(); i++){
    if (parts[i].Contains("*") || parts[i].Contains("?") || foundWildcards){
      foundWildcards = true;
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
        TString tempPath = paths[j];
        TString tempPart = parts[i];
        paths[j] = gSystem->PrependPathName(tempPath,tempPart);
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


TString
FSSystem::getHostName(){
  TString hostName = gSystem->HostName();
  return hostName;
}


