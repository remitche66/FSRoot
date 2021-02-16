TString FN("$FSAMPTOOLS/run/phasespace.gen.root");
TString NT("nt");
TString CFG("$FSAMPTOOLS/run/fsamptools1.cfg");
TString RCT("fsamptools");



void dalitz(){

  FSAmpTools::setupFromConfigFile(CFG);

  FSAmpTools::showAmpNames();

  FSAmpTools::defineAmpWt("INR12","*R12");
  FSAmpTools::defineAmpWt("INR13","*R13");
  FSAmpTools::defineAmpWt("INTOT","*");
  FSAmpTools::defineAmpWt("RER12","*R12");
  FSAmpTools::defineAmpWt("RER13","*R13");
  FSAmpTools::defineAmpWt("RETOT","*");
  FSAmpTools::defineAmpWt("IMR12","*R12");
  FSAmpTools::defineAmpWt("IMR13","*R13");
  FSAmpTools::defineAmpWt("IMTOT","*",true);

  FSAmpTools::makeAmpWts(FN,NT,RCT,3);

}



void plot1d(TString var, TString bounds){
  FSTree::addFriendTree("AmpWts");
  TCanvas* c31d = new TCanvas("c31d","c31d",2000,700);
  c31d->Divide(3,1);
  c31d->cd(1);
  TH1F* hINTOT = FSHistogram::getTH1F(FN,NT,var,bounds,"INTOT");
  TH1F* hINR12 = FSHistogram::getTH1F(FN,NT,var,bounds,"INR12");
  TH1F* hINR13 = FSHistogram::getTH1F(FN,NT,var,bounds,"INR13");
  FSHistogram::setHistogramMaxima(hINTOT,hINR12,hINR13);
  FSHistogram::setHistogramMinima(hINTOT,hINR12,hINR13);
  hINR12->SetLineColor(kBlue);
  hINR13->SetLineColor(kRed);
  hINTOT->SetTitle("Intensity");
  hINTOT->SetXTitle(var);
  hINTOT->Draw("hist");
  hINR12->Draw("hist,same");
  hINR13->Draw("hist,same");
  c31d->cd(2);
  TH1F* hRETOT = FSHistogram::getTH1F(FN,NT,var,bounds,"RETOT");
  TH1F* hRER12 = FSHistogram::getTH1F(FN,NT,var,bounds,"RER12");
  TH1F* hRER13 = FSHistogram::getTH1F(FN,NT,var,bounds,"RER13");
  FSHistogram::setHistogramMaxima(hRETOT,hRER12,hRER13);
  FSHistogram::setHistogramMinima(hRETOT,hRER12,hRER13);
  hRER12->SetLineColor(kBlue);
  hRER13->SetLineColor(kRed);
  hRETOT->SetTitle("Real Part");
  hRETOT->SetXTitle(var);
  hRETOT->Draw("hist");
  hRER12->Draw("hist,same");
  hRER13->Draw("hist,same");
  c31d->cd(3);
  TH1F* hIMTOT = FSHistogram::getTH1F(FN,NT,var,bounds,"IMTOT");
  TH1F* hIMR12 = FSHistogram::getTH1F(FN,NT,var,bounds,"IMR12");
  TH1F* hIMR13 = FSHistogram::getTH1F(FN,NT,var,bounds,"IMR13");
  FSHistogram::setHistogramMaxima(hIMTOT,hIMR12,hIMR13);
  FSHistogram::setHistogramMinima(hIMTOT,hIMR12,hIMR13);
  hIMR12->SetLineColor(kBlue);
  hIMR13->SetLineColor(kRed);
  hIMTOT->SetTitle("Imaginary Part");
  hIMTOT->SetXTitle(var);
  hIMTOT->Draw("hist");
  hIMR12->Draw("hist,same");
  hIMR13->Draw("hist,same");
}

void plot2d(TString var, TString bounds){
  FSTree::addFriendTree("AmpWts");
  TCanvas* c32d = new TCanvas("c32d","c32d",2000,700);
  c32d->Divide(3,1);
  c32d->cd(1);
  TH2F* hINTOT = FSHistogram::getTH2F(FN,NT,var,bounds,"INTOT");
  hINTOT->SetTitle("Intensity");
  hINTOT->GetZaxis()->SetLabelSize(0.03);
  hINTOT->Draw("colz");
  c32d->cd(2);
  TH2F* hRETOT = FSHistogram::getTH2F(FN,NT,var,bounds,"RETOT");
  hRETOT->SetTitle("Real Part");
  hRETOT->GetZaxis()->SetLabelSize(0.03);
  hRETOT->Draw("colz");
  c32d->cd(3);
  TH2F* hIMTOT = FSHistogram::getTH2F(FN,NT,var,bounds,"IMTOT");
  hIMTOT->SetTitle("Imaginary Part");
  hIMTOT->GetZaxis()->SetLabelSize(0.03);
  hIMTOT->Draw("colz");
}

void plots(){
  plot1d("MASS(1,3)","(100,0.0,3.0)");
  plot2d("MASS2(1,3):MASS2(1,2)","(100,0.0,10.0,100,0.0,10.0)");
}
