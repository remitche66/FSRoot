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
    if (words[0] == "addXYPoint"){
      if (words.size() < 2){
        cout << "FSXYPointList ERROR: problem with addXYPoint command" << endl;
        exit(0);
      }
      FSXYPoint* xyp = new FSXYPoint();
      TString sVals = line;  while(sVals.Contains("addXYPoint")){ sVals.Replace(sVals.Index("addXYPoint"),10,""); }
      xyp->setValuesFromString(sVals);
      xyp->display();
      m_vectorXYPoints.push_back(xyp);
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


