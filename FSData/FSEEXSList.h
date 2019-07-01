#if !defined(FSEEXSLIST_H)
#define FSEEXSLIST_H

#include <iostream>
#include "TH1F.h"
#include "TF1.h"
#include "FSData/FSEEDataSet.h"
#include "FSData/FSEEXS.h"

using namespace std;


class FSEEXSList{

  public:

    static void addXSFromFile(TString fileName);

    static vector<FSEEXS*> getXSVector(TString xsCat = "", TString dsCat = "", TString lumCat = "");

    static void display(TString xsCat = "", TString dsCat = "", TString lumCat = "");

    static void clearXSList();

    static TH1F* histXS(TString xsCat = "", TString dsCat = "", TString lumCat = "", 
                        TString histBounds = "(5000,0.0,5.0)");

    static void addXSFromFunction(TString reactionName, TString sourceName, TF1* funcPrediction,
                                  TString dsCatPredition, double ecmGrouping,
                                  TString xsCatOld, TString dsCatOld, TString lumCatOld);

    static double bgxsAve(TString xsCat = "", TString dsCat = "", TString lumCat = "");

    static double bgxsEst(double ecm, TString xsCat = "", TString dsCat = "", TString lumCat = "");

    static double effEst(double ecm, TString xsCat = "", TString dsCat = "", TString lumCat = "");


  private:

    static vector<FSEEXS*> m_vectorXS;

    static FSEEXS* getXS(TString reactionName, TString dataSetName, TString sourceName);
    static FSEEXS* addXS(TString reactionName, TString dataSetName, TString sourceName);




};

#endif


