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


class FSFitHIST2 : public FSFitFunction{
  public:

    FSFitHIST2(TString n_fName, TH1F* hist) :
      FSFitFunction(n_fName,-1.0,-1.0){ 
      addParameter("S",1.0,"Scale");
      addParameter("shift",0.0,"shift");
      addParameter("sigma",0.0,"smearing sigma");
      m_hist = hist;
      m_nBins = hist->GetNbinsX();
      m_binsize = m_hist->GetBinWidth(1);
      m_shift = 0.0;
      m_sigma = 0.0;
      setXLow(m_hist->GetBinLowEdge(1));
      setXHigh(m_hist->GetBinLowEdge(m_nBins)+m_hist->GetBinWidth(1));
      if (m_nBins > m_MAXBINS){ cout << "ERROR in FSFitHIST2: Too many bins" << endl;  exit(0); }
      m_histOriginal = (TH1F*) m_hist->Clone();
      m_histShifted = (TH1F*) m_hist->Clone();
      m_histSmeared = (TH1F*) m_hist->Clone();
    }

    double fx (double x){
      updateHistSmeared();
      double S = getParameterValue("S");
      int iBin = (int) ((x-m_xLow)/((m_xHigh-m_xLow)/m_nBins)) + 1;
      return S*m_histSmeared->GetBinContent(iBin);
    }

    double efx (double x){
      updateHistSmeared();
      double S = getParameterValue("S");
      int iBin = (int) ((x-m_xLow)/((m_xHigh-m_xLow)/m_nBins)) + 1;
      return S*m_histSmeared->GetBinError(iBin);
    }

    double integral (double x1, double x2){
      updateHistSmeared();
      double S = getParameterValue("S");
      double total = 0.0;
      for (int iBin = 1; iBin <= m_nBins; iBin++){
        double x = m_histSmeared->GetBinCenter(iBin);
        if ((x >= x1) && (x <= x2)) total += m_histSmeared->GetBinContent(iBin);
      }
      return S*m_histSmeared->GetBinWidth(1)*total;
    }

    FSFitHIST2* clone(){ return new FSFitHIST2("",m_hist); }

  private:

    void updateHistSmeared(){
      double shift = getParameterValue("shift");
      double sigma = getParameterValue("sigma");
      if (shift == m_shift && sigma == m_sigma) return;
      if (shift == m_shift && sigma != m_sigma){ m_sigma = sigma; smearHist(); return; }
      if (shift != m_shift){ m_shift = shift; m_sigma = sigma; shiftHist(); smearHist(); }
    }

    void addFromOriginalToShifted(int iOriginal, int iShifted, double fraction){
      if (iOriginal < 1 || iOriginal > m_nBins || iShifted < 1 || iShifted > m_nBins) return;
      double yO  = fraction*m_histOriginal->GetBinContent(iOriginal);
      double eyO = fraction*m_histOriginal->GetBinError(iOriginal);
      double yM  = m_histShifted->GetBinContent(iShifted);
      double eyM = m_histShifted->GetBinError(iShifted);
      m_histShifted->SetBinContent(iShifted,yO+yM);
      m_histShifted->SetBinError(iShifted,sqrt(eyO*eyO+eyM+eyM));
    }

    void addFromShiftedToSmeared(int iShifted, int iSmeared, double fraction){
      if (iShifted < 1 || iShifted > m_nBins || iSmeared < 1 || iSmeared > m_nBins) return;
      double yO  = fraction*m_histShifted->GetBinContent(iShifted);
      double eyO = fraction*m_histShifted->GetBinError(iShifted);
      double yM  = m_histSmeared->GetBinContent(iSmeared);
      double eyM = m_histSmeared->GetBinError(iSmeared);
      m_histSmeared->SetBinContent(iSmeared,yO+yM);
      m_histSmeared->SetBinError(iSmeared,sqrt(eyO*eyO+eyM+eyM));
    }

    void shiftHist(){
      //cout << "SHIFTING HISTOGRAM BY " << m_shift << endl;
      m_histShifted->Reset();
      int ishift1 = (int)(m_shift/m_binsize);  
      int ishift2 = ishift1 + 1;  if (m_shift < 0.0) ishift2 = ishift1 - 1;  
      double frac2 = fabs(m_shift/m_binsize) - (int)fabs(m_shift/m_binsize);
      double frac1 = 1.0 - frac2;
      for (int i = 1; i <= m_nBins; i++){
        addFromOriginalToShifted(i,i+ishift1,frac1);
        addFromOriginalToShifted(i,i+ishift2,frac2);
      }
    }

    void smearHist(){
      //cout << "SMEARING HISTOGRAM BY " << m_sigma << endl;
      m_histSmeared->Reset();
      double x1 = -0.5*m_binsize;
      double x2 =  0.5*m_binsize;
      for (int i = 0; i < m_MAXBINS; i++){
        m_smear[i] = 0.5*(TMath::Erf(x2/sqrt(2.0)/m_sigma)-TMath::Erf(x1/sqrt(2.0)/m_sigma));
        if (m_smear[i] < 1.0e-7) break;
        x1 += m_binsize;  x2 += m_binsize;
      }
      for (int j = 1; j <= m_nBins; j++){
        for (int i = 0; i < m_MAXBINS; i++){
          if (m_smear[i] < 1.0e-7) break;
          addFromShiftedToSmeared(j,j+i,m_smear[i]);
          if (i > 0) addFromShiftedToSmeared(j,j-i,m_smear[i]);
        }
      }
    }

    TH1F* m_hist;
    TH1F* m_histOriginal;
    TH1F* m_histShifted;
    TH1F* m_histSmeared;
    int m_nBins;
    double m_binsize;
    double m_shift;
    double m_sigma;
    static const int m_MAXBINS = 1000;
    double m_smear[m_MAXBINS];

};


class FSFitRELBW : public FSFitFunction{
  public:
    FSFitRELBW(TString n_fName, double n_xLow, double n_xHigh) :
      FSFitFunction(n_fName,n_xLow,n_xHigh){ 
      addParameter("N",10.0,"size");
      addParameter("M",1.0,"mass");
      addParameter("W",0.1,"width");
    }
    double fx (double x){
      double N = getParameterValue("N");
      double M = getParameterValue("M");
      double W = getParameterValue("W");
      double g = sqrt(M*M*(M*M+W*W));
      double k = 2.0*sqrt(2.0)*M*W*g/3.141592654/sqrt(M*M+g);
      double f = N*k/((x*x-M*M)*(x*x-M*M) + M*M*W*W);
      if (f > 0) return f;
      return 1.0e-12;
    }
    FSFitRELBW* clone(){ return new FSFitRELBW("",m_xLow,m_xHigh); }
};


class FSFit2RELBW : public FSFitFunction{
  public:
    FSFit2RELBW(TString n_fName, double n_xLow, double n_xHigh) :
      FSFitFunction(n_fName,n_xLow,n_xHigh){ 
      addParameter("N1",10.0,"size1");
      addParameter("M1",1.0,"mass1");
      addParameter("W1",0.1,"width1");
      addParameter("N2",10.0,"size2");
      addParameter("M2",1.0,"mass2");
      addParameter("W2",0.1,"width2");
      addParameter("PHI12",0.0,"relative phase");
    }
    double fx (double x){
      double N1 = getParameterValue("N1");
      double N2 = getParameterValue("N2");
      double M1 = getParameterValue("M1");
      double M2 = getParameterValue("M2");
      double W1 = getParameterValue("W1");
      double W2 = getParameterValue("W2");
      double PHI12 = getParameterValue("PHI12");
      double g1 = sqrt(M1*M1*(M1*M1+W1*W1));
      double g2 = sqrt(M2*M2*(M2*M2+W2*W2));
      double k1 = 2.0*sqrt(2.0)*M1*W1*g1/3.141592654/sqrt(M1*M1+g1);
      double k2 = 2.0*sqrt(2.0)*M2*W2*g2/3.141592654/sqrt(M2*M2+g2);
      complex<double> d1 = complex<double>(x*x - M1*M1, M1*W1);
      complex<double> d2 = complex<double>(x*x - M2*M2, M2*W2);
      if ((N1*k1 < 0) || (N2*k2 < 0)) return 1.0e-12;
      complex<double> a1 = sqrt(N1*k1) / d1;
      complex<double> a2 = sqrt(N2*k2) / d2;
      complex<double> phase12 = complex<double>(cos(PHI12),sin(PHI12));
      double f1 = norm(a1);
      double f2 = norm(a2);
      double f = norm(a1 + phase12*a2);
      if (f > 0) return f;
      return 1.0e-12;
    }
    FSFit2RELBW* clone(){ return new FSFit2RELBW("",m_xLow,m_xHigh); }
};


#endif
