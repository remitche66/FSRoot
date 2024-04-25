#include <iostream>
#include "TCanvas.h"
#include "TColor.h"
#include "TStyle.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TLine.h"
#include "TArrow.h"
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

  else if (i == 1){
    double stops[3] = {0.0,0.3,1.0};
    double red[3]   = {1.0,0.2,1.0};
    double green[3] = {1.0,0.2,0.0};
    double blue[3]  = {1.0,0.2,0.0};
    TColor::CreateGradientColorTable(3,stops,red,green,blue,250);
  }

  else if (i == -1){
    double stops[3] = {0.0,0.5,1.0};
    double red[3]   = {0.0,1.0,1.0};
    double green[3] = {0.0,1.0,0.0};
    double blue[3]  = {1.0,1.0,0.0};
    TColor::CreateGradientColorTable(3,stops,red,green,blue,250);
  }

  else if (i == 2){
    //double stops[3] = {0.0,0.4,1.0};
    //double red[3]   = {1.0,0.2,0.0};
    //double green[3] = {1.0,0.2,0.0};
    //double blue[3]  = {1.0,0.2,0.0};
    double stops[3] = {0.0,0.4,1.0};
    double red[3]   = {1.0,0.5,0.0};
    double green[3] = {1.0,0.5,0.0};
    double blue[3]  = {1.0,0.5,0.0};
    TColor::CreateGradientColorTable(3,stops,red,green,blue,250);
  }

  else if (i == 3){  // from internet
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;
    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);
  }

  else{
    gStyle->SetPalette(i);
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



  // ********************************************************
  //  DRAW AN ARROW (size and length are in percent of canvas width)
  // ********************************************************

void
FSCanvas::drawCutArrow(double x, int color, TString type, double size, double length){
  if (!gPad) return;
  gPad->Update();
  if (type.Contains("<")) length = -1.0*length;
  double xmin = gPad->GetX1();
  double xmax = gPad->GetX2();
  double ymax = gPad->GetY2();
  double xline = x;
  double xarr1 = x;
  double xarr2 = xline + (xmax-xmin)*length;
  if (type.Contains("<")){ double xtemp = xarr2; xarr2 = xarr1; xarr1 = xtemp; }
  TLine* tline1 = new TLine(xline,0.0,xline,ymax*0.5);
  tline1->SetLineColor(color); tline1->Draw();
  TArrow* tarr1 = new TArrow(xarr1,ymax*0.25,xarr2,ymax*0.25,size,type);
  tarr1->SetFillColor(color); tarr1->SetLineColor(color); tarr1->Draw();
}
