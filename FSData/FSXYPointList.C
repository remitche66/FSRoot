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
FSXYPointList::addXYPointFromString(TString inputString){
  FSXYPoint* xyp = new FSXYPoint();
  xyp->setValuesFromString(inputString);
  m_vectorXYPoints.push_back(xyp);
}

void
FSXYPointList::addXYPointsFromFile(TString fileName){
  cout << "FSXYPointList: Reading points from file: " << fileName << endl;
  vector<TString> fileCategories;
  vector<TString> lines = FSString::readLinesFromFile(fileName);
  for (unsigned int iL = 0; iL < lines.size(); iL++){
    TString line = lines[iL];
    vector<TString> words = FSString::parseTString(line);
    if (words.size() == 0) continue;
    if (words[0][0] == '#') continue;
    if (words[0] == "addXYPoint"){
      if (words.size() < 2){
        cout << "FSXYPointList ERROR: problem with addXYPoint command" << endl;
        exit(0);
      }
      FSXYPoint* xyp = new FSXYPoint();
      TString sVals = line;  sVals.Replace(sVals.Index("addXYPoint"),10,"");
      xyp->setValuesFromString(sVals);
      xyp->addCategories(fileCategories);
      m_vectorXYPoints.push_back(xyp);
      xyp->display(m_vectorXYPoints.size());
    }
    else if (words[0] == "addCategory"){
      if (words.size() < 3){
        cout << "FSXYPointList ERROR: problem with addCategory command" << endl;
        exit(0);
      }
      TString newCategory = words[1];
      for (unsigned int i = 2; i < words.size(); i++){
        TString oldCategory = words[i];
        vector<FSXYPoint*> vxyp = getXYPoints(oldCategory);
        for (unsigned int j = 0; j < vxyp.size(); j++){
          vxyp[j]->addCategory(newCategory);
        }
      }
    }
    else if (words[0] == "addXYPointFileCategories"){
      TString sVals = line;  sVals.Replace(sVals.Index("addXYPointFileCategories"),24,"");
      vector<TString> vVals = FSString::parseTString(sVals);
      for (unsigned int i = 0; i < vVals.size(); i++){ fileCategories.push_back(vVals[i]); }
    }
    else if (words[0] == "clearXYPointFileCategories"){
      fileCategories.clear();
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
    xypv[i]->display(i+1);
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
  return hist;
}

TH1F* 
FSXYPointList::getEmptyTH1F(TString category, TString histBounds){
  TH1F* hist = getTH1F("!*",histBounds);
  hist->SetTitle("FSXYPointList (category = \""+category+"\")");
  pair<double,double> minmax = getMinMax(category,true);
  double min = minmax.first; double max = minmax.second;
  hist->SetMinimum(min - 0.05*abs(min));
  hist->SetMaximum(max + 0.05*abs(max));
  return hist;
}

pair<double,double>
FSXYPointList::getMinMax(TString category, bool includeErrors){
  vector<FSXYPoint*> vxyp = getXYPoints(category);
  if (vxyp.size() == 0) return pair<double,double>(0.0,0.0);
  double min = vxyp[0]->yValue(); if (includeErrors) min -= vxyp[0]->yErrorTotLow();
  double max = vxyp[0]->yValue(); if (includeErrors) max += vxyp[0]->yErrorTotHigh();
  for (unsigned int i = 1; i < vxyp.size(); i++){
    double minTest = vxyp[i]->yValue(); if (includeErrors) minTest -= vxyp[i]->yErrorTotLow();
    double maxTest = vxyp[i]->yValue(); if (includeErrors) maxTest += vxyp[i]->yErrorTotHigh();
    if (minTest < min) min = minTest;
    if (maxTest > max) max = maxTest;
  }
  return pair<double,double>(min,max);
}

TGraphAsymmErrors*
FSXYPointList::getTGraph(TString category, bool includeSystErrors, 
                         int color, int markerStyle, double lineWidth, double markerSize){
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
  tgraph->SetMarkerColor(color);
  tgraph->SetLineColor(color);
  tgraph->SetMarkerStyle(markerStyle);
  tgraph->SetLineWidth(lineWidth);
  tgraph->SetMarkerSize(markerSize);
  return tgraph;
}


TF1*
FSXYPointList::getIdeogram(TString category, TString XY, bool includeSystErrors){
  TString sIdeogram("");
  XY.ToUpper();
  vector<FSXYPoint*> vxyp = getXYPoints(category);
  double x1 = 0.0; double x2 = 0.0;
  for (unsigned int i = 0; i < vxyp.size(); i++){
    double mean = 0.0;
    if (XY == "X") mean = vxyp[i]->xValue();
    if (XY == "Y") mean = vxyp[i]->yValue();
    double width = 0.0;
    if ((XY == "X") && (!includeSystErrors)) width = vxyp[i]->xError();
    if ((XY == "Y") && (!includeSystErrors)) width = vxyp[i]->yError();
    if ((XY == "X") &&  (includeSystErrors)) width = vxyp[i]->xErrorTot();
    if ((XY == "Y") &&  (includeSystErrors)) width = vxyp[i]->yErrorTot();
    if (width == 0.0) continue;
    TString sMean = FSString::double2TString(mean,8);
    TString sWidth = FSString::double2TString(width,8);
    TString sSize = FSString::double2TString(1.0/width,8);
    double x1temp = mean - width*5.0;  double x2temp = mean + width*5.0;
    if (sIdeogram == "" || x1temp < x1){ x1 = x1temp; }
    if (sIdeogram == "" || x2temp > x2){ x2 = x2temp; }
    if (sIdeogram != "") sIdeogram += "+";
    sIdeogram += sSize+"*TMath::Gaus(x,"+sMean+","+sWidth+",1)";
  }
  if (sIdeogram == "") return NULL;
  TF1* f1 = new TF1(category,sIdeogram,x1,x2);
  f1->SetLineColor(kBlack);
  f1->SetNpx(10000);
  return f1;
}

