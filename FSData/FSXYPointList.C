#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include "TChain.h"
#include "TString.h"
#include "FSData/FSXYPoint.h"
#include "FSData/FSXYPointList.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSHistogram.h"

vector<FSXYPoint*> FSXYPointList::m_vectorXYPoints;



void
FSXYPointList::addXYPointsFromFile(TString fileName){
  cout << "FSXYPointList: Reading data from file: " << fileName << endl;
  ifstream infile(FSString::TString2string(fileName).c_str());
  if (!infile.is_open()){
    cout << "FSXYPointList ERROR: cannot find file: " << fileName << endl;
    exit(0);
  }
  string sline;
  while(getline(infile,sline)){
    TString line = FSString::string2TString(sline);
    vector<TString> words = FSString::parseTString(line);
    if (words.size() == 0) continue;
    if (words[0][0] == '#') continue;
    if (words[0] == "data"){
/*
      if (words.size() < 10){
        cout << "FSXYPointList ERROR: problem with data command" << endl;
        exit(0);
      }
      TString fsName(words[1]);
        if (getXYPoints(fsName).size() > 0){
          cout << "FSXYPointList ERROR:  using the same data set name twice: " << fsName <<  endl;
          exit(1);
        }
      int fsRunStart(FSString::TString2int(words[2]));
      int fsRunEnd(FSString::TString2int(words[3]));
      TString sEcm(words[4]);
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
      vector<TString> fsCategories;
        for (unsigned int i = 10; i < words.size(); i++){ fsCategories.push_back(words[i]); }
*/
      FSXYPoint* xyp = new FSXYPoint();
// addstuff
      xyp->display();
      m_vectorXYPoints.push_back(xyp);
    }
    else if (words[0] == "data0"){
      if (words.size() < 4){
        cout << "FSXYPointList ERROR: problem with data0 command" << endl;
        exit(0);
      }
    }
    else if (words[0] == "dsCategory"){
      if (words.size() < 3){
        cout << "FSXYPointList ERROR: problem with dsCategory command" << endl;
        exit(0);
      }
      TString category = words[1];
      for (unsigned int i = 2; i < words.size(); i++){
        vector<FSXYPoint*> xypv = getXYPoints(words[i]);
        for (unsigned int idata = 0; idata < xypv.size(); idata++){
          //xypv[idata]->addCategory(category,false);
        }
      }
    }
    else{
      cout << "FSXYPointList ERROR: problem reading file " << fileName << endl;
      cout << "  line:" << endl << line << endl;  exit(1);
    }
  }
  if (m_vectorXYPoints.size() == 0){
    cout << "FSXYPointList WARNING:  no points found in file: " << fileName << endl;
  }
}




vector<FSXYPoint*>
FSXYPointList::getXYPoints(TString category){
    // loop over points and keep those that pass the category logic
  vector<FSXYPoint*> xypVectorSelected;
  for (unsigned int i = 0; i < m_vectorXYPoints.size(); i++){
    FSXYPoint* xyp = m_vectorXYPoints[i];
    if (FSString::evalLogicalTString(category,xyp->categories()))
      xypVectorSelected.push_back(xyp);
  }
  return xypVectorSelected;
}


void 
FSXYPointList::display(TString category){
  cout << category << endl;
/*
  FSXYPoint* xyp = getXYPoint(dsCategory,lumCategory,ecmGrouping);
  if (!xyp){
    cout << "No data sets found for dsCategory: " << dsCategory << 
                             " and lumCategory: " << lumCategory << endl;
    return;
  }
  vector<FSXYPoint*> xypv = xyp->subSets();
  cout << endl << "****** SUB DATA SETS ******" << endl << endl;
  for (unsigned int i = 0; i < xypv.size(); i++){
    xypv[i]->display(i+1);
  }
  cout << "****** TOTAL DATA SET ******" << endl << endl;
  xyp->display();
*/
}



void
FSXYPointList::clearXYPoints(){
  for (unsigned int i = 0; i < m_vectorXYPoints.size(); i++){
    if (m_vectorXYPoints[i]) delete m_vectorXYPoints[i];
  }
  m_vectorXYPoints.clear();
}


/*
TH1F* 
FSXYPointList::histLuminosity(TString dsCategory,
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
  FSXYPoint* xyps = getXYPoint(dsCategory,lumCategory,ecmGrouping);
  if (!xyps) return FSHistogram::getTH1F(hist);
  vector<FSXYPoint*> vxyps = xyps->subSets(); 
  for (unsigned int i = 0; i < vxyps.size(); i++){
    double ecm = vxyps[i]->ecm();
    double lum = vxyps[i]->lum();
    double elum = vxyps[i]->lumError();
    if (elum < 1.0e-4*lum) elum = 1.0e-4*lum;
    int iecm = 1 + (int)((ecm-hist->GetBinLowEdge(1))/hist->GetBinWidth(1));
    hist->SetBinContent(iecm,lum);
    hist->SetBinError(iecm,elum);
  }
  hist = FSHistogram::getTH1F(hist);
  FSHistogram::setHistogramMaxima(hist);
  return hist;
}
*/


