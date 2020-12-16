
void examples_sec22(){
  TCanvas* c22 = new TCanvas("c22","c22",1000,1000);
  c22->Divide(2,2);
  TString fileName("ExampleTree_0_221.root");
  TString treeName("ntExampleTree_0_221");
  c22->cd(1);
    FSHistogram::getTH1F(fileName,treeName,"Chi2DOF","(60,0.0,6.0)","")->Draw();
  c22->cd(2);
    FSHistogram::getTH2F(fileName,treeName,"Chi2DOF:Event",
         "(100,0.0,100.0,100,0.0,6.0)","")->Draw("colz");
  c22->cd(3);
    FSHistogram::getTH1F(fileName,treeName,"Chi2DOF",
        "(60,0.0,6.0)","Chi2DOF<2.0")->Draw();
  c22->cd(4);
    FSHistogram::getTH1F(fileName,treeName,"MASS(2,4,5)",
        "(100,3.0,3.2)","Chi2DOF<2.0&&abs(MASS(2,4)-1.0)<0.2")->Draw();
}

void examples_sec23(){
  TCanvas* c23 = new TCanvas("c23","c23",1000,1000);
  c23->Divide(2,2);
  TString fileName("ExampleTree_0_221.root");
  TString treeName("ntExampleTree_0_221");
  c23->cd(1);
    TString cutCHI2("Chi2DOF<2.0");
    TString cutMASS("abs(MASS(2,4)-1.0)<0.2");
    FSHistogram::getTH1F(fileName,treeName,"MASS(2,4,5)",
        "(100,3.0,3.2)",cutCHI2+"&&"+cutMASS)->Draw();
  c23->cd(2);
    FSCut::defineCut("chi2",cutCHI2);
    FSCut::defineCut("mass",cutMASS);
    FSHistogram::getTH1F(fileName,treeName,"MASS(2,4,5)",
        "(100,3.0,3.2)","CUT(chi2,mass)")->Draw();
  c23->cd(3);
    TString cutCHI2SB("Chi2DOF>3.0&&Chi2DOF<7.0");
      // the relative size of signal to sideband is 0.5
    FSCut::defineCut("chi2",cutCHI2,cutCHI2SB,0.5);
    FSHistogram::getTH1F(fileName,treeName,"MASS(2,4,5)",
        "(100,3.0,3.2)","CUT(mass)&&CUTSB(chi2)")->Draw();
  c23->cd(4);
    TString cutMASSSB("abs(MASS(2,4)-1.8)<0.6");
      // the relative size of signal to sideband is 1.0/3.0
    FSCut::defineCut("mass",cutMASS,cutMASSSB,1.0/3.0);
    FSHistogram::getTH1F(fileName,treeName,"MASS(2,4,5)",
        "(100,3.0,3.2)","CUTSB(chi2,mass)")->Draw();
}


