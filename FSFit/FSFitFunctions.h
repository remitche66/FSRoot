#if !defined(FSFITFUNCTIONS_H)
#define FSFITFUNCTIONS_H

#include "FSFit/FSFitPrivate.h"

using namespace std;



class FSFitPOLY : public FSFitFunction{
  public:

    FSFitPOLY(TString n_fName, double n_xLow, double n_xHigh, int n_order = 2, double n_central = 0.0) :
      FSFitFunction(n_fName,n_xLow,n_xHigh){ 
      m_order = n_order;
      m_central = n_central;
      if (m_order >= 0) addParameter("a",10.0,"variable a");
      if (m_order >= 1) addParameter("b", 0.0,"variable b");
      if (m_order >= 2) addParameter("c", 0.0,"variable c");
      if (m_order >= 3) addParameter("d", 0.0,"variable d");
      if (m_order >= 4) addParameter("e", 0.0,"variable e");
      if (m_order >= 5) addParameter("f", 0.0,"variable f");
      if (m_order >= 6) addParameter("g", 0.0,"variable g");
      for (int i = 0; i < 7; i++){ m_pars[i] = 0.0; }
    }

    double fx (double x){
      if (m_order >= 0) m_pars[0] = getParameterValue("a");
      if (m_order >= 1) m_pars[1] = getParameterValue("b");
      if (m_order >= 2) m_pars[2] = getParameterValue("c");
      if (m_order >= 3) m_pars[3] = getParameterValue("d");
      if (m_order >= 4) m_pars[4] = getParameterValue("e");
      if (m_order >= 5) m_pars[5] = getParameterValue("f");
      if (m_order >= 6) m_pars[6] = getParameterValue("g");
      return  m_pars[0] +
              m_pars[1]*pow((x-m_central),1) +
              m_pars[2]*pow((x-m_central),2) +
              m_pars[3]*pow((x-m_central),3) +
              m_pars[4]*pow((x-m_central),4) +
              m_pars[5]*pow((x-m_central),5) +
              m_pars[6]*pow((x-m_central),6);
    }

    double integral(double x1, double x2){
      if (m_order >= 0) m_pars[0] = getParameterValue("a");
      if (m_order >= 1) m_pars[1] = getParameterValue("b");
      if (m_order >= 2) m_pars[2] = getParameterValue("c");
      if (m_order >= 3) m_pars[3] = getParameterValue("d");
      if (m_order >= 4) m_pars[4] = getParameterValue("e");
      if (m_order >= 5) m_pars[5] = getParameterValue("f");
      if (m_order >= 6) m_pars[6] = getParameterValue("g");
      return (          m_pars[0]*x2 +
              1.0/2.0 * m_pars[1]*pow((x2-m_central),2) +
              1.0/3.0 * m_pars[2]*pow((x2-m_central),3) +
              1.0/4.0 * m_pars[3]*pow((x2-m_central),4) +
              1.0/5.0 * m_pars[4]*pow((x2-m_central),5) +
              1.0/6.0 * m_pars[5]*pow((x2-m_central),6) +
              1.0/7.0 * m_pars[6]*pow((x2-m_central),7) -
                        m_pars[0]*x1 -
              1.0/2.0 * m_pars[1]*pow((x1-m_central),2) -
              1.0/3.0 * m_pars[2]*pow((x1-m_central),3) -
              1.0/4.0 * m_pars[3]*pow((x1-m_central),4) -
              1.0/5.0 * m_pars[4]*pow((x1-m_central),5) -
              1.0/6.0 * m_pars[5]*pow((x1-m_central),6) -
              1.0/7.0 * m_pars[6]*pow((x1-m_central),7) );
    }

    FSFitPOLY* clone(){ return new FSFitPOLY("",m_xLow,m_xHigh,m_order,m_central); }

  private:

    int m_order;
    double m_central;
    double m_pars[7];

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
