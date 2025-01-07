#include <iostream>
#include "TCanvas.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSHistogram.h"

using namespace std;

int main(){

  cout << "testing!" << endl;

  cout << FSString::double2TString(4.4444) << endl;

  TCanvas* c1 = new TCanvas("c1","c1",500,500);
  TH1F* hist = FSHistogram::getTH1FFormula("x+1","(100,0.0,1.0)");
  hist->Draw();
  c1->Print("testing.pdf");

}

