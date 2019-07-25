#if !defined(FSFITFUNCTIONS_H)
#define FSFITFUNCTIONS_H

#include "FSFit/FSFitPrivate.h"

using namespace std;



class FSFitPOLY : public FSFitFunction{
  public:

    FSFitPOLY(TString n_fName, double n_xLow, double n_xHigh) :
      FSFitFunction(n_fName,n_xLow,n_xHigh){ 
      addParameter("a",10.0,"variable a");
      addParameter("b", 0.0,"variable b");
      addParameter("c", 0.0,"variable c");
    }

    double fx (double x){
      double a = getParameterValue("a");
      double b = getParameterValue("b");
      double c = getParameterValue("c");
      return (a + b*x + c*x*x);
    }

    double integral(double x1, double x2){
      double a = getParameterValue("a");
      double b = getParameterValue("b");
      double c = getParameterValue("c");
      return (   a*x2 + 1.0/2.0*b*x2*x2 + 1.0/3.0*c*x2*x2*x2
               - a*x1 - 1.0/2.0*b*x1*x1 - 1.0/3.0*c*x1*x1*x1 );
    }

    FSFitPOLY* clone(){ return new FSFitPOLY("",m_xLow,m_xHigh); }

};



class FSFitGAUS : public FSFitFunction{
  public:

    FSFitGAUS(TString n_fName, double n_xLow, double n_xHigh) :
      FSFitFunction(n_fName,n_xLow,n_xHigh){ 
      addParameter("N",10.0,"size");
      addParameter("M",10.0,"mean");
      addParameter("W",10.0,"width");
    }

    double fx (double x){
      double N = getParameterValue("N");
      double M = getParameterValue("M");
      double W = getParameterValue("W");
      if (W == 0.0) return 1.0e-6;
      return N/W/sqrt(2.0*3.141592654)*exp(-0.5*(x-M)*(x-M)/W/W);
    }

    double integral (double x1, double x2){
      double N = getParameterValue("N");
      double M = getParameterValue("M");
      double W = getParameterValue("W");
      if ((x1 < M - 5.0*W) && (x2 > M + 5.0*W)) return N;
      cout << "FSFitGAUS ERROR:  integral is not defined" << endl;
      exit(0);
      return -1.0;
    }

    FSFitGAUS* clone(){ return new FSFitGAUS("",m_xLow,m_xHigh); }

};


class FSFitARGUS : public FSFitFunction{
  public:

    FSFitARGUS(TString n_fName, double n_xLow, double n_xHigh) :
      FSFitFunction(n_fName,n_xLow,n_xHigh){ 
      addParameter("a",10.0,"a");
      addParameter("b",10.0,"b");
      addParameter("T",10.0,"Threshold");
    }

    double fx (double x){
      double a = getParameterValue("a");
      double b = getParameterValue("b");
      double T = getParameterValue("T");
      if (x >= T) return 0.0;
      return a*x*sqrt(1.0-x*x/T/T) * exp(-1.0*b*(1.0-x*x/T/T));
    }

    FSFitARGUS* clone(){ return new FSFitARGUS("",m_xLow,m_xHigh); }

};


class FSFitHIST : public FSFitFunction{
  public:

    FSFitHIST(TString n_fName, TH1F* hist) :
      FSFitFunction(n_fName,-1.0,-1.0){ 
      m_hist = hist;
      m_nBins = hist->GetNbinsX();
      setXLow(m_hist->GetBinLowEdge(1));
      setXHigh(m_hist->GetBinLowEdge(m_nBins)+m_hist->GetBinWidth(1));
      addParameter("S",1.0,"Scale");
    }

    double fx (double x){
      double S = getParameterValue("S");
      int iBin = (int) ((x-m_xLow)/((m_xHigh-m_xLow)/m_nBins)) + 1;
      return S*m_hist->GetBinContent(iBin);
    }

    double efx (double x){
      double S = getParameterValue("S");
      int iBin = (int) ((x-m_xLow)/((m_xHigh-m_xLow)/m_nBins)) + 1;
      return S*m_hist->GetBinError(iBin);
    }

    double integral (double x1, double x2){
      double S = getParameterValue("S");
      double total = 0.0;
      for (int iBin = 1; iBin <= m_nBins; iBin++){
        double x = m_hist->GetBinCenter(iBin);
        if ((x >= x1) && (x <= x2)) total += m_hist->GetBinContent(iBin);
      }
      return S*m_hist->GetBinWidth(1)*total;
    }

    FSFitHIST* clone(){ return new FSFitHIST("",m_hist); }

  private:

    TH1F* m_hist;
    int m_nBins;

};


#endif
