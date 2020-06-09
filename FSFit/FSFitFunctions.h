#if !defined(FSFITFUNCTIONS_H)
#define FSFITFUNCTIONS_H
#include <complex>
#include <math.h>
#include "FSFit/FSFitPrivate.h"

using namespace std;

complex<double> chewMandel(double Ecm, double m, double mu, bool debug=false){
  double pi = TMath::Pi();
  double s = Ecm*Ecm;
  double m_plus = (m+mu)*(m+mu);
  double m_minus = abs((m-mu)*(m-mu));
  complex<double> comp_plus(0,0);
  complex<double> comp_minus(0,0);
  if(m_plus-s>0){
    comp_plus = complex<double>(sqrt(m_plus-s),0);
  }else{
    comp_plus = complex<double>(0,sqrt(s-m_plus));
  }
  if(m_minus-s>0){
    comp_minus = complex<double>(sqrt(m_minus-s),0);
  }else{
    comp_minus = complex<double>(0,sqrt(-(m_minus-s)));
  }

  double mmu = m*mu;

  //complex<double> logTerm(real(log(((comp_plus+comp_minus)/(2.0*sqrt(mmu))))),imag(log(((comp_plus+comp_minus)/(2.0*sqrt(mmu))))));
  complex<double> logTerm(0,0);
  logTerm = log((comp_plus+comp_minus)/(2.0*sqrt(mmu)));
  
  double divergy = ((m*m+mu*mu)/(2.0*(m*m-mu*mu)))*log(m/mu);
  if(abs(m-mu)<0.0000000001){
    divergy = 0.5;
  }
  complex<double> value = (-2.0/pi)*((-1.0/s)*comp_plus*comp_minus*logTerm+((m*m-mu*mu)/(2.0*s))*log(m/mu)-divergy-0.5);

  if(debug){
    cout << "------------" << endl;
    cout << "1.0/s is " << 1.0/s << endl;
    cout << "comp_plus is " << comp_plus << endl;
    cout << "comp_minus is " << comp_minus << endl;
    cout << "ln part " << log((comp_plus+comp_minus)/(2.0*sqrt(m*mu))) << endl;
    cout << "(m^2+mu^2)/(2(m^2-mu^2))ln(m/mu) is " << (m*m+mu*mu)/(2.0*(m*m-mu*mu))*log(m/mu) << endl;
    cout << "value is " << value << endl;
  }
  
  return conj(value);

}

// norm of the BW
class FSFitBerger : public FSFitFunction{
 public:

  FSFitBerger(TString n_fName, double n_xLow, double n_xHigh) :
    FSFitFunction(n_fName,n_xLow,n_xHigh){ 
    addParameter("m1_squared",10.0,"variable M");
    addParameter("f1_squared", 0.0,"variable f");
    addParameter("N",1.0,"variable N");
  }

  double fx (double x){
    double ecm = x;
    double m1_squared = getParameterValue("m1_squared");
    double f1_squared = getParameterValue("f1_squared");
    double scale = getParameterValue("N");

    
     double mass_pi = 0.135;
     double mass_d = 1.86483;
     double mass_dstar = 2.00685;
     
     double numerator = -f1_squared*(ecm*ecm-pow((mass_d+mass_pi),2));
     //double numerator = 1.0;

     complex<double> sigma((ecm*ecm-pow((mass_pi+mass_d),2))*real(chewMandel(ecm, mass_d,mass_pi)), (ecm*ecm-pow((mass_pi+mass_d),2))*imag(chewMandel(ecm, mass_d,mass_pi)));
  
     complex<double> denom(ecm*ecm-m1_squared+f1_squared*sigma);

     return scale*norm(numerator/denom);
  }

  FSFitBerger* clone(){ return new FSFitBerger("",m_xLow,m_xHigh); }

};




// norm of the BW
class FSFitCM : public FSFitFunction{
 public:

  FSFitCM(TString n_fName, double n_xLow, double n_xHigh) :
    FSFitFunction(n_fName,n_xLow,n_xHigh){ 
    addParameter("M",10.0,"variable M");
    addParameter("mu", 0.0,"variable mu");
    addParameter("N",1.0,"variable N");
  }

  double fx (double x){
    // now a is the number of bkg events
    double m = getParameterValue("M");
    double mu = getParameterValue("mu");
    double scale = getParameterValue("N");
    double pi = TMath::Pi();
    double s = x*x;
    double m_plus = (m+mu)*(m+mu);
    double m_minus = abs((m-mu)*(m-mu));
    complex<double> comp_plus(0,0);
    complex<double> comp_minus(0,0);
    if(m_plus-s>0){
      comp_plus = complex<double>(sqrt(m_plus-s),0);
    }else{
      comp_plus = complex<double>(0,sqrt(s-m_plus));
    }
    if(m_minus-s>0){
      comp_minus = complex<double>(sqrt(m_minus-s),0);
    }else{
      comp_minus = complex<double>(0,sqrt(-(m_minus-s)));
    }
    
    double mmu = m*mu;
    
    complex<double> logTerm(0,0);
    logTerm = log((comp_plus+comp_minus)/(2.0*sqrt(mmu)));
  
    double massTerm = ((m*m+mu*mu)/(2.0*(m*m-mu*mu)))*log(m/mu);

    complex<double> value = (-2.0/pi)*((-1.0/s)*comp_plus*comp_minus*logTerm+((m*m-mu*mu)/(2.0*s))*log(m/mu)-massTerm-0.5);
  
    return scale*norm(value);
  }

  FSFitCM* clone(){ return new FSFitCM("",m_xLow,m_xHigh); }

};









// norm of the BW
class FSFitBW : public FSFitFunction{
 public:

  FSFitBW(TString n_fName, double n_xLow, double n_xHigh) :
    FSFitFunction(n_fName,n_xLow,n_xHigh){ 
    addParameter("M",10.0,"variable M");
    addParameter("W", 0.0,"variable W");
    addParameter("N",1.0,"variable N");
  }

  double fx (double x){
    // now a is the number of bkg events
    double M = getParameterValue("M");
    double W = getParameterValue("W");
    double scale = getParameterValue("N");
    
    double gamma = sqrt(M*M*(M*M+W*W));
  
    double k = 2*sqrt(2)*M*W*gamma/(3.14*sqrt(M*M+gamma));

    complex<double> N(k/(pow((x*x-M*M),2)+M*M*W*W));
  
    
    return scale*norm(N);
  }

  FSFitBW* clone(){ return new FSFitBW("",m_xLow,m_xHigh); }

};



class FSFitPOLY1 : public FSFitFunction{
 public:

  FSFitPOLY1(TString n_fName, double n_xLow, double n_xHigh) :
    FSFitFunction(n_fName,n_xLow,n_xHigh){ 
    addParameter("a",10.0,"variable a");
    addParameter("b", 0.0,"variable b");
  }

  double fx (double x){
    // now a is the number of bkg events
    double a = getParameterValue("a");
    double b = getParameterValue("b");
    return a+b*(x-3.872);
  }

  FSFitPOLY1* clone(){ return new FSFitPOLY1("",m_xLow,m_xHigh); }

};

class FSFitPOLY2 : public FSFitFunction{
 public:

  FSFitPOLY2(TString n_fName, double n_xLow, double n_xHigh) :
    FSFitFunction(n_fName,n_xLow,n_xHigh){ 
    addParameter("a",10.0,"variable a");
    addParameter("b", 0.0,"variable b");
    addParameter("c", 0.0,"variable c");
  }

  double fx (double x){
    double a = getParameterValue("a");
    double b = getParameterValue("b");
    double c = getParameterValue("c");
    return (a + b*(x -3.872)+ c*(x-3.872)*(x-3.872));
  }

  FSFitPOLY2* clone(){ return new FSFitPOLY2("",m_xLow,m_xHigh); }

};

class FSFitPOLY2b : public FSFitFunction{
 public:

  FSFitPOLY2b(TString n_fName, double n_xLow, double n_xHigh) :
    FSFitFunction(n_fName,n_xLow,n_xHigh){ 
    addParameter("a",10.0,"variable a");
    addParameter("b", 0.0,"variable b");
    addParameter("c", 0.0,"variable c");
  }

  double fx (double x){
    double a = getParameterValue("a");
    double b = getParameterValue("b");
    double c = getParameterValue("c");
    return a*(1 + (x-b)+ (x*x-c));
  }

  FSFitPOLY2b* clone(){ return new FSFitPOLY2b("",m_xLow,m_xHigh); }

};


class FSFitPOLY3 : public FSFitFunction{
 public:

  FSFitPOLY3(TString n_fName, double n_xLow, double n_xHigh) :
    FSFitFunction(n_fName,n_xLow,n_xHigh){ 
    addParameter("a",10.0,"variable a");
    addParameter("b", 0.0,"variable b");
    addParameter("c", 0.0,"variable c");
    addParameter("d", 0.0,"variable d");
  }

  double fx (double x){
    double a = getParameterValue("a");
    double b = getParameterValue("b");
    double c = getParameterValue("c");
    double d = getParameterValue("d");
    return (a + b*(x -3.872)+ c*(x-3.872)*(x-3.872)+d*(x-3.872)*(x-3.872)*(x-3.872));
  }

  FSFitPOLY3* clone(){ return new FSFitPOLY3("",m_xLow,m_xHigh); }

};


class FSFitGAUS : public FSFitFunction{
 public:

 FSFitGAUS(TString n_fName, double n_xLow, double n_xHigh) :
  FSFitFunction(n_fName,n_xLow,n_xHigh){ 
    addParameter("N",10.0,"size");
    addParameter("M",10.0,"mean");
    addParameter("W",10.0,"width");
    addParameter("S",1.0,"scale");
    f_name = n_fName;
  }

  double fx (double x){
    double N = getParameterValue("N");
    double M = getParameterValue("M");
    double W = getParameterValue("W");
    double S = getParameterValue("S");
    if (W == 0.0) return 1.0e-6;
    return N*S/W/sqrt(2.0*3.141592654)*exp(-0.5*(x-M)*(x-M)/W/W);
  }
  
  FSFitGAUS* clone(){ return new FSFitGAUS("",m_xLow,m_xHigh); }
 private:

  TString f_name;
};





// the convolution of a guassian and a lorentzian

class FSFitVOIGT : public FSFitFunction{
 public:

 FSFitVOIGT(TString n_fName, double n_xLow, double n_xHigh) :
  FSFitFunction(n_fName,n_xLow,n_xHigh){
   // N can give us the overall normalization
   addParameter("N",1000,"size");
   // we also need the mean to center the function
   addParameter("M",10.0,"mean");
   // width of gaussian
   addParameter("W",10.0,"width");
   // internal width of particle
   addParameter("L",10.0,"internal width");
   addParameter("S",1.0,"scale");
   f_name = n_fName;
  }

  double fx (double x){
    double N = getParameterValue("N");
    double M = getParameterValue("M");
    double W = getParameterValue("W");
    double L = getParameterValue("L");
    double S = getParameterValue("S");
    //if (W == 0.0) return 1.0e-6;
    return N*S*TMath::Voigt(x-M,W,L);
  }
  
  FSFitVOIGT* clone(){ return new FSFitVOIGT("",m_xLow,m_xHigh); }
 private:

  TString f_name;
  
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

      // here x is the mass of the resonance candidates
      //need to map x=T to infinity to 0 to T
      //x=T/(x+1);
      
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



class FSFitRISE : public FSFitFunction{
 public:

  FSFitRISE(TString n_fName, double n_xLow, double n_xHigh) :
    FSFitFunction(n_fName,n_xLow,n_xHigh){ 
    addParameter("a", 1.0,"variable a");
    addParameter("b", 1.0,"variable b");
    addParameter("c", 1.0,"variable c");
    addParameter("d", 1.0,"variable d");
    addParameter("e", 1.0,"variable e");
    addParameter("f", 1.0,"variable f");
  }

  double fx (double x){
    // now a is the number of bkg events
    double a = getParameterValue("a");
    double b = getParameterValue("b");
    double c = getParameterValue("c");
    double d = getParameterValue("d");
    double e = getParameterValue("e");
    double f = getParameterValue("f");
    return a+b*exp(-c*(x-d))/(1+exp(-(x-e)/f));
  }

  FSFitRISE* clone(){ return new FSFitRISE("",m_xLow,m_xHigh); }

};




class FSFitRISE2 : public FSFitFunction{
 public:

  FSFitRISE2(TString n_fName, double n_xLow, double n_xHigh) :
    FSFitFunction(n_fName,n_xLow,n_xHigh){ 
    addParameter("a", 1.0,"variable a");
    addParameter("b", 1.0,"variable b");
    addParameter("c", 1.0,"variable c");
    addParameter("d", 1.0,"variable d");
    addParameter("f", 1.0,"variable f");

  }

  double fx (double x){
    // now a is the number of bkg events
    double a = getParameterValue("a");
    double b = getParameterValue("b");
    double c = getParameterValue("c");
    double d = getParameterValue("d");
    double f = getParameterValue("f");
    return a+b*exp(-c*(x-d))/(1+exp(-f*(x-d)));
  }

  FSFitRISE2* clone(){ return new FSFitRISE2("",m_xLow,m_xHigh); }

};




class FSFitDOUBLEGAUS : public FSFitFunction{
 public:

 FSFitDOUBLEGAUS(TString n_fName, double n_xLow, double n_xHigh) :
  FSFitFunction(n_fName,n_xLow,n_xHigh){ 
    addParameter("N",10.0,"size");
    addParameter("M",10.0,"mean");
    addParameter("W1",10.0,"lo width");
    addParameter("W2",10.0,"hi width");
    f_name = n_fName;
  }

  double fx (double x){
    double N = getParameterValue("N");
    double M = getParameterValue("M");
    double W1 = getParameterValue("W1");
    double W2 = getParameterValue("W2");

    double W;
    if(x<M){
      W=W1;
    } else {
      W=W2;
    }
    if (W == 0.0) return 1.0e-6;
    return N*sqrt(2./3.1416)/(W1+W2)*exp(-0.5*(x-M)*(x-M)/(W*W));
  }
  
  FSFitDOUBLEGAUS* clone(){ return new FSFitDOUBLEGAUS("",m_xLow,m_xHigh); }
 private:

  TString f_name;
};




class FSFitCRYSTALBALL : public FSFitFunction{
 public:

 FSFitCRYSTALBALL(TString n_fName, double n_xLow, double n_xHigh) :
  FSFitFunction(n_fName,n_xLow,n_xHigh){ 
    addParameter("N",10.0,"size");
    addParameter("M",10.0,"mean");
    addParameter("W",10.0,"width");
    addParameter("A",10.0,"alpha");
    addParameter("n",1,"n");
    f_name = n_fName;
  }

  double fx (double x){
    double N = getParameterValue("N");
    double M = getParameterValue("M");
    double W = getParameterValue("W");
    double alpha = getParameterValue("A");
    double n = getParameterValue("n");


    double A = pow((n/abs(alpha)),n)*exp(-pow(alpha,2)/2.0);
    double B = n/abs(alpha) - abs(alpha);
    double C = n/abs(alpha)*1.0/(n-1)*exp(-pow(alpha,2)/2.0);
    double D = sqrt(3.1416/2.0)*(1+erf(abs(alpha)/sqrt(2)));

    double Norm = 1.0/(W*(C+D));
    
   
    if (W == 0.0) return 1.0e-6;
    if ((x-M)/W>-alpha){
      return N*Norm*exp(-pow((x-M)/W,2)/2.0);
    } else{
      return N*Norm*A*pow(((B-(x-M)/W)),-n);
    }
  }
  
  FSFitCRYSTALBALL* clone(){ return new FSFitCRYSTALBALL("",m_xLow,m_xHigh); }
 private:

  TString f_name;
};


#endif
