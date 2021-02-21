#include "FSAmpToolsAmp/angDist.h"

// angular distribution for R --> A + B
// where C is the particle recoiling off R
// and D is either the beam particle or the particle recoiling off (R+C), depending on kinematics

// coupling to LS scheme
complex<double> angDist::ampDecay(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD,
                               int J, int lam, int JA, int lamA, int JB, int lamB, int gl, int gs){
   complex<double> amp(0.,0.);
   TLorentzVector helA = helicity::helvector(PA,PB,PC,PD);
   double helthetaA = helA.Theta()*TMath::RadToDeg();
   double helphiA = helA.Phi();

   amp = wignerDC_1(J, lam, lamA-lamB,helthetaA,helphiA)
         * clebschGordan(gl,gs,0,lamA-lamB,J,lamA-lamB)
         * clebschGordan(JA,JB,lamA,-lamB,gs,lamA-lamB)
         * sqrt(2*gl+1);
         // no Barrier Factor here, that will be put into the dynamics of resonance R
    return amp;
}

complex<double> angDist::ampDecay(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD,
                               double J, double lam, double JA, double lamA, double JB, double lamB, double gl, double gs){
   // this is the exact same function, but calls a different clebschGordan and wignerD function for half-integer spins 
   complex<double> amp(0.,0.);
   TLorentzVector helA = helicity::helvector(PA,PB,PC,PD);
   double helthetaA = helA.Theta()*TMath::RadToDeg();
   double helphiA = helA.Phi();

   amp = wignerDC_1(J, lam, lamA-lamB,helthetaA,helphiA)
         * clebschGordan(gl,gs,0.,lamA-lamB,J,lamA-lamB)
         * clebschGordan(JA,JB,lamA,-lamB,gs,lamA-lamB)
         * sqrt(2*gl+1);
         // no Barrier Factor here, that will be put into the dynamics of resonance R
    return amp;
}

// using helicity eigenstates
complex<double> angDist::helampDecay(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD,
                               int J, int lam, int lamA, int lamB){
   complex<double> amp(0.,0.);
   TLorentzVector helA = helicity::helvector(PA,PB,PC,PD);
   double helthetaA = helA.Theta()*TMath::RadToDeg();
   double helphiA = helA.Phi();
   amp = wignerDC_1(J, lam, lamA-lamB,helthetaA,helphiA)
         * sqrt(2*J+1)/(4*TMath::Pi());
         // no Barrier Factor here, that will be put into the dynamics of resonance R
    return amp;
}

complex<double> angDist::helampDecay(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD,
                               double J, double lam, double lamA, double lamB){
   complex<double> amp(0.,0.);
   TLorentzVector helA = helicity::helvector(PA,PB,PC,PD);
   double helthetaA = helA.Theta()*TMath::RadToDeg();
   double helphiA = helA.Phi();
   amp = wignerDC_1(J, lam, lamA-lamB,helthetaA,helphiA)
         * sqrt(2*J+1)/(4*TMath::Pi());
         // no Barrier Factor here, that will be put into the dynamics of resonance R
    return amp;
}

// coupling to radiative multipole basis
complex<double> angDist::multipoleDecay(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD,
                               int J, int lam, int J_r, int mu_r, int J_gam, int mu_gam, string type){
    complex<double> amp(0.,0.);
    TLorentzVector helA = helicity::helvector(PA,PB,PC,PD);
    double helthetaA = helA.Theta()*TMath::RadToDeg();
    double helphiA = helA.Phi();
    double factor=1;
    if(mu_gam==-1){
        if(type.compare("E")==0){
            // odd J_gam changes parity: (-1)^(J_gam+1) for odd values of J_gam, (-1)^(J_gam) for even values of J_gam
            factor *= pow(-1,J_gam+J_gam%2);
        }
        else if(type.compare("M")==0){
            // other way around for magnetic transitions
            factor *= pow(-1,J_gam+(J_gam+1)%2);
        }
        else
        {
            cout << "error, unknown type in radiative multipole amplitude: " << type << endl;
            exit(0);
        }
    }
    amp = wignerDC_1(J, lam, mu_r-mu_gam,helthetaA,helphiA)
         * clebschGordan(J_r,J_gam,mu_r,-mu_gam,J,mu_r - mu_gam)
         * sqrt(2*J_gam+1)/sqrt(2*J+1);
    amp *= factor;
    return amp;
}

complex<double> angDist::multipoleDecay(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD,
                               double J, double lam, double J_r, double mu_r, double J_gam, double mu_gam, string type){
    complex<double> amp(0.,0.);
    TLorentzVector helA = helicity::helvector(PA,PB,PC,PD);
    double helthetaA = helA.Theta()*TMath::RadToDeg();
    double helphiA = helA.Phi();
    double factor=1;
    if(abs(mu_gam+1)<1e-5){
        if(type.compare("E")==0){
            // odd J_gam changes parity: (-1)^(J_gam+1) for odd values of J_gam, (-1)^(J_gam) for even values of J_gam
            factor *= pow(-1,J_gam+((int)J_gam)%2);
        }
        else if(type.compare("M")==0){
            // other way around for magnetic transitions
            factor *= pow(-1,J_gam+((int)J_gam+1)%2);
        }
        else
        {
            cout << "error, unknown type in radiative multipole amplitude: " << type << endl;
            exit(0);
        }
    }

    amp = wignerDC_1(J, lam, mu_r-mu_gam,helthetaA,helphiA)
         * clebschGordan(J_r,J_gam,mu_r,-mu_gam,J,mu_r - mu_gam)
         * sqrt(2*J_gam+1)/sqrt(2*J+1);
    amp *= factor;
    return amp;
}


// ***************************************************** //

// angular distribution for D + Q ---> A + B
// where C is the particle recoiling off R
// and D is either the beam particle or the particle recoiling off (R+C), depending on kinematics

// coupling to LS scheme
complex<double> angDist::ampProd(TLorentzVector PA, TLorentzVector PB, TLorentzVector PD,
                               int J, int lam, int JA, int lamA, int JB, int lamB, int gl, int gs){
   complex<double> amp(0.,0.);
   
   TLorentzVector helA = helicity::prodvector(PA,PB,PD);
   double prodthetaA = helA.Theta()*TMath::RadToDeg();
   double prodphiA = helA.Phi();
   double prodrhoA = helA.Rho();

   amp = wignerDC_1(J, lam, lamA-lamB,prodthetaA,prodphiA)
         * clebschGordan(gl,gs,0,lamA-lamB,J,lamA-lamB)
         * clebschGordan(JA,JB,lamA,-lamB,gs,lamA-lamB)
         * sqrt(2*gl+1);
    if(gl!=0)
        amp *= BarrierFactor::BlattWeisskopf(gl,prodrhoA,0.1973);
    return amp;
}

complex<double> angDist::ampProd(TLorentzVector PA, TLorentzVector PB, TLorentzVector PD,
                               double J, double lam, double JA, double lamA, double JB, double lamB, double gl, double gs){
   complex<double> amp(0.,0.);
   
   TLorentzVector helA = helicity::prodvector(PA,PB,PD);
   double prodthetaA = helA.Theta()*TMath::RadToDeg();
   double prodphiA = helA.Phi();
   double prodrhoA = helA.Rho();

   amp = wignerDC_1(J, lam, lamA-lamB,prodthetaA,prodphiA)
         * clebschGordan(gl,gs,0.,lamA-lamB,J,lamA-lamB)
         * clebschGordan(JA,JB,lamA,-lamB,gs,lamA-lamB)
         * sqrt(2*gl+1);
    if(abs(gl)>1e-5)
        amp *= BarrierFactor::BlattWeisskopf((int)gl,prodrhoA,0.1973);
    return amp;
}

// using helicity eigenstates
complex<double> angDist::helampProd(TLorentzVector PA, TLorentzVector PB, TLorentzVector PD,
                               int J, int lam, int lamA, int lamB, int gl){
   complex<double> amp(0.,0.);
   
   TLorentzVector helA = helicity::prodvector(PA,PB,PD);
   double prodthetaA = helA.Theta()*TMath::RadToDeg();
   double prodphiA = helA.Phi();
   double prodrhoA = helA.Rho();

   amp = wignerDC_1(J, lam, lamA-lamB,prodthetaA,prodphiA)
         * sqrt(2*J+1)/(4*TMath::Pi());
         
    // we do not know L in helicity frame, so we can in principle not put a Barrier Factor
    // however, use option to give some L for Barrier factor
    if(gl!=0)
        amp *= BarrierFactor::BlattWeisskopf(gl,prodrhoA,0.1973);
    return amp;
}

complex<double> angDist::helampProd(TLorentzVector PA, TLorentzVector PB, TLorentzVector PD,
                               double J, double lam, double lamA, double lamB, double gl){
   complex<double> amp(0.,0.);
   
   TLorentzVector helA = helicity::prodvector(PA,PB,PD);
   double prodthetaA = helA.Theta()*TMath::RadToDeg();
   double prodphiA = helA.Phi();
   double prodrhoA = helA.Rho();

   amp = wignerDC_1(J, lam, lamA-lamB,prodthetaA,prodphiA)
         * sqrt(2*J+1)/(4*TMath::Pi());
         
    // we do not know L in helicity frame, so we can in principle not put a Barrier Factor
    // however, use option to give some L for Barrier factor
    if(abs(gl)>1e-5)
        amp *= BarrierFactor::BlattWeisskopf((int)gl,prodrhoA,0.1973);
    return amp;
}



// coupling to radiative multipole basis
complex<double> angDist::multipoleProd(TLorentzVector PA, TLorentzVector PB, TLorentzVector PD,
                               int J, int lam, int J_r, int mu_r, int J_gam, int mu_gam, string type){
    complex<double> amp(0.,0.);
    
    TLorentzVector helA = helicity::prodvector(PA,PB,PD);
    double prodthetaA = helA.Theta()*TMath::RadToDeg();
    double prodphiA = helA.Phi();
    double prodrhoA = helA.Rho();
    double factor=1;
    if(mu_gam == -1){
        if(type.compare("E")==0){
            // odd J_gam changes parity: (-1)^(J_gam+1) for odd values of J_gam, (-1)^(J_gam) for even values of J_gam
            factor *= pow(-1,J_gam+J_gam%2);
        }
        else if(type.compare("M")==0){
            // other way around for magnetic transitions
            factor *= pow(-1,J_gam+(J_gam+1)%2);
        }
        else
        {
            cout << "error, unknown type in radiative multipole amplitude: " << type << endl;
            exit(0);
        }
    }

    amp = wignerDC_1(J, lam, mu_r - mu_gam,prodthetaA,prodphiA)
         * clebschGordan(J_r,J_gam,mu_r,-mu_gam,J,mu_r-mu_gam)
         * sqrt(2*J_gam+1)/sqrt(2*J+1);
    amp *= factor;
    return amp;
}

complex<double> angDist::multipoleProd(TLorentzVector PA, TLorentzVector PB, TLorentzVector PD,
                               double J, double lam, double J_r, double mu_r, double J_gam, double mu_gam, string type){
    complex<double> amp(0.,0.);
    
    TLorentzVector helA = helicity::prodvector(PA,PB,PD);
    double prodthetaA = helA.Theta()*TMath::RadToDeg();
    double prodphiA = helA.Phi();
    double prodrhoA = helA.Rho();
    double factor=1;
    if(abs(mu_gam+1)<1e-5){
        if(type.compare("E")==0){
            // odd J_gam changes parity: (-1)^(J_gam+1) for odd values of J_gam, (-1)^(J_gam) for even values of J_gam
            factor *= pow(-1,J_gam+((int)J_gam)%2);
        }
        else if(type.compare("M")==0){
            // other way around for magnetic transitions
            factor *= pow(-1,J_gam+((int)J_gam+1)%2);
        }
        else
        {
            cout << "error, unknown type in radiative multipole amplitude: " << type << endl;
            exit(0);
        }
    }

    amp = wignerDC_1(J, lam, mu_r - mu_gam,prodthetaA,prodphiA)
         * clebschGordan(J_r,J_gam,mu_r,-mu_gam,J,mu_r-mu_gam)
         * sqrt(2*J_gam+1)/sqrt(2*J+1);
    amp *= factor;
    return amp;
}
