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
  cout << "FSXYPointList: Reading points from file: " << fileName << endl;
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
// come back to these:  derive newCat = oldCat1 */+- oldCat2
//    else if (words[0] == "derive"){
// category cat cat1 cat2 etc.
//    else if (words[0] == "category"){
    else{
      cout << "FSXYPointList ERROR: problem reading file " << fileName << endl;
      cout << "  line:" << endl << line << endl;  exit(1);
    }
  }
  if (m_vectorXYPoints.size() == 0){
    cout << "FSXYPointList WARNING:  no points found in file: " << fileName << endl;
  }
}


void
FSXYPointList::addDerivedXYPoints(TString newCategory, TString oldCategory1, TString operation, 
                                  TString oldCategory2, double xTolerance){
  cout << newCategory << endl;
  cout << oldCategory1 << endl;
  cout << oldCategory2 << endl;
  cout << xTolerance << endl;
  cout << operation << endl;
    // test for overlaps
  TString ctest("");
  if (oldCategory1!="") ctest += "("+oldCategory1+")";
  if (oldCategory1!=""&&oldCategory2!="") ctest += "&";
  if (oldCategory2!="") ctest += "("+oldCategory2+")";
  vector<FSXYPoint*> vtest = getXYPoints(ctest);
  if (vtest.size() != 0){
    cout << "FSXYPointList ERROR:  category overlap in addDerivedXYPoints" << endl;
    exit(0);
  }
    // test for ambiguities
  vector<FSXYPoint*> vxyp1 = getXYPoints(oldCategory1);
  vector<FSXYPoint*> vxyp2 = getXYPoints(oldCategory2);
  bool ambiguityError = false;
  for (unsigned int i = 1; i < vxyp1.size(); i++){
    if ((vxyp1[i-1]->xLabel() == "") && (vxyp1[i]->xLabel() == "") &&
        (vxyp1[i]->xValue() - vxyp1[i-1]->xValue()) < xTolerance) ambiguityError = true; }
  for (unsigned int i = 1; i < vxyp2.size(); i++){
    if ((vxyp2[i-1]->xLabel() == "") && (vxyp2[i]->xLabel() == "") &&
        (vxyp2[i]->xValue() - vxyp2[i-1]->xValue()) < xTolerance) ambiguityError = true; }
  if (ambiguityError){
    cout << "FSXYPointList ERROR:  ambiguity in addDerivedXYPoints" << endl;
    exit(0);
  }
//    FSXYPoint(TString category, double scale, FSXYPoint* xyp); 
//    FSXYPoint(TString category, TString operation, FSXYPoint* xyp1, FSXYPoint* xyp2); 
//  NOTE:  needs some more thought -- do averages from oldCategories and then use those for newCategory?
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
  if (xypVectorSelected.size() < 2) return xypVectorSelected;
    // sort points by their xValue or xLabel
  for (unsigned int i = 0; i < xypVectorSelected.size()-1; i++){
  for (unsigned int j = i+1; j < xypVectorSelected.size(); j++){
    double xi = xypVectorSelected[i]->xValue();
    double xj = xypVectorSelected[j]->xValue();
    TString si = xypVectorSelected[i]->xLabel();
    TString sj = xypVectorSelected[j]->xLabel();
    if ((xj < xi) || ((xj == xi) && (sj < si))){
      FSXYPoint* temp = xypVectorSelected[i];
      xypVectorSelected[i] = xypVectorSelected[j];
      xypVectorSelected[j] = temp;
    }
  }}
  return xypVectorSelected;
}


void 
FSXYPointList::display(TString category){
  vector<FSXYPoint*> xypv = getXYPoints(category);
  for (unsigned int i = 0; i < xypv.size(); i++){
    xypv[i]->display();
  }
}



void
FSXYPointList::clearXYPoints(){
  for (unsigned int i = 0; i < m_vectorXYPoints.size(); i++){
    if (m_vectorXYPoints[i]) delete m_vectorXYPoints[i];
  }
  m_vectorXYPoints.clear();
}


TH1F* 
FSXYPointList::getTH1F(TString category, TString histBounds, bool includeSystErrors){
  int nbins = FSString::parseBoundsNBinsX(histBounds);
  double x1 = FSString::parseBoundsLowerX(histBounds);
  double x2 = FSString::parseBoundsUpperX(histBounds);
  TH1F* hist = new TH1F("histXYPoint","histXYPoint",nbins,x1,x2);
  hist->SetTitle("FSXYPointList (category = \""+category+"\")");
  hist->SetXTitle("X Values");
  hist->SetYTitle("Y Values");
  vector<FSXYPoint*> vxyp = getXYPoints(category);
  for (unsigned int i = 0; i < vxyp.size(); i++){
    double x  = vxyp[i]->xValue();
    double y  = vxyp[i]->yValue();
    double ey = vxyp[i]->yError();  if (includeSystErrors) ey = vxyp[i]->yErrorTot();
    int ix = 1 + (int)((x-hist->GetBinLowEdge(1))/hist->GetBinWidth(1));
    if (hist->GetBinContent(ix) != 0){ 
      cout << "FSXYPointList WARNING: overwriting data in histogram" << endl; 
    }
    hist->SetBinContent(ix,y);
    hist->SetBinError(ix,ey);
  }
  hist = FSHistogram::getTH1F(hist);
  FSHistogram::setHistogramMaxima(hist);
  return hist;
}

TH1F* 
FSXYPointList::getEmptyTH1F(TString category, TString histBounds){
  TH1F* hist = getTH1F(category,histBounds);
  double max = hist->GetMaximum();
  hist->Reset();
  hist = FSHistogram::getTH1F(hist);
  hist->SetMaximum(max);
  return hist;
}

TGraphAsymmErrors*
FSXYPointList::getTGraph(TString category, bool includeSystErrors){
  static const int MAXPOINTS = 10000;
  int n = 0;
  double vx[MAXPOINTS];  double vxel[MAXPOINTS];  double vxeh[MAXPOINTS];
  double vy[MAXPOINTS];  double vyel[MAXPOINTS];  double vyeh[MAXPOINTS];
  vector<FSXYPoint*> vxyp = getXYPoints(category);
  for (unsigned int i = 0; i < vxyp.size(); i++){
    vx[n]   = vxyp[i]->xValue();
    vxel[n] = vxyp[i]->xErrorLow();   if (includeSystErrors) vxel[n] = vxyp[i]->xErrorTotLow();
    vxeh[n] = vxyp[i]->xErrorHigh();  if (includeSystErrors) vxeh[n] = vxyp[i]->xErrorTotHigh();
    vy[n]   = vxyp[i]->yValue();
    vyel[n] = vxyp[i]->yErrorLow();   if (includeSystErrors) vyel[n] = vxyp[i]->yErrorTotLow();
    vyeh[n] = vxyp[i]->yErrorHigh();  if (includeSystErrors) vyeh[n] = vxyp[i]->yErrorTotHigh();
    n++;
  }
  TGraphAsymmErrors* tgraph = new TGraphAsymmErrors(n,vx,vy,vxel,vxeh,vyel,vyeh);
  return tgraph;
}



