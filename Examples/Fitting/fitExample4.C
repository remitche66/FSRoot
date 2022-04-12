{
  FSFitUtilities::clear();


    // (1) make a histogram and draw it (BLACK)

  TH1F* hist = FSHistogram::getTH1FFormula("1+4.0*TMath::Gaus(x,5.0,0.5,true)","(100,0.0,10.0)",10000);
  hist->Draw();


    // (2) create functions and draw them (PURPLE)

  FSFitUtilities::createFunction(new FSFitPOLY("p",0.0,10.0,1,3),50.0,0.0);
  FSFitUtilities::createFunction(new FSFitGAUS("g",0.0,10.0),50.0,4.0,0.4);
  FSFitUtilities::createFunction("pg","p+g");
  TF1* fpg2 = FSFitUtilities::getTF1("pg");
  fpg2->SetLineColor(6); fpg2->SetLineStyle(kDotted);
  fpg2->Draw("same");


    // (3) fit the polynomial background in a limited range and draw it (GREEN)

  FSFitUtilities::addFitRange(0.0,4.5);
  FSFitUtilities::addFitRange(5.5,10.0);
  FSFitUtilities::migrad(hist,"p");
  TF1* fp3 = FSFitUtilities::getTF1("p");
  fp3->SetLineColor(kGreen); fp3->SetLineStyle(kDotted);
  fp3->Draw("same");


    // (4) fix the polynomial background and fit the gaussian part (BLUE)

  FSFitUtilities::clearFitRange();
  FSFitUtilities::fixParameters("p");
  FSFitUtilities::migrad(hist,"pg");
  TF1* fpg4 = FSFitUtilities::getTF1("pg");
  fpg4->SetLineColor(kBlue); fpg4->SetLineStyle(kDotted);
  fpg4->Draw("same");


    // (5) float everything (RED)

  FSFitUtilities::releaseParameters("p");
  FSFitUtilities::migrad(hist,"pg");
  TF1* fpg5 = FSFitUtilities::getTF1("pg");
  fpg5->SetLineColor(kRed); fpg5->SetLineStyle(kSolid);
  fpg5->Draw("same");

}

