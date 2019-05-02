#include <iostream>
#include "TCanvas.h"
#include "TColor.h"
#include "TStyle.h"
#include "TH2F.h"
#include "TRandom.h"
#include "FSBasic/FSCanvas.h"


  // ********************************************************
  // DIVIDE A CANVAS INTO NUM PIECES
  // ********************************************************

void
FSCanvas::divideGPad(int num){
  if (!gPad) new TCanvas;
  int nx = divideGPadX(num);
  int ny = divideGPadY(num);
  gPad->Clear();
  gPad->Divide(nx,ny);
}

int
FSCanvas::divideGPadY(int num){
       if (num <= 2)  {return 1;}
  else if (num <= 8)  {return 2;}
  else if (num <= 15) {return 3;}
  else if (num <= 24) {return 4;}
  else                {return 5;}
}

int
FSCanvas::divideGPadX(int num){
  return 1 + (num-1)/divideGPadY(num);
}


  // ********************************************************
  // CHANGE COLOR SCHEMES
  // ********************************************************

void 
FSCanvas::colorScheme(int i){

  if (i == 0){
    gStyle->SetPalette(1);
  }

  if (i == 1){
    double stops[3] = {0.0,0.3,1.0};
    double red[3]   = {1.0,0.2,1.0};
    double green[3] = {1.0,0.2,0.0};
    double blue[3]  = {1.0,0.2,0.0};
    TColor::CreateGradientColorTable(3,stops,red,green,blue,250);
  }

  if (i == 2){
    double stops[3] = {0.0,0.4,1.0};
    double red[3]   = {1.0,0.2,0.0};
    double green[3] = {1.0,0.2,0.0};
    double blue[3]  = {1.0,0.2,0.0};
    TColor::CreateGradientColorTable(3,stops,red,green,blue,250);
  }

  if (i == 3){  // from internet
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
  }

}


void
FSCanvas::testColorScheme(int i){
  FSCanvas::colorScheme(i);
  TH2F* hist = new TH2F("hist","hist",100,-3.0,3.0,100,-3.0,3.0);
  TRandom random;
  for (int n = 0; n <= 100000; n++){
    hist->Fill(random.Gaus(),random.Gaus());
  }
  hist->SetStats(0);
  hist->SetTitle("Testing Color Scheme");
  hist->Draw("colz");
}

