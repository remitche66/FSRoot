#if !defined(FSEEXSLIST_H)
#define FSEEXSLIST_H

#include <iostream>
#include "TH1F.h"
#include "TF1.h"
#include "FSEEData/FSEEDataSet.h"
#include "FSEEData/FSEEXS.h"

using namespace std;


class FSEEXSList{

  public:

    static void addXSFromFile(TString fileName);

    static vector<FSEEXS*> getXSVector(TString xsCat = "", TString dsCat = "", TString lumCat = "");

    static void display(TString xsCat = "", TString dsCat = "", TString lumCat = "");

    static void clearXSList() {}

    static TH1F* histXS(TString xsCat = "", TString dsCat = "", TString lumCat = "", 
                        TString histBounds = "(5000,0.0,5.0)");

    static void addXSFromFunction(TString reactionName, TString sourceName, TF1* funcPrediction,
                                  TString dsCatPredition, double ecmGrouping,
                                  TString xsCatOld, TString dsCatOld, TString lumCatOld);

    static double bgxsAve(TString xsCat = "", TString dsCat = "", TString lumCat = "");

    static double effEst(double ecm, TString xsCat = "", TString dsCat = "", TString lumCat = "");


  private:

    static vector<FSEEXS*> m_vectorXS;

    static FSEEXS* getXS(TString reactionName, TString dataSetName, TString sourceName);
    static FSEEXS* addXS(TString reactionName, TString dataSetName, TString sourceName);




};

#endif




    //FSEEXSList(){
/*
      m_BGXS = 0.0;
      m_measured.clear();
      m_measuredBig.clear();
      m_predicted.clear();
      m_unused.clear();
      m_pastDataSet.clearDataSet();
      m_unusedDataSet.clearDataSet();
      m_futureDataSet.clearDataSet();
      m_fPRED = NULL;
*/
    //}

/*
    ~Reaction(){
      clearPredictions();
      clearMeasurements();
      clearPastDataSet();
      clearFutureDataSet();
      clearUnusedDataSet();
    }

*/

/*
    void clearMeasurements();

    void addFutureDataSet(string dataSetName){ m_futureDataSet.addDataSet(dataSetName); }
    DataSet getFutureDataSet(){ return m_futureDataSet; }
    void writeFutureDataSet(){ m_futureDataSet.writeDataSet(); }
    void clearFutureDataSet(){ m_futureDataSet.clearDataSet(); }

    void addPastDataSet(string dataSetName){ m_pastDataSet.addDataSet(dataSetName); }
    DataSet getPastDataSet(){ return m_pastDataSet; }
    void clearPastDataSet(){ m_pastDataSet.clearDataSet(); }

    void addUnusedDataSet(string dataSetName){ m_unusedDataSet.addDataSet(dataSetName); }
    DataSet getUnusedDataSet(){ return m_unusedDataSet; }
    void clearUnusedDataSet(){ m_unusedDataSet.clearDataSet(); }

    void makePredictions(bool fluctuate, double constant){
      clearPredictions();
      //if (m_fPRED) delete m_fPRED;
      m_fPRED = new TF1("fpred",to_string(constant).c_str(),0.0,5.0);
      generatePredictions(fluctuate);
    }

    void makePredictions(bool fluctuate, string predName);

    void clearPredictions(){
      for (unsigned int i = 0; i < m_predicted.size(); i++){
        delete m_predicted[i];
      }
      m_predicted.clear();
      for (unsigned int i = 0; i < m_unused.size(); i++){
        delete m_unused[i];
      }
      m_unused.clear();
      //if (m_fPRED) delete m_fPRED;
      m_fPRED = NULL;
    }


    TH1F* hMEASURED  (double lumCut = 0.0, int color = kBlack, double markersize = 1.0, double linewidth = 1.0);
    TH1F* hPREDICTED (double lumCut = 0.0, int color = kRed,   double markersize = 1.0, double linewidth = 1.0);
    TH1F* hUNUSED    (double lumCut = 0.0, int color = kBlue,  double markersize = 1.0, double linewidth = 1.0);
    TH1F* hMP        (double lumCut = 0.0, int color = kBlack, double markersize = 1.0, double linewidth = 1.0);
    TH1F* hMPU       (double lumCut = 0.0, int color = kBlack, double markersize = 1.0, double linewidth = 1.0);
    TH1F* hMU        (double lumCut = 0.0, int color = kBlack, double markersize = 1.0, double linewidth = 1.0);

    static TH1F* hATT(TH1F* hist, int color = kBlack, double markersize = 1.0, double linewidth = 1.0){
      hist->SetLineColor(color);
      hist->SetMarkerColor(color);
      hist->SetMarkerSize(markersize);
      hist->SetLineWidth(linewidth);
      hist->SetStats(0);
      hist->SetTitleOffset(1.5,"X");
      hist->SetTitleOffset(1.8,"Y");
      if (hist->GetMinimum() >= 0) hist->SetMinimum(0.0);
      return hist;
    }

*/



/*

    TF1* fPRED() {  m_fPRED->SetLineColor(kBlack);
                    m_fPRED->SetLineStyle(kDashed);
                    m_fPRED->SetNpx(1000);
                    return m_fPRED; }

    TF1* fit(string fitStudy, string histType, double lumCut, double mlow = 3.95, double mhigh = 4.65);

    static double BIGDATAPOINT;


    vector<EPM*> m_predicted;
*/


/*

  private:

    void generatePredictions(bool fluctuate);

    double m_BGXS;
    string m_reactionName;
    vector<EPM*> m_measured;
    vector<EPM*> m_measuredBig;
    vector<EPM*> m_unused;
    DataSet m_futureDataSet;
    DataSet m_pastDataSet;
    DataSet m_unusedDataSet;
    TF1* m_fPRED;
*/

