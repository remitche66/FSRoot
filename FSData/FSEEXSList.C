#include <iostream>
#include <fstream>
#include <string>
#include <complex>
#include "FSBasic/FSString.h"
#include "FSBasic/FSHistogram.h"
#include "FSData/FSEEXSList.h"


vector<FSEEXS*> FSEEXSList::m_vectorXS;


void
FSEEXSList::addXSFromFile(TString fileName){
  cout << "FSEEXSList: Reading XS from file: " << fileName << endl;
  vector<TString> lines = FSString::readLinesFromFile(fileName);
  for (unsigned int iL = 0; iL < lines.size(); iL++){
    TString line = lines[iL];
    vector<TString> words = FSString::parseTString(line);
    if (words.size() == 0) continue;
    if (words[0][0] == '#') continue;
    int ixsType = 0;
    if (words[0] == "xs1") ixsType = 1;
    if (words[0] == "xs2") ixsType = 2;
    if (words[0] == "xs3") ixsType = 3;
    if (words[0] == "xs4") ixsType = 4;
    if (ixsType > 0){
      unsigned int nWords[5] = {0,7,9,7,7}; 
      if (words.size() < nWords[ixsType]){
        cout << "FSEEXSList ERROR: problem with " << words[0] << " command" << endl;
        exit(0);
      }
      TString reactionName(words[1]);
      TString dataSetName(words[2]);
      TString sourceName(words[3]);
      FSEEXS* fsxs = addXS(reactionName,dataSetName,sourceName);
      double x[20] = {};
      if (nWords[ixsType] > 4) x[4] = FSString::TString2double(words[4]); 
      if (nWords[ixsType] > 5) x[5] = FSString::TString2double(words[5]); 
      if (nWords[ixsType] > 6) x[6] = FSString::TString2double(words[6]); 
      if (nWords[ixsType] > 7) x[7] = FSString::TString2double(words[7]); 
      if (nWords[ixsType] > 8) x[8] = FSString::TString2double(words[8]); 
      if (ixsType == 1) fsxs->initWithXS1(x[4],x[5],x[6]);
      if (ixsType == 2) fsxs->initWithXS2(x[4],x[5],x[6],x[7],x[8]);
      if (ixsType == 3) fsxs->initWithXS3(x[4],x[5],x[6]);
      if (ixsType == 4) fsxs->initWithXS4(x[4],x[5],x[6]);
      for (unsigned int i = nWords[ixsType]; i < words.size(); i++){
        fsxs->addXSCategory(words[i]);
      }
      fsxs->display();
    }
    else if (words[0] == "xsCategory"){
      if (words.size() < 3){
        cout << "FSEEXSList ERROR: problem with xsCategory command" << endl;
        exit(0);
      }
      TString category = words[1];
      for (unsigned int i = 2; i < words.size(); i++){
        vector<FSEEXS*> fsxsv = getXSVector(words[i]);
        for (unsigned int ixs = 0; ixs < fsxsv.size(); ixs++){
          fsxsv[ixs]->addXSCategory(category);
        }
      }
    }
    else{
      cout << "FSEEXSList ERROR: problem reading file " << fileName << endl;
      cout << "  line:" << endl << line << endl;  exit(1);
    }
  }
}



vector<FSEEXS*>
FSEEXSList::getXSVector(TString xsCat, TString dsCat, TString lumCat) { 
  vector<FSEEXS*> xsSelected;
  for (unsigned int i = 0; i < m_vectorXS.size(); i++){
    FSEEXS* xs = m_vectorXS[i];
    FSEEDataSet* ds = xs->dataSet();
    if (FSString::evalLogicalTString(xsCat,xs->xsCategories()) &&
        FSString::evalLogicalTString(dsCat,ds->dsCategories()) &&
        FSString::evalLogicalTString(lumCat,ds->lumCategories())){
      xsSelected.push_back(xs);
    }
  }
  return xsSelected;
}

void
FSEEXSList::display(TString xsCat, TString dsCat, TString lumCat){
  vector<FSEEXS*> vfsxs = getXSVector(xsCat,dsCat,lumCat);
  for (unsigned int i = 0; i < vfsxs.size(); i++){ vfsxs[i]->display(i+1); }
}


FSEEXS*
FSEEXSList::getXS(TString reactionName, TString dataSetName, TString sourceName){
  for (unsigned int i = 0; i < m_vectorXS.size(); i++){
    if ((m_vectorXS[i]->reactionName() == reactionName) &&
        (m_vectorXS[i]->dataSetName()  == dataSetName) &&
        (m_vectorXS[i]->sourceName()   == sourceName)) return m_vectorXS[i];
  }
  return NULL;
}


FSEEXS*
FSEEXSList::addXS(TString reactionName, TString dataSetName, TString sourceName){
  FSEEXS* fsxs = getXS(reactionName,dataSetName,sourceName);
  if (fsxs){
    cout << "FSEEXSList ERROR:  duplicate XS" << endl;
    fsxs->display();
    exit(0);
  }
  fsxs = new FSEEXS(reactionName,dataSetName,sourceName);
  m_vectorXS.push_back(fsxs);
  return fsxs;
}



TH1F* 
FSEEXSList::histXS(TString xsCat, TString dsCat, TString lumCat, TString histBounds){
  int nbins = FSString::parseBoundsNBinsX(histBounds);
  double x1 = FSString::parseBoundsLowerX(histBounds);
  double x2 = FSString::parseBoundsUpperX(histBounds);
  TH1F* hist = new TH1F("hist","histXS",nbins,x1,x2);
  hist->SetTitle("Cross Sections");
  hist->SetXTitle("Center-of-Mass Energy  (GeV)");
  hist->SetYTitle("Cross Section  (pb)");
  vector<FSEEXS*> vfsxs = getXSVector(xsCat,dsCat,lumCat);
  if (vfsxs.size()==0) return FSHistogram::getTH1F(hist);
  for (unsigned int i = 0; i < vfsxs.size(); i++){
    double ecm = vfsxs[i]->ecm();
    double xs = vfsxs[i]->xs();
    double exs = vfsxs[i]->exs();
    int iecm = 1 + (int)((ecm-hist->GetBinLowEdge(1))/hist->GetBinWidth(1));
    hist->SetBinContent(iecm,xs);
    hist->SetBinError(iecm,exs);
  }
  hist = FSHistogram::getTH1F(hist);
  FSHistogram::setHistogramMaxima(hist);
  return hist;
}


double
FSEEXSList::bgxsAve(TString xsCat, TString dsCat, TString lumCat){
  vector<FSEEXS*> vxs = getXSVector(xsCat,dsCat,lumCat);
  if (vxs.size() == 0){
    cout << "FSEEXSList::bgxsAve ERROR: no cross sections" << endl; 
    exit(0);
  }
  double N = 0.0;
  double D = 0.0;
  for (unsigned int i = 0; i < vxs.size(); i++){
    double L = vxs[i]->lum();
    double B = vxs[i]->bgxs();
    double nsigma = 0.0;
    if (vxs[i]->exs() > 0.0) nsigma = vxs[i]->xs()/vxs[i]->exs();
    if (nsigma < 1.5) continue;
    if (B >= 0){
      N += L*B;
      D += L;
    }
  }
  if (D == 0){
    cout << "FSEEXSList::bgxsAve WARNING: problem determining bgxsAve, returning 0" << endl; 
    return 0.0;
  }
  return N/D;
}



double
FSEEXSList::bgxsEst(double ecm, TString xsCat, TString dsCat, TString lumCat){
    // get an original list of cross sections
  vector<FSEEXS*> vxs = getXSVector(xsCat,dsCat,lumCat);
  if (vxs.size() == 0){
    cout << "FSEEXSList::bgxsEst ERROR: no cross sections" << endl; 
    exit(0);
  }
    // make a select list with good measurements
  vector<FSEEXS*> selectvxs;
  for (unsigned int i = 0; i < vxs.size(); i++){
    double nsigma = 0.0;
    if (vxs[i]->exs() > 0.0) nsigma = vxs[i]->xs()/vxs[i]->exs();
    if (nsigma < 1.5) continue;
    selectvxs.push_back(vxs[i]);
  }
    // find the closest
  double bgxs = 0.0;
  double closestecm = 10000.0;
  for (unsigned int i = 0; i < selectvxs.size(); i++){
    double ecmdiff = fabs(selectvxs[i]->ecm() - ecm);
    if (ecmdiff < closestecm){
      closestecm = ecmdiff;
      bgxs = selectvxs[i]->bgxs();
    }
  }
  if (bgxs == 0.0){
    cout << "FSEEXSList::bgxsEst WARNING: problem determining bgxsEst, returning 0" << endl; 
  }
  return bgxs;
}


double
FSEEXSList::effEst(double ecm, TString xsCat, TString dsCat, TString lumCat){
    // get an original list of cross sections
  vector<FSEEXS*> vxs = getXSVector(xsCat,dsCat,lumCat);
  if (vxs.size() == 0){
    cout << "FSEEXSList::effExt ERROR: no cross sections" << endl; 
    exit(0);
  }
    // make a list with non-zero efficiency
  vector<FSEEXS*> vxsEff;
  for (unsigned int i = 0; i < vxs.size(); i++){
    if (vxs[i]->eff() > 0) vxsEff.push_back(vxs[i]);
  }
  if (vxsEff.size() == 0){
    cout << "FSEEXSList::effExt ERROR: no cross sections with non-zero efficiency" << endl; 
    exit(0);
  }
    // interpololate
  if (ecm < vxsEff[0]->ecm()) return vxsEff[0]->eff();
  for (unsigned int i = 0; i < vxsEff.size()-1; i++){
    double ecm1 = vxsEff[i]->ecm();
    double ecm2 = vxsEff[i+1]->ecm();
    if (ecm >= ecm1 && ecm < ecm2){
      double eff1 = vxsEff[i]->eff();
      double eff2 = vxsEff[i+1]->eff();
      return (eff2-eff1)/(ecm2-ecm1)*(ecm-ecm1)+eff1;
    }
  }
  return vxsEff[vxsEff.size()-1]->eff();
}


void
FSEEXSList::addXSFromFunction(TString reactionName, TString sourceName, TF1* funcPrediction,
                              TString dsCatPrediction, double ecmGrouping,
                              TString xsCatOld, TString dsCatOld, TString lumCatOld){
  FSEEDataSet* fsds = FSEEDataSetList::getDataSet(dsCatPrediction,"",ecmGrouping);
  if (!fsds){
    cout << "FSEEXSList::addXSFromFunction ERROR: no data sets" << endl;
    exit(0);
  }
  vector<FSEEDataSet*> vfsds = fsds->subSets();
  //double bgxs = bgxsAve(xsCatOld,dsCatOld,lumCatOld);
  for (unsigned int i = 0; i < vfsds.size(); i++){
    TString dataSetName = vfsds[i]->name();
    double ecm = vfsds[i]->ecm();
    double eff = effEst(ecm,xsCatOld,dsCatOld,lumCatOld);
    double bgxs = bgxsEst(ecm,xsCatOld,dsCatOld,lumCatOld);
    FSEEXS* fsxs = addXS(reactionName,dataSetName,sourceName);
    fsxs->initWithPrediction(funcPrediction->Eval(ecm),eff,bgxs,false);
  }
}



void
FSEEXSList::clearXSList(){
  for (unsigned int i = 0; i < m_vectorXS.size(); i++){
    if (m_vectorXS[i]) delete m_vectorXS[i];
  }
  m_vectorXS.clear();
}
