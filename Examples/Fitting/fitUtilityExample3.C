{

  TCanvas* c2 = new TCanvas("c2","c2",600,600);
  c2->Divide(1,2);

    // make two histograms and draw them

  TH1F* hist1 = new TH1F("hist1","hist1",100,3.0,4.0);
  for (int i = 0; i < 10000; i++){ hist1->Fill(gRandom->Uniform(3.2,4.0)); }
  for (int i = 0; i < 2000; i++){  hist1->Fill(gRandom->Uniform(3.0,3.2)); }
  for (int i = 0; i < 1000; i++){  hist1->Fill(gRandom->Gaus(3.7,0.05)); }
  hist1->SetTitle("Histogram 1");
  hist1->SetXTitle("Mass");
  hist1->SetYTitle("Events");
  hist1 = FSHistogram::getTH1F(hist1);

  TH1F* hist2 = new TH1F("hist2","hist2",100,3.0,4.0);
  for (int i = 0; i < 10000; i++){ hist2->Fill(gRandom->Uniform(3.2,4.0)); }
  for (int i = 0; i < 3000; i++){  hist2->Fill(gRandom->Uniform(3.0,3.2)); }
  for (int i = 0; i < 1000; i++){  hist2->Fill(gRandom->Gaus(3.5,0.05)); }
  hist2->SetTitle("Histogram 2");
  hist2->SetXTitle("Mass");
  hist2->SetYTitle("Events");
  hist2 = FSHistogram::getTH1F(hist2);

  c2->cd(1);  hist1->Draw();
  c2->cd(2);  hist2->Draw();


   // now do the script

  FSFitUtilities::clear();

    // set up functions

  FSFitUtilities::createFunction(new FSFitPOLY("poly1",0.0,20.0));
  FSFitUtilities::createFunction(new FSFitGAUS("gaus1",0.0,20.0));
  FSFitUtilities::createFunction("pg1","poly1+gaus1");
  FSFitUtilities::setParameters    ("poly1", 120.0, 0.0, 0.0);
  FSFitUtilities::setParameters    ("gaus1",   2.0, 3.8, 0.01);

  FSFitUtilities::createFunction(new FSFitPOLY("poly2",0.0,20.0));
  FSFitUtilities::createFunction(new FSFitGAUS("gaus2",0.0,20.0));
  FSFitUtilities::createFunction("pg2","poly2+gaus2");
  FSFitUtilities::setParameters    ("poly2", 120.0, 0.0, 0.0);
  FSFitUtilities::setParameters    ("gaus2",   2.0, 3.5, 0.01);

  //FSFitUtilities::constrainParameters("poly1:a","poly2:a");
  //FSFitUtilities::constrainParameters("poly1:b","poly2:b");
  //FSFitUtilities::constrainParameters("poly1:c","poly2:c");
  FSFitUtilities::constrainParameters("gaus1:N","gaus2:N");
  FSFitUtilities::constrainParameters("gaus1:W","gaus2:W");


    // draw functions before fit

  c2->cd(1);
  TF1* fpre1 = FSFitUtilities::getTF1("pg1"); fpre1->SetLineStyle(kDashed); fpre1->SetLineColor(kBlue); fpre1->Draw("same");

  c2->cd(2);
  TF1* fpre2 = FSFitUtilities::getTF1("pg2"); fpre2->SetLineStyle(kDashed); fpre2->SetLineColor(kBlue); fpre2->Draw("same");


    // do the fit

  FSFitUtilities::createDataSet("hist1",hist1);
  FSFitUtilities::createDataSet("hist2",hist2);
  FSFitUtilities::createMinuit("minuit");
  FSFitUtilities::addFSFitComponent("minuit","hist1","pg1");
  FSFitUtilities::addFSFitComponent("minuit","hist2","pg2");
  FSFitUtilities::migrad("minuit");


    // draw function after fit

  c2->cd(1);
  FSFitUtilities::getTF1("pg1")->Draw("same");
  TF1* fbg1 = FSFitUtilities::getTF1("poly1"); fbg1->SetLineStyle(kDashed); fbg1->Draw("same");
  TF1* fsg1 = FSFitUtilities::getTF1("gaus1"); fsg1->SetLineStyle(kDotted); fsg1->Draw("same");

  c2->cd(2);
  FSFitUtilities::getTF1("pg2")->Draw("same");
  TF1* fbg2 = FSFitUtilities::getTF1("poly2"); fbg2->SetLineStyle(kDashed); fbg2->Draw("same");
  TF1* fsg2 = FSFitUtilities::getTF1("gaus2"); fsg2->SetLineStyle(kDotted); fsg2->Draw("same");


    // do a minos fit and redraw functions

  FSFitUtilities::minos("minuit");

  c2->cd(1);
  TF1* ftot1minos = FSFitUtilities::getTF1("pg1");  ftot1minos->SetLineColor(kBlack); ftot1minos->Draw("same");

  c2->cd(2);
  TF1* ftot2minos = FSFitUtilities::getTF1("pg2");  ftot2minos->SetLineColor(kBlack); ftot2minos->Draw("same");

  FSFitUtilities::printParameters("pg1");
  FSFitUtilities::printParameters("pg2");

  cout << "FIT STATUS = " << FSFitUtilities::fitStatus("minuit") << endl;

}

