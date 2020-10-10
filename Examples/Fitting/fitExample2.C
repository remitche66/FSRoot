{


    // make a histogram and draw it

  TH1F* hist = new TH1F("hist","hist",100,3.0,4.0);
  for (int i = 0; i < 10000; i++){ hist->Fill(gRandom->Uniform(3.2,4.0)); }
  for (int i = 0; i < 2000; i++){  hist->Fill(gRandom->Uniform(3.0,3.2)); }
  for (int i = 0; i < 1000; i++){  hist->Fill(gRandom->Gaus(3.772,0.05)); }
  hist->SetTitle("Example Fit");
  hist->SetXTitle("Mass");
  hist->SetYTitle("Events");
  hist = FSHistogram::getTH1F(hist);
  hist->Draw();


   // now do the script

  FSFitUtilities::clear();

    // set up function

  FSFitUtilities::createFunction(new FSFitPOLY("poly",0.0,20.0));
  FSFitUtilities::createFunction(new FSFitGAUS("gaus",0.0,20.0));
  FSFitUtilities::createFunction("pg","poly+gaus");
  FSFitUtilities::setParameters    ("poly", 120.0, 0.0, 0.0);
  //FSFitUtilities::setParameterSteps("poly",   1.0, 1.0, 1.0);
  FSFitUtilities::setParameters    ("gaus",   2.0, 3.8, 0.01);
  //FSFitUtilities::setParameterSteps("gaus",   1.0, 0.1, 0.001);
  //FSFitUtilities::constrainParameters("poly:b","poly:c");
  FSFitUtilities::fixParameter("gaus:M",3.78);


    // get function before fit

  TF1* fpre = FSFitUtilities::getTF1("pg"); fpre->SetLineStyle(kDashed); fpre->SetLineColor(kBlue);


    // do the fit

  FSFitUtilities::createDataSet("hist",hist);
  FSFitUtilities::createMinuit("minuit");
  FSFitUtilities::addFitComponent("minuit","hist","pg");
  FSFitUtilities::migrad("minuit");


    // draw function after fit

  FSFitUtilities::getTF1("pg")->Draw("same");
  TF1* fbg = FSFitUtilities::getTF1("poly"); fbg->SetLineStyle(kDashed); fbg->Draw("same");
  TF1* fsg = FSFitUtilities::getTF1("gaus"); fsg->SetLineStyle(kDotted); fsg->Draw("same");


    // draw function as it was before the fit

  fpre->Draw("same");

    // print out some information about the fit

  cout << endl << endl << "**** INFORMATION ABOUT THE FIT ****" << endl;
  cout << "fcnValue          = " << FSFitUtilities::fcnValue("minuit") << endl;
  cout << "likelihood        = " << FSFitUtilities::likelihood("minuit") << endl;
  cout << "chisquare         = " << FSFitUtilities::chisquare("minuit") << endl;
  cout << "nDataPoints       = " << FSFitUtilities::nDataPoints("minuit") << endl;
  cout << "nFreeParameters   = " << FSFitUtilities::nFreeParameters("minuit") << endl;
  cout << "nDOF              = " << FSFitUtilities::nDOF("minuit") << endl;
  cout << "fitStatus         = " << FSFitUtilities::fitStatus("minuit") << endl;
  cout << "***********************************" << endl << endl;

    // print out some example parameter information

  cout << "**** EXAMPLE PARAMETER INFORMATION ****" << endl;
  cout << "  [information about the gaussian width (gaus:W)]" << endl;
  cout << "parameterValue                  = " << FSFitUtilities::getParameterValue("gaus:W") << endl;
  cout << "parameterError                  = " << FSFitUtilities::getParameterError("gaus:W") << endl;
  cout << "parameterErrorLow               = " << FSFitUtilities::getParameterErrorLow("gaus:W") << endl;
  cout << "parameterErrorHigh              = " << FSFitUtilities::getParameterErrorHigh("gaus:W") << endl;
  cout << "correlatedError (gaus:W,gaus:N) = " << FSFitUtilities::getCorrelatedError("minuit","gaus:W","gaus:N") << endl;
  cout << "correlatedError (poly:b,poly:c) = " << FSFitUtilities::getCorrelatedError("minuit","poly:b","poly:c") << endl;
  cout << "***************************************" << endl;

}

