#include <iostream>
#include <vector>
#include <utility>
#include "TH1F.h"
#include "TString.h"
#include "TCanvas.h"
#include "TBox.h"
#include "TLine.h"
#include "FSBasic/FSPhysics.h"


const double FSPhysics::XMY3S    = 10.3552;
const double FSPhysics::XMY1S    = 9.46030;
const double FSPhysics::XMpsi2S  = 3.686093;
const double FSPhysics::XMetac2S = 3.638;
const double FSPhysics::XMchic0  = 3.41476;
const double FSPhysics::XMchic1  = 3.51066;
const double FSPhysics::XMchic2  = 3.55620;
const double FSPhysics::XMJpsi   = 3.096916;
const double FSPhysics::XMetac   = 2.9804;
const double FSPhysics::XMhc     = 3.52593;
const double FSPhysics::XMrho    = 0.7755;
const double FSPhysics::XMomega  = 0.78265;
const double FSPhysics::XMphi    = 1.01946;
const double FSPhysics::XMn      = 0.939565;
const double FSPhysics::XMp      = 0.938272;
const double FSPhysics::XMeta    = 0.547853;
const double FSPhysics::XMK      = 0.493677;
const double FSPhysics::XMKs     = 0.497614;
const double FSPhysics::XMpi     = 0.13957;
const double FSPhysics::XMpi0    = 0.134977;
const double FSPhysics::XMmu     = 0.105658;
const double FSPhysics::XMe      = 0.000511;




      // ********************************************************
      // CONVERT pdgID TO A TSTRING
      // ********************************************************

TString
FSPhysics::pdgName(int id){

  TString name("");

  if      (id == kpdgPsi2S)      name = "psi(2S)";
  else if (id == kpdgGamma)      name = "gamma";
  else if (id == kpdgFSRGamma)   name = "gamma_{FSR}";
  else if (id == kpdgHc)         name = "h_{c}";
  else if (id == kpdgChic0)      name = "chi_{c0}";
  else if (id == kpdgChic1)      name = "chi_{c1}";
  else if (id == kpdgChic2)      name = "chi_{c2}";
  else if (id == kpdgJpsi)       name = "J/psi";
  else if (id == kpdgEtac)       name = "eta_{c}";
  else if (id == kpdgPhi)        name = "phi";
  else if (id == kpdgOmega)      name = "omega";
  else if (id == kpdgPi0)        name = "pi0";
  else if (id == kpdgPip)        name = "pi+";
  else if (id == kpdgPim)        name = "pi-";
  else if (id == kpdgRho0)       name = "rho0";
  else if (id == kpdgRhop)       name = "rho+";
  else if (id == kpdgRhom)       name = "rho-";
  else if (id == kpdgEtaprime)   name = "eta'";
  else if (id == kpdgEta)        name = "eta";
  else if (id == kpdgKs)         name = "Ks";
  else if (id == kpdgKl)         name = "Kl";
  else if (id == kpdgKp)         name = "K+";
  else if (id == kpdgKm)         name = "K-";
  else if (id == kpdgPp)         name = "p+";
  else if (id == kpdgPm)         name = "p-";
  else if (id == kpdgN)          name = "n";
  else if (id == kpdgAntiN)      name = "nbar";
  else if (id == kpdgDelta0)     name = "Delta0";
  else if (id == kpdgDeltap)     name = "Delta+";
  else if (id == kpdgDeltapp)    name = "Delta++";
  else if (id == kpdgEp)         name = "e+";
  else if (id == kpdgEm)         name = "e-";
  else if (id == kpdgMup)        name = "mu+";
  else if (id == kpdgMum)        name = "mu-";
  else if (id == kpdgTaup)       name = "tau+";
  else if (id == kpdgTaum)       name = "tau-";
  else if (id == kpdgNuE)        name = "nu";
  else if (id == kpdgNuMu)       name = "nu";
  else if (id == kpdgNuTau)      name = "nu";
  else if (id == kpdgAntiNuE)    name = "nu";
  else if (id == kpdgAntiNuMu)   name = "nu";
  else if (id == kpdgAntiNuTau)  name = "nu";
  else if (id == kpdgF0600)      name = "f0(600)";
  else if (id == kpdgK0)         name = "K0";
  else if (id == kpdgAntiK0)     name = "K0";
  else if (id == kpdgKstarp)     name = "K*+";
  else if (id == kpdgKstarm)     name = "K*-";
  else if (id == kpdgKstar0)     name = "K*0";
  else if (id == kpdgAntiKstar0) name = "K*0bar";
  else if (id == kpdgLambda)     name = "Lambda";
  else if (id == kpdgALambda)    name = "ALambda";
  else if (id == kpdgDp)         name = "D+";
  else if (id == kpdgDm)         name = "D-";
  else if (id == kpdgD0)         name = "D0";
  else if (id == kpdgDA)         name = "D0bar";
  else if (id == kpdgDstarp)     name = "D*+";
  else if (id == kpdgDstarm)     name = "D*-";
  else if (id == kpdgDstar0)     name = "D*0";
  else if (id == kpdgDstarA)     name = "D*0bar";
  else{
    name += id;
  }

  return name;
 
}


  // ********************************************************
  // SUM NUMBERS (e.g. ERRORS) IN QUADRATURE
  // ********************************************************

double 
FSPhysics::quadraticSum(double e1, double e2, double e3,
                                                     double e4,
                                                     double e5,
                                                     double e6,
                                                     double e7,
                                                     double e8,
                                                     double e9,
                                                     double e10,
                                                     double e11,
                                                     double e12,
                                                     double e13,
                                                     double e14,
                                                     double e15){
  return sqrt(e1*e1 +
              e2*e2 +
              e3*e3 +
              e4*e4 +
              e5*e5 +
              e6*e6 +
              e7*e7 +
              e8*e8 +
              e9*e9 +
              e10*e10 +
              e11*e11 +
              e12*e12 +
              e13*e13 +
              e14*e14 +
              e15*e15);
}



  // ********************************************************
  // KINEMATICS OF RADIATIVE DECAYS
  // ********************************************************

double 
FSPhysics::radPhotonEnergy(double parentMass, double daughterMass){ 
  double m = parentMass;
  double m1 = daughterMass;
  return (m*m - m1*m1) / (2.0*m);
}


double 
FSPhysics::radMass(double parentMass, double photonEnergy){ 
  double m = parentMass;
  double e = photonEnergy;
  return sqrt(m*m - 2.0*m*e);
}


double 
FSPhysics::radMassError(double parentMass, double photonEnergy, double photonEnergyError){
  double m = parentMass;
  double e = photonEnergy;
  double ee = photonEnergyError;
  return ((m/radMass(m,e))*ee);
}


  // ********************************************************
  // FIT SIGNIFICANCE
  // ********************************************************

double 
FSPhysics::fitSignificance(double chi2Difference, int ndfDifference){
  int sign = 1;  if (chi2Difference < 0.0) sign = -1;
  chi2Difference = abs(chi2Difference);
  return sign*abs(TMath::NormQuantile(0.5*TMath::Prob(chi2Difference,ndfDifference)));
}

