#include <iostream>
#include <fstream>
#include <string>
#include <complex>
#include "FSBasic/FSString.h"
#include "FSBasic/FSHistogram.h"
#include "FSEEData/FSEEXSList.h"


vector<FSEEXS*> FSEEXSList::m_vectorXS;


void
FSEEXSList::addXSFromFile(TString fileName){
  cout << "FSEEXSList: Reading XS from file: " << fileName << endl;
  ifstream infile(FSString::TString2string(fileName).c_str());
  if (!infile.is_open()){
    cout << "FSEEXSList ERROR: cannot find file: " << fileName << endl;
    exit(0);
  }
  string sline;
  while(getline(infile,sline)){
    TString line = FSString::string2TString(sline);
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
      double x[20];
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
    if (B >= 0){
      N += L*B;
      D += L;
    }
  }
  if (D == 0){
    cout << "FSEEXSList::bgxsAve ERROR: no luminosity" << endl; 
    exit(0);
  }
  return N/D;
}


double
FSEEXSList::effEst(double ecm, TString xsCat, TString dsCat, TString lumCat){
  vector<FSEEXS*> vxs = getXSVector(xsCat,dsCat,lumCat);
  if (vxs.size() == 0){
    cout << "FSEEXSList::effExt ERROR: no cross sections" << endl; 
    exit(0);
  }
  if (ecm < vxs[0]->ecm()) return vxs[0]->eff();
  for (unsigned int i = 0; i < vxs.size()-1; i++){
    double ecm1 = vxs[i]->ecm();
    double ecm2 = vxs[i+1]->ecm();
    if (ecm >= ecm1 && ecm < ecm2){
      double eff1 = vxs[i]->eff();
      double eff2 = vxs[i+1]->eff();
      return (eff2-eff1)/(ecm2-ecm1)*(ecm-ecm1)+eff1;
    }
  }
  return vxs[vxs.size()-1]->eff();
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
  double bgxs = bgxsAve(xsCatOld,dsCatOld,lumCatOld);
  for (unsigned int i = 0; i < vfsds.size(); i++){
    TString dataSetName = vfsds[i]->name();
    double ecm = vfsds[i]->ecm();
    double eff = effEst(ecm,xsCatOld,dsCatOld,lumCatOld);
    FSEEXS* fsxs = addXS(reactionName,dataSetName,sourceName);
    fsxs->initWithPrediction(funcPrediction->Eval(ecm),eff,bgxs,false);
  }
}


/*

double Reaction::BIGDATAPOINT = 100.0;


void
Reaction::addMeasurements(string fileName){


    // read from file

  ifstream fmeas(fileName.c_str());
  if (!fmeas.is_open()){
    cout << "problem opening file " << fileName << endl;
    exit(0);
  }
  string line;
  int type = 0;
  vector<EPM*> measured;
  while (!fmeas.eof()){
    getline(fmeas,line);
    if (line == "initWithMeasurement1"){
      cout << "READING TYPE 1" << endl;
      type = 1;
    }
    else if (line == "initWithMeasurement2"){
      cout << "READING TYPE 2" << endl;
      type = 2;
    }
    else if (line == "initWithMeasurement3"){
      cout << "READING TYPE 3" << endl;
      type = 3;
    }
    else if (line == "initWithMeasurement4"){
      cout << "READING TYPE 4" << endl;
      type = 4;
    }
    else if (line.size() > 0 && line[0] == '#'){
    }
    else if (line.size() > 3){
      vector<string> words = StringFunctions::parseString(line);
      if (words.size() < 3){
        cout << "REACTION parsing problem...  " << line << endl;
        exit(0);
      }
      EPM* epm = new EPM(atoi(words[0].c_str()),atof(words[1].c_str()));
      if (type == 1){
        if (words.size() != 5){
          cout << "REACTION parsing problem... type 1" << endl;
          exit(0);
        }
        epm->initWithMeasurement1(atof(words[2].c_str()),atof(words[3].c_str()),atof(words[4].c_str()));
        cout << epm->ECM() << "  lum = " << epm->LUM() << "  xs = " << epm->XS() << "  bgxs = " << epm->BGXS() << "  eff = " << epm->EFF() << endl;
      }
      if (type == 2){
        if ((words.size() != 6) && (words.size() != 7)){
          cout << "REACTION parsing problem... type 2" << endl;
          exit(0);
        }
        if (words.size() == 6)
        epm->initWithMeasurement2(atof(words[2].c_str()),atof(words[3].c_str()),atof(words[4].c_str()),atof(words[5].c_str()));
        if (words.size() == 7)
        epm->initWithMeasurement2(atof(words[2].c_str()),atof(words[3].c_str()),atof(words[4].c_str()),atof(words[5].c_str()),atof(words[6].c_str()));
        cout << epm->ECM() << "  lum = " << epm->LUM() << "  xs = " << epm->XS() << "  bgxs = " << epm->BGXS() << "  eff = " << epm->EFF() << endl;
      }
      if (type == 3){
        if (words.size() != 5){
          cout << "REACTION parsing problem... type 3" << endl;
          exit(0);
        }
        epm->initWithMeasurement3(atof(words[2].c_str()),atof(words[3].c_str()),atof(words[4].c_str()));
        cout << epm->ECM() << "  lum = " << epm->LUM() << "  xs = " << epm->XS() << "  bgxs = " << epm->BGXS() << "  eff = " << epm->EFF() << endl;
      }
      if (type == 4){
        if (words.size() != 5){
          cout << "REACTION parsing problem... type 4" << endl;
          exit(0);
        }
        epm->initWithMeasurement4(atof(words[2].c_str()),atof(words[3].c_str()),atof(words[4].c_str()));
        cout << epm->ECM() << "  lum = " << epm->LUM() << "  xs = " << epm->XS() << "  bgxs = " << epm->BGXS() << "  eff = " << epm->EFF() << endl;
      }
      if (type == 0){
        cout << "REACTION parsing problem... type 0" << endl;
        exit(0);
      }
      measured.push_back(epm);
    }
  }
  if (measured.size() == 0){
    cout << "nothing found for " << fileName << endl;
    exit(0);
  }
  fmeas.close();


    // loop through the new points and add to the old points

  for (unsigned int inew = 0; inew < measured.size(); inew++){
    if (measured[inew]->LUM() > 0){
      bool found = false;
      for (unsigned int iold = 0; iold < m_measured.size() && !found; iold++){
        if (measured[inew] == m_measured[iold]){
          m_measured[iold]->addPoint(measured[inew]);
          delete measured[inew];
          found = true;
        }
      }
      if (!found){
        m_measured.push_back(measured[inew]);
      }
    }
  }

  if (m_measured.size() == 0) { cout << "Reaction::addMeasurements problem" << endl; exit(0); }


    // sort points

  for (unsigned int i = 0; i < m_measured.size()-1; i++){
  for (unsigned int j = i+1; j < m_measured.size(); j++){
    if (m_measured[j]->ECM() < m_measured[i]->ECM()){
      EPM* temp = m_measured[j];
      m_measured[j] = m_measured[i];
      m_measured[i] = temp;
    }
  }
  }


    // save big points

  m_measuredBig.clear();
  for (unsigned int i = 0; i < m_measured.size(); i++){
    if (m_measured[i]->LUM() > BIGDATAPOINT && m_measured[i]->XS() > 0.0 && m_measured[i]->EFF() > 0.0) m_measuredBig.push_back(m_measured[i]);
  }


    // checks

  if (m_measured.size() == 0){ cout << "problem" << endl; exit(0); }
  if (m_measuredBig.size() == 0){ cout << "problem" << endl; exit(0); }


    // calculate an average background cross section

  double n = 0;
  double d = 0;
  for (unsigned int i = 0; i < m_measuredBig.size(); i++){
    double L = m_measuredBig[i]->LUM();
    double B = m_measuredBig[i]->BGXS();
    if (B >= 0){
      n += L*B;
      d += L;
    }
  }
  if (d == 0){ cout << "problem" << endl; exit(0); }
  m_BGXS = n/d;

}


void
Reaction::clearMeasurements(){
  m_BGXS = 0.0;
  for (unsigned int i = 0; i < m_measured.size(); i++){
    delete m_measured[i];
  }
  m_measured.clear();
  m_measuredBig.clear();
}



void
Reaction::generatePredictions(bool fluctuate){

    // create predictions from future data

  vector<int>    ECMs = m_futureDataSet.ECMs();
  vector<double> LUMs = m_futureDataSet.LUMs();
  for (unsigned int i = 0; i < ECMs.size(); i++){
    EPM* pred = new EPM(ECMs[i],LUMs[i]);
    if (!m_fPRED){ cout << "problem with fPRED" << endl;  exit(0); }
    pred->initWithPrediction(m_fPRED->Eval(ECMs[i]/1000.0),EFF(ECMs[i]),BGXS(),fluctuate);
    m_predicted.push_back(pred);
  }


    // make an unused data set

  ECMs = m_unusedDataSet.ECMs();
  LUMs = m_unusedDataSet.LUMs();
  vector<int>    oldECMs = m_pastDataSet.ECMs();
  vector<double> oldLUMs = m_pastDataSet.LUMs();
  for (unsigned int iold = 0; iold < oldECMs.size(); iold++){
    bool found = false;
    for (unsigned int imeas = 0; imeas < m_measured.size(); imeas++){
      if (oldECMs[iold] == m_measured[imeas]->ECM()){
        if (oldLUMs[iold] >= m_measured[imeas]->LUM() + 5.0){
          oldLUMs[iold] -= m_measured[imeas]->LUM();
        }
        else{
          found = true;
        }
      }
    }
    if (!found){
      ECMs.push_back(oldECMs[iold]);
      LUMs.push_back(oldLUMs[iold]);
    }
  }

    // create predictions from unused past data

  for (unsigned int i = 0; i < ECMs.size(); i++){
    EPM* pred = new EPM(ECMs[i],LUMs[i]);
    pred->initWithPrediction(m_fPRED->Eval(ECMs[i]/1000.0),EFF(ECMs[i]),BGXS(),fluctuate);
    m_unused.push_back(pred);
  }

}


TH1F*
Reaction::hMEASURED(double lumCut, int color, double markersize, double linewidth){
  TH1F* hist = new TH1F("hist","hist",5000,0.0,5.0);
  for (unsigned int i = 0; i < m_measured.size(); i++){
    if (m_measured[i]->LUM() > lumCut){
      hist->SetBinContent(m_measured[i]->ECM(),m_measured[i]->XS());
      hist->SetBinError(m_measured[i]->ECM(),m_measured[i]->EXS());
    }
  }
  return hATT(hist,color,markersize,linewidth);
}


TH1F*
Reaction::hPREDICTED(double lumCut, int color, double markersize, double linewidth){
  TH1F* hist2 = new TH1F("hist2","hist2",5000,0.0,5.0);
  for (unsigned int i = 0; i < m_predicted.size(); i++){
    if (m_predicted[i]->LUM() > lumCut){
      hist2->SetBinContent(m_predicted[i]->ECM()+1,m_predicted[i]->XS());
      hist2->SetBinError(m_predicted[i]->ECM()+1,m_predicted[i]->EXS());
    }
  }
  return hATT(hist2,color,markersize,linewidth);
}


TH1F*
Reaction::hUNUSED(double lumCut, int color, double markersize, double linewidth){
  TH1F* hist3 = new TH1F("hist3","hist3",5000,0.0,5.0);
  for (unsigned int i = 0; i < m_unused.size(); i++){
    if (m_unused[i]->LUM() > lumCut){
      hist3->SetBinContent(m_unused[i]->ECM()+2,m_unused[i]->XS());
      hist3->SetBinError(m_unused[i]->ECM()+2,m_unused[i]->EXS());
    }
  }
  return hATT(hist3,color,markersize,linewidth);
}



TH1F*
Reaction::hMP(double lumCut, int color, double markersize, double linewidth){
  vector<EPM*> mp;
  for (unsigned int i = 0; i < m_measured.size(); i++){
    mp.push_back(new EPM(*m_measured[i]));
  }
  for (unsigned int i = 0; i < m_predicted.size(); i++){
    bool found = false;
    for (unsigned int j = 0; j < mp.size() && !found; j++){
      if (m_predicted[i]->ECM() == mp[j]->ECM()){
        found = true;
        mp[j]->addPoint(m_predicted[i]);
      }
    }
    if (!found) mp.push_back(new EPM(*m_predicted[i]));
  }
  TH1F* histMP = new TH1F("histMP","histMP",5000,0.0,5.0);
  for (unsigned int i = 0; i < mp.size(); i++){
    if (mp[i]->LUM() > lumCut){
      histMP->SetBinContent(mp[i]->ECM(),mp[i]->XS());
      histMP->SetBinError(mp[i]->ECM(),mp[i]->EXS());
    }
  }
  for (unsigned int i = 0; i < mp.size(); i++){ delete mp[i]; }  mp.clear();
  return hATT(histMP,color,markersize,linewidth);
}



TH1F*
Reaction::hMPU(double lumCut, int color, double markersize, double linewidth){
  vector<EPM*> mpu;
  for (unsigned int i = 0; i < m_measured.size(); i++){
    mpu.push_back(new EPM(*m_measured[i]));
  }
  for (unsigned int i = 0; i < m_predicted.size(); i++){
    bool found = false;
    for (unsigned int j = 0; j < mpu.size() && !found; j++){
      if (m_predicted[i]->ECM() == mpu[j]->ECM()){
        found = true;
        mpu[j]->addPoint(m_predicted[i]);
      }
    }
    if (!found) mpu.push_back(new EPM(*m_predicted[i]));
  }
  for (unsigned int i = 0; i < m_unused.size(); i++){
    bool found = false;
    for (unsigned int j = 0; j < mpu.size() && !found; j++){
      if (m_unused[i]->ECM() == mpu[j]->ECM()){
        found = true;
        mpu[j]->addPoint(m_unused[i]);
      }
    }
    if (!found) mpu.push_back(new EPM(*m_unused[i]));
  }
  TH1F* histMPU = new TH1F("histMPU","histMPU",5000,0.0,5.0);
  for (unsigned int i = 0; i < mpu.size(); i++){
    if (mpu[i]->LUM() > lumCut){
      histMPU->SetBinContent(mpu[i]->ECM(),mpu[i]->XS());
      histMPU->SetBinError(mpu[i]->ECM(),mpu[i]->EXS());
    }
  }
  for (unsigned int i = 0; i < mpu.size(); i++){ delete mpu[i]; }  mpu.clear();
  return hATT(histMPU,color,markersize,linewidth);
}



TH1F*
Reaction::hMU(double lumCut, int color, double markersize, double linewidth){
  vector<EPM*> mu;
  for (unsigned int i = 0; i < m_measured.size(); i++){
    mu.push_back(new EPM(*m_measured[i]));
  }
  for (unsigned int i = 0; i < m_unused.size(); i++){
    bool found = false;
    for (unsigned int j = 0; j < mu.size() && !found; j++){
      if (m_unused[i]->ECM() == mu[j]->ECM()){
        found = true;
        mu[j]->addPoint(m_unused[i]);
      }
    }
    if (!found) mu.push_back(new EPM(*m_unused[i]));
  }
  TH1F* histMU = new TH1F("histMU","histMU",5000,0.0,5.0);
  for (unsigned int i = 0; i < mu.size(); i++){
    if (mu[i]->LUM() > lumCut){
      histMU->SetBinContent(mu[i]->ECM(),mu[i]->XS());
      histMU->SetBinError(mu[i]->ECM(),mu[i]->EXS());
    }
  }
  for (unsigned int i = 0; i < mu.size(); i++){ delete mu[i]; }  mu.clear();
  return hATT(histMU,color,markersize,linewidth);
}




// *************************************************************************************
// *************************************************************************************
//       FUNCTIONS
// *************************************************************************************
// *************************************************************************************

  // pipihc functions from yuping

Double_t f2BWpipihcPS(Double_t x){
//The formula used for phase space factor is c0+c1*x+c2*pow(x,2)+c3*pow(x,3), 
//with c0 = 0.00693521; c1 = -0.00239323; c2 = -0.000138674; c3 = 7.61013e-05;
  double c0 = 0.00693521;
  double c1 = -0.00239323;
  double c2 = -0.000138674;
  double c3 = 7.61013e-05;
  return c0+c1*x+c2*pow(x,2)+c3*pow(x,3);
}

Double_t f2BWpipihc(Double_t *x, Double_t *par){
  Double_t xx = x[0];
  Double_t s1 = par[0];
  Double_t m1 = par[1];
  Double_t g1 = par[2];
  Double_t s2 = par[3];
  Double_t m2 = par[4];
  Double_t g2 = par[5];
  Double_t ph = par[6]/180.0*3.14159;
  Double_t cc = par[7];
  complex<double> bw1N(12*3.14159*s1*g1,0.0);
  complex<double> bw1D(xx*xx - m1*m1, m1*g1);
  complex<double> bw1 = sqrt(bw1N)/bw1D;
  complex<double> bw2N(12*3.14159*s2*g2,0.0);
  complex<double> bw2D(xx*xx - m2*m2, m2*g2);
  complex<double> bw2 = sqrt(bw2N)/bw2D;
  complex<double> cph(cos(ph),sin(ph));
  //double phsp1 = 1.0;
  //double phsp2 = 1.0;
  double phsp1 = sqrt(f2BWpipihcPS(xx)/f2BWpipihcPS(m1));
  double phsp2 = sqrt(f2BWpipihcPS(xx)/f2BWpipihcPS(m2));
  return cc+abs(bw1*phsp1+cph*phsp2*bw2)*abs(bw1*phsp1+cph*phsp2*bw2)*0.389379e9;
}


Double_t fBWP4(Double_t *x, Double_t *par){
  Double_t E  = x[0];
  Double_t A  = par[0];
  Double_t M  = par[1];
  Double_t G  = par[2];
  Double_t pi = 3.141592654;
  Double_t g = sqrt(M*M*(M*M+G*G));
  Double_t k = 2.0*sqrt(2.0)*M*G*g/(pi*sqrt(M*M+g));
  Double_t BW = A*k/((E*E-M*M)*(E*E-M*M)+M*M*G*G);
  Double_t c0 = par[3];
  Double_t c1 = par[4];
  Double_t c2 = par[5];
  Double_t c3 = par[6];
  Double_t c4 = par[7];
  Double_t poly4 = c0 + c1*E + c2*E*E + c3*E*E*E + c4*E*E*E*E;
  return BW+poly4;
}



// *************************************************************************************
// *************************************************************************************
//       FITTING
// *************************************************************************************
// *************************************************************************************

TF1*
Reaction::fit(string fitStudy, string histType, double lumCut, double mlow, double mhigh){
  TH1F* hist = NULL;
  if (histType == "M")    hist = hMEASURED(lumCut);
  if (histType == "P")    hist = hPREDICTED(lumCut);
  if (histType == "U")    hist = hUNUSED(lumCut);
  if (histType == "MP")   hist = hMP(lumCut);
  if (histType == "MU")   hist = hMU(lumCut);
  if (histType == "MPU")  hist = hMPU(lumCut);
  if (!hist) { cout << "fit problem" << endl; exit(0);}
  TF1* func = new TF1(*m_fPRED);
  if (fitStudy == "yupingMW"){  // let whole 1st BW float, fix 2nd
    func->FixParameter(3,func->GetParameter(3));
    func->FixParameter(4,func->GetParameter(4));
    func->FixParameter(5,func->GetParameter(5));
    func->FixParameter(6,func->GetParameter(6));
    hist->Fit(func,"0","",mlow,mhigh);
  }
  if (fitStudy == "yuping1BW"){  // take out the 1st BW, let 2nd float
    func->FixParameter(0,0.0);
    func->FixParameter(1,func->GetParameter(1));
    func->FixParameter(2,func->GetParameter(2));
    //func->FixParameter(3,func->GetParameter(3));
    //func->FixParameter(4,func->GetParameter(4));
    //func->FixParameter(5,func->GetParameter(5));
    //func->FixParameter(6,func->GetParameter(6));
    hist->Fit(func,"0","",mlow,mhigh);
  }
  if (fitStudy == "yuping2BW"){  // let everything float
    //func->FixParameter(1,func->GetParameter(1));
    //func->FixParameter(2,func->GetParameter(2));
    //func->FixParameter(3,func->GetParameter(3));
    //func->FixParameter(4,func->GetParameter(4));
    //func->FixParameter(5,func->GetParameter(5));
    //func->FixParameter(6,func->GetParameter(6));
    hist->Fit(func,"0","",mlow,mhigh);
  }
  if (fitStudy == "BWpoly1"){ 
    func->FixParameter(5,0.0);
    func->FixParameter(6,0.0);
    func->FixParameter(7,0.0);
    hist->Fit(func,"0","",mlow,mhigh);
  }
  if (fitStudy == "poly1"){ 
    func->FixParameter(0,0.0);
    func->FixParameter(1,func->GetParameter(1));
    func->FixParameter(2,func->GetParameter(2));
    func->FixParameter(5,0.0);
    func->FixParameter(6,0.0);
    func->FixParameter(7,0.0);
    hist->Fit(func,"0","",mlow,mhigh);
  }
  func->SetLineColor(kBlue);
  func->SetLineStyle(kDashed);
  return func;
}



void
Reaction::makePredictions(bool fluctuate, string predName){
  clearPredictions();
  m_fPRED = NULL;
  if (predName == "pipihc"){
    m_fPRED = new TF1("pipihc",f2BWpipihc,0.0,5.0,8);
    m_fPRED->SetParameter(0,3.6e-9);
    m_fPRED->SetParameter(1,4.2202);
    m_fPRED->SetParameter(2,0.0608);
    m_fPRED->SetParameter(3,10.2e-9);
    m_fPRED->SetParameter(4,4.3891);
    m_fPRED->SetParameter(5,0.1400);
    m_fPRED->SetParameter(6,177.4);
    m_fPRED->SetParameter(7,0.0);
  }
  if (predName == "X3872"){
    m_fPRED = new TF1("X3872",fBWP4,0.0,5.0,8);
    m_fPRED->SetParameter(0,0.05);
    m_fPRED->SetParameter(1,4.263);
    m_fPRED->SetParameter(2,0.095);
    m_fPRED->SetParameter(3,0.0);
    m_fPRED->SetParameter(4,0.0);
    m_fPRED->SetParameter(5,0.0);
    m_fPRED->SetParameter(6,0.0);
    m_fPRED->SetParameter(7,0.0);
  }
  if (predName == "DsDs"){
    m_fPRED = new TF1("DsDs",f2BWpipihc,0.0,5.0,8);
    m_fPRED->SetParameter(0,3.55e-9);
    m_fPRED->SetParameter(1,4.25);
    m_fPRED->SetParameter(2,0.0608);
    m_fPRED->SetParameter(3,13.2e-9);
    m_fPRED->SetParameter(4,4.45);
    m_fPRED->SetParameter(5,0.3000);
    m_fPRED->SetParameter(6,177.4);
    m_fPRED->SetParameter(7,0.0);
  }
  if (predName == "KKJpsi"){
    m_fPRED = new TF1("KKJpsi",f2BWpipihc,0.0,5.0,8);
    m_fPRED->SetParameter(0,3.0e-10);
    m_fPRED->SetParameter(1,4.23);
    m_fPRED->SetParameter(2,0.0508);
    m_fPRED->SetParameter(3,6.0e-10);
    m_fPRED->SetParameter(4,4.5);
    m_fPRED->SetParameter(5,0.080);
    m_fPRED->SetParameter(6,40.0);
    m_fPRED->SetParameter(7,0.0);
  }
  if (predName == "etaJpsi"){
    m_fPRED = new TF1("etaJpsi",f2BWpipihc,0.0,5.0,8);
    m_fPRED->SetParameter(0,3.8e-9);
    m_fPRED->SetParameter(1,4.21);
    m_fPRED->SetParameter(2,0.0608);
    m_fPRED->SetParameter(3,1.3e-9);
    m_fPRED->SetParameter(4,4.02);
    m_fPRED->SetParameter(5,0.0300);
    m_fPRED->SetParameter(6,177.4);
    m_fPRED->SetParameter(7,0.0);
  }
  if (predName == "pipiJpsi"){
    m_fPRED = new TF1("pipiJpsi",f2BWpipihc,0.0,5.0,8);
    m_fPRED->SetParameter(0,4.3e-9);
    m_fPRED->SetParameter(1,4.2237);
    m_fPRED->SetParameter(2,0.0431);
    m_fPRED->SetParameter(3,5.0e-9);
    m_fPRED->SetParameter(4,4.3186);
    m_fPRED->SetParameter(5,0.0957);
    m_fPRED->SetParameter(6,177.4);
    m_fPRED->SetParameter(7,10.0);
  }
  if (predName == "pipipsi2S"){
    m_fPRED = new TF1("pipipsi2S",f2BWpipihc,0.0,5.0,8);
    m_fPRED->SetParameter(0,1.8*0.8e-9);
    m_fPRED->SetParameter(1,4.2095);
    m_fPRED->SetParameter(2,0.0801);
    m_fPRED->SetParameter(3,1.8*3.6e-9);
    m_fPRED->SetParameter(4,4.3838);
    m_fPRED->SetParameter(5,0.0842);
    m_fPRED->SetParameter(6,150);
    m_fPRED->SetParameter(7,0.0);
  }
  if (predName == "omegachic"){
    m_fPRED = new TF1("omegachic",f2BWpipihc,0.0,5.0,8);
    m_fPRED->SetParameter(0,3.0e-9);
    m_fPRED->SetParameter(1,4.2237);
    m_fPRED->SetParameter(2,0.0431);
    m_fPRED->SetParameter(3,0.0e-9);
    m_fPRED->SetParameter(4,4.3186);
    m_fPRED->SetParameter(5,0.0957);
    m_fPRED->SetParameter(6,177.4);
    m_fPRED->SetParameter(7,0.0);
  }
  generatePredictions(fluctuate);
}

*/
