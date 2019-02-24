{

  TH1F* hist = new TH1F("example","Example with Parameters and Functions",10,0.0,20.0);
  hist = FSHistogram::getTH1F(hist);
  hist->SetMaximum(10.0);
  hist->Draw();

  FSFitUtilities::clear();


    // make a function: polynomial - gaussian

  FSFitUtilities::createFunction(new FSFitGAUS("gaus",0.0,20.0));
  FSFitUtilities::createFunction(new FSFitPOLY("poly",0.0,20.0));
  FSFitUtilities::createFunction("pg","poly-gaus");
  FSFitUtilities::setParameters("pg",  9.0, 0.001, 0.001,   9.0, 9.0, 1.0);
  FSFitUtilities::printParameters("pg");

    // draw all the components of the function (RED)

  TF1* fpg = FSFitUtilities::getTF1("pg"); fpg->SetLineColor(kRed); fpg->Draw("same");
  TF1* fg = FSFitUtilities::getTF1("gaus"); fg->SetLineColor(kRed);  fg->Draw("same");
  TF1* fp = FSFitUtilities::getTF1("poly"); fp->SetLineColor(kRed); fp->SetLineStyle(kDashed); fp->Draw("same");


    // change the parameters and redraw (BLUE)

  FSFitUtilities::setParameters("pg",  4.5, 0.002, 0.002,   3.0, 16.0, 1.0);
  FSFitUtilities::printParameters("pg");
  fpg = FSFitUtilities::getTF1("pg"); fpg->SetLineColor(kBlue); fpg->Draw("same");
  fg = FSFitUtilities::getTF1("gaus");  fg->SetLineColor(kBlue); fg->Draw("same");
  fp = FSFitUtilities::getTF1("poly");  fp->SetLineColor(kBlue); fp->SetLineStyle(kDashed); fp->Draw("same");


    // add another gaussian and redraw (GREEN)

  FSFitUtilities::createFunction(new FSFitGAUS("gaus2",0.0,20.0));
  FSFitUtilities::createFunction("pgg","pg+gaus2");
  FSFitUtilities::setParameters("gaus2", 4.0, 2.0, 0.5);
  FSFitUtilities::setParameter("poly:a",3.7);
  FSFitUtilities::printParameters("pgg");
  TF1* fpgg = FSFitUtilities::getTF1("pgg"); fpgg->SetLineColor(kGreen); fpgg->Draw("same");


    // constrain the gaussian widths and redraw (BLACK)

  FSFitUtilities::constrainParameters("gaus:W","gaus2:W",0.4);
  TF1* fpgg2 = FSFitUtilities::getTF1("pgg"); fpgg2->SetLineColor(kBlack); fpgg2->Draw("same");

}


