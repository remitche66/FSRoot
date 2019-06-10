#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include "TChain.h"
#include "TString.h"
#include "FSData/FSEEDataSet.h"
#include "FSData/FSEEDataSetList.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSHistogram.h"

vector<FSEEDataSet*> FSEEDataSetList::m_vectorDataSets;
map< TString, FSEEDataSet* > FSEEDataSetList::m_mapTempDataSets;


void
FSEEDataSetList::addDataSetsFromFile(TString fileName){
  clearTempDataSets();
  cout << "FSEEDataSetList: Reading data from file: " << fileName << endl;
  ifstream infile(FSString::TString2string(fileName).c_str());
  if (!infile.is_open()){
    cout << "FSEEDataSetList ERROR: cannot find file: " << fileName << endl;
    exit(0);
  }
  string sline;
  while(getline(infile,sline)){
    TString line = FSString::string2TString(sline);
    vector<TString> words = FSString::parseTString(line);
    if (words.size() == 0) continue;
    if (words[0][0] == '#') continue;
    if (words[0] == "data"){
      if (words.size() < 10){
        cout << "FSEEDataSetList ERROR: problem with data command" << endl;
        exit(0);
      }
      TString fsName(words[1]);
        if (getDataSetVector(fsName).size() > 0){
          cout << "FSEEDataSetList ERROR:  using the same data set name twice: " << fsName <<  endl;
          exit(1);
        }
      int fsRunStart(FSString::TString2int(words[2]));
      int fsRunEnd(FSString::TString2int(words[3]));
      TString sEcm(words[4]);
      TString sEcmStatError(words[5]);
      TString sEcmSystError(words[6]);
        bool convertFromMeVToGeV = false;
        bool relativeErrorStat = false;
        bool relativeErrorSyst = false;
        while (sEcm.Contains("M")){
          sEcm.Replace(sEcm.Index("M"),1,"");
          convertFromMeVToGeV = true; }
        while (sEcmStatError.Contains("R")){
          sEcmStatError.Replace(sEcmStatError.Index("R"),1,"");
          relativeErrorStat = true; }
        while (sEcmSystError.Contains("R")){
          sEcmSystError.Replace(sEcmSystError.Index("R"),1,"");
          relativeErrorSyst = true; }
        double fsEcm         (FSString::TString2double(sEcm));
        double fsEcmStatError(FSString::TString2double(sEcmStatError));
        double fsEcmSystError(FSString::TString2double(sEcmSystError));
        if (relativeErrorStat) fsEcmStatError = fsEcmStatError*fsEcm;
        if (relativeErrorSyst) fsEcmSystError = fsEcmSystError*fsEcm;
        if (convertFromMeVToGeV) fsEcm = fsEcm/1000.0;
        if (convertFromMeVToGeV) fsEcmStatError = fsEcmStatError/1000.0;
        if (convertFromMeVToGeV) fsEcmSystError = fsEcmSystError/1000.0;
      double fsLum(FSString::TString2double(words[7]));
      TString sLumStatError(words[8]);
      TString sLumSystError(words[9]);
        relativeErrorStat = false;
        relativeErrorSyst = false;
        while (sLumStatError.Contains("R")){
          sLumStatError.Replace(sLumStatError.Index("R"),1,"");
          relativeErrorStat = true; }
        while (sLumSystError.Contains("R")){
          sLumSystError.Replace(sLumSystError.Index("R"),1,"");
          relativeErrorSyst = true; }
        double fsLumStatError(FSString::TString2double(sLumStatError));
        double fsLumSystError(FSString::TString2double(sLumSystError));
        if (relativeErrorStat) fsLumStatError = fsLumStatError*fsLum;
        if (relativeErrorSyst) fsLumSystError = fsLumSystError*fsLum;
      vector<TString> fsCategories;
        for (unsigned int i = 10; i < words.size(); i++){ fsCategories.push_back(words[i]); }
      FSEEDataSet* fsd = new FSEEDataSet(fsName, fsRunStart, fsRunEnd,
                                     fsEcm, fsEcmStatError, fsEcmSystError,
                                     fsLum, fsLumStatError, fsLumSystError, fsCategories);
      fsd->display();
      m_vectorDataSets.push_back(fsd);
    }
    else if (words[0] == "data0"){
      if (words.size() < 4){
        cout << "FSEEDataSetList ERROR: problem with data0 command" << endl;
        exit(0);
      }
      TString fsName(words[1]);
        if (getDataSetVector(fsName).size() > 0){
          cout << "FSEEDataSetList ERROR:  using the same data set name twice: " << fsName <<  endl;
          exit(1);
        }
      TString sEcm(words[2]);
        bool convertFromMeVToGeV = false;
        while (sEcm.Contains("M")){
          sEcm.Replace(sEcm.Index("M"),1,"");
          convertFromMeVToGeV = true; }
        double fsEcm         (FSString::TString2double(sEcm));
        if (convertFromMeVToGeV) fsEcm = fsEcm/1000.0;
      double fsLum(FSString::TString2double(words[3]));
      vector<TString> fsCategories;
        for (unsigned int i = 4; i < words.size(); i++){ fsCategories.push_back(words[i]); }
      FSEEDataSet* fsd = new FSEEDataSet(fsName, fsEcm, fsLum, fsCategories);
      fsd->display();
      m_vectorDataSets.push_back(fsd);
    }
    else if (words[0] == "dsCategory"){
      if (words.size() < 3){
        cout << "FSEEDataSetList ERROR: problem with dsCategory command" << endl;
        exit(0);
      }
      TString category = words[1];
      for (unsigned int i = 2; i < words.size(); i++){
        vector<FSEEDataSet*> fsdv = getDataSetVector(words[i]);
        for (unsigned int idata = 0; idata < fsdv.size(); idata++){
          fsdv[idata]->addDSCategory(category,false);
        }
      }
    }
    else if (words[0] == "lumCategory"){
      if (words.size() < 3){
        cout << "FSEEDataSetList ERROR: problem with lumCategory command" << endl;
        exit(0);
      }
      TString category = words[1];
      double lumMin = FSString::TString2double(words[2]);
      double lumMax = -1.0;
      if (words.size() > 3) lumMax = FSString::TString2double(words[3]);
      for (unsigned int i = 0; i < FSEEDataSet::m_vectorLumCategories.size(); i++){
        if (FSEEDataSet::m_vectorLumCategories[i].first == category){
          cout << "FSEEDataSetList ERROR: duplicate lumCategory command" << endl;
          exit(0);
        }
      }
      pair<double,double> lumPair(lumMin,lumMax);
      FSEEDataSet::m_vectorLumCategories.push_back(pair<TString, pair<double,double> >(category,lumPair));
      for (unsigned int i = 0; i < m_vectorDataSets.size(); i++){
        m_vectorDataSets[i]->addLUMCategories();
      }
    }
    else{
      cout << "FSEEDataSetList ERROR: problem reading file " << fileName << endl;
      cout << "  line:" << endl << line << endl;  exit(1);
    }
  }
  if (m_vectorDataSets.size() == 0){
    cout << "FSEEDataSetList WARNING:  no points found in file: " << fileName << endl;
  }
}




FSEEDataSet*
FSEEDataSetList::getDataSet(TString dsCategory, TString lumCategory, double ecmGrouping){
  TString index = "(totalDS)" + dsCategory + "(totalLUM)" + lumCategory;
  if (ecmGrouping > 0.0){
    index += "(ecmgroup)" + FSString::double2TString(ecmGrouping,8,true);
  }
  FSEEDataSet* fsd = findTempDataSet(index);
  if (fsd) return fsd;
  vector<FSEEDataSet*> fsdv = getDataSetVector(dsCategory,lumCategory,ecmGrouping);
  if (fsdv.size() == 0) return NULL;
  fsd = new FSEEDataSet();
  TString name("[[composite]");
  for (unsigned int i = 0; i < fsdv.size(); i++){
    fsd->addSubSet(fsdv[i]);
    name += fsdv[i]->name();
    if (i != fsdv.size()-1) name += "+++";
  }
  name += "]";
  fsd->setName(name);
  fsd->addLUMCategories();
  m_mapTempDataSets[index] = fsd;
  return fsd;
}



vector<FSEEDataSet*>
FSEEDataSetList::getDataSetVector(TString dsCategory, TString lumCategory, double ecmGrouping){
    // loop over data sets and keep those that pass the dsCategory logic
  vector<FSEEDataSet*> fsdVectorSelected;
  for (unsigned int i = 0; i < m_vectorDataSets.size(); i++){
    FSEEDataSet* fsd = m_vectorDataSets[i];
    if (FSString::evalLogicalTString(dsCategory,fsd->dsCategories()))
      fsdVectorSelected.push_back(fsd);
  }
    // sort by ecm
  if (fsdVectorSelected.size() > 1){
    for (unsigned int i = 0; i < fsdVectorSelected.size()-1; i++){
    for (unsigned int j = i+1; j < fsdVectorSelected.size(); j++){
      double ecmi = fsdVectorSelected[i]->ecm();
      double ecmj = fsdVectorSelected[j]->ecm();
      TString namei = fsdVectorSelected[i]->name();
      TString namej = fsdVectorSelected[j]->name();
      if ((ecmi > ecmj) || 
          ((ecmi == ecmj) && (namei > namej))){
        FSEEDataSet* tempDataSet = fsdVectorSelected[i];
        fsdVectorSelected[i] = fsdVectorSelected[j];
        fsdVectorSelected[j] = tempDataSet;
      }
    }}
  }
    // do the ecm grouping
  if ((ecmGrouping > 0.0) && (fsdVectorSelected.size() > 1)){
    vector<FSEEDataSet*> fsdVectorGrouped;
    for (unsigned int i = 0; i < fsdVectorSelected.size(); i++){
      TString groupCat = "(" + fsdVectorSelected[i]->name() + ")";
      double ecmi = fsdVectorSelected[i]->ecm();
      int iskip = 0;
      for (unsigned int j = i+1; j < fsdVectorSelected.size(); j++){
        double ecmj = fsdVectorSelected[j]->ecm();
        if ((ecmj - ecmi) < ecmGrouping){
          groupCat += ",(" + fsdVectorSelected[j]->name() + ")";
          iskip++;
        }
        else{ break; }
      }
      FSEEDataSet* fsdGroup = getDataSet(groupCat);
      if (!fsdGroup){ cout << "FSEEDataSetList ERROR" << endl; exit(1); }
      vector<FSEEDataSet*> groupSubSets = fsdGroup->subSets();
      if (groupSubSets.size() == 1){
        fsdVectorGrouped.push_back(groupSubSets[0]);
      }
      else{
        fsdVectorGrouped.push_back(fsdGroup);
      }
      i += iskip;
    }
    fsdVectorSelected = fsdVectorGrouped;
  }
    // now check the lumCategory logic
  vector<FSEEDataSet*> fsdVectorFinal;
  for (unsigned int i = 0; i < fsdVectorSelected.size(); i++){
    FSEEDataSet* fsd = fsdVectorSelected[i];
    if (FSString::evalLogicalTString(lumCategory,fsd->lumCategories()))
      fsdVectorFinal.push_back(fsd);
  }
  // if gaps too big, error, ambiguity?
 return fsdVectorFinal;
}


void 
FSEEDataSetList::display(TString dsCategory, TString lumCategory, double ecmGrouping){
  FSEEDataSet* fsd = getDataSet(dsCategory,lumCategory,ecmGrouping);
  if (!fsd){
    cout << "No data sets found for dsCategory: " << dsCategory << 
                             " and lumCategory: " << lumCategory << endl;
    return;
  }
  vector<FSEEDataSet*> fsdv = fsd->subSets();
  cout << endl << "****** SUB DATA SETS ******" << endl << endl;
  for (unsigned int i = 0; i < fsdv.size(); i++){
    fsdv[i]->display(i+1);
  }
  cout << "****** TOTAL DATA SET ******" << endl << endl;
  fsd->display();
}



FSEEDataSet*
FSEEDataSetList::findTempDataSet(TString index){
  map<TString,FSEEDataSet*>::iterator it = m_mapTempDataSets.find(index);
  if (it != m_mapTempDataSets.end()) return (*it).second;
  FSEEDataSet* fsd = NULL;
  return fsd;
}


void
FSEEDataSetList::clearTempDataSets(){
  for (map<TString,FSEEDataSet*>::iterator mapitr = m_mapTempDataSets.begin();
       mapitr != m_mapTempDataSets.end(); mapitr++){
    if ((*mapitr).second) delete (*mapitr).second;
  }
  m_mapTempDataSets.clear();
}


void
FSEEDataSetList::clearDataSets(){
  clearTempDataSets();
  for (unsigned int i = 0; i < m_vectorDataSets.size(); i++){
    if (m_vectorDataSets[i]) delete m_vectorDataSets[i];
  }
  m_vectorDataSets.clear();
}


TH1F* 
FSEEDataSetList::histLuminosity(TString dsCategory,
                                TString lumCategory, 
                                TString histBounds){
  double ecmGrouping = FSString::parseBoundsBinSizeX(histBounds);
  int nbins = FSString::parseBoundsNBinsX(histBounds);
  double x1 = FSString::parseBoundsLowerX(histBounds);
  double x2 = FSString::parseBoundsUpperX(histBounds);
  TH1F* hist = new TH1F("hist","histLuminosity",nbins,x1,x2);
  hist->SetTitle("Integrated Luminosities");
  hist->SetXTitle("Center-of-Mass Energy  (GeV)");
  hist->SetYTitle("Integrated Luminosity  (pb^{-1})");
  FSEEDataSet* fsds = getDataSet(dsCategory,lumCategory,ecmGrouping);
  if (!fsds) return FSHistogram::getTH1F(hist);
  vector<FSEEDataSet*> vfsds = fsds->subSets(); 
  for (unsigned int i = 0; i < vfsds.size(); i++){
    double ecm = vfsds[i]->ecm();
    double lum = vfsds[i]->lum();
    double elum = vfsds[i]->lumError();
    if (elum < 1.0e-4*lum) elum = 1.0e-4*lum;
    int iecm = 1 + (int)((ecm-hist->GetBinLowEdge(1))/hist->GetBinWidth(1));
    hist->SetBinContent(iecm,lum);
    hist->SetBinError(iecm,elum);
  }
  hist = FSHistogram::getTH1F(hist);
  FSHistogram::setHistogramMaxima(hist);
  return hist;
}

