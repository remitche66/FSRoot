#if !defined(FSPHYSICS_H)
#define FSPHYSICS_H

#include <cmath>

using namespace std;


class FSPhysics{

  public:

      // ********************************************************
      // PDG MASSES AND ID NUMBERS
      // ********************************************************

    static const double XMY3S;
    static const double XMY1S;
    static const double XMpsi2S;
    static const double XMetac2S;
    static const double XMchic0;
    static const double XMchic1;
    static const double XMchic2;
    static const double XMJpsi;
    static const double XMetac;
    static const double XMhc;
    static const double XMrho;
    static const double XMomega;
    static const double XMphi;
    static const double XMn;
    static const double XMp;
    static const double XMeta;
    static const double XMK;
    static const double XMKs;
    static const double XMpi;
    static const double XMpi0;
    static const double XMmu;
    static const double XMe;

    static const int kpdgPsi2S      = 100443;     
    static const int kpdgGamma      = 22;         
    static const int kpdgFSRGamma   = -22;        
    static const int kpdgHc         = 10443;      
    static const int kpdgChic0      = 10441;      
    static const int kpdgChic1      = 20443;      
    static const int kpdgChic2      = 445;        
    static const int kpdgJpsi       = 443;        
    static const int kpdgEtac       = 441;        
    static const int kpdgPhi        = 333;        
    static const int kpdgOmega      = 223;        
    static const int kpdgPi0        = 111;        
    static const int kpdgPip        = 211;        
    static const int kpdgPim        = -211;       
    static const int kpdgRho0       = 113;        
    static const int kpdgRhop       = 213;        
    static const int kpdgRhom       = -213;       
    static const int kpdgEtaprime   = 331;        
    static const int kpdgEta        = 221;        
    static const int kpdgKs         = 310;        
    static const int kpdgKl         = 130;        
    static const int kpdgKp         = 321;        
    static const int kpdgKm         = -321;       
    static const int kpdgPp         = 2212;       
    static const int kpdgPm         = -2212;      
    static const int kpdgN          = 2112;       
    static const int kpdgAntiN      = -2112;
    static const int kpdgDelta0     = 2114;
    static const int kpdgDeltap     = 2214;
    static const int kpdgDeltapp    = 2224;
    static const int kpdgEp         = -11;         
    static const int kpdgEm         = 11;        
    static const int kpdgMup        = -13;         
    static const int kpdgMum        = 13;        
    static const int kpdgTaup       = -15;        
    static const int kpdgTaum       = 15;         
    static const int kpdgNuE        = 12;         
    static const int kpdgNuMu       = 14;         
    static const int kpdgNuTau      = 16;         
    static const int kpdgAntiNuE    = -12;        
    static const int kpdgAntiNuMu   = -14;        
    static const int kpdgAntiNuTau  = -16;        
    static const int kpdgF0600      = 9000221;    
    static const int kpdgK0         = 311;        
    static const int kpdgAntiK0     = -311;       
    static const int kpdgKstarp     = 323;        
    static const int kpdgKstarm     = -323;       
    static const int kpdgKstar0     = 313;        
    static const int kpdgAntiKstar0 = -313;
    static const int kpdgLambda     = 3122;
    static const int kpdgALambda    = -3122;
    static const int kpdgDp         = 411;
    static const int kpdgDm         = -411;
    static const int kpdgD0         = 421;
    static const int kpdgDA         = -421;
    static const int kpdgDstarp     = 413;
    static const int kpdgDstarm     = -413;
    static const int kpdgDstar0     = 423;
    static const int kpdgDstarA     = -423;


      // ********************************************************
      // CONVERT pdgID TO A TSTRING
      // ********************************************************

    static TString pdgName(int pdgID);


      // ********************************************************
      // SUM NUMBERS (e.g. ERRORS) IN QUADRATURE
      // ********************************************************

    static double quadraticSum(double e1, double e2, double e3 = 0.0,
                                                     double e4 = 0.0,
                                                     double e5 = 0.0,
                                                     double e6 = 0.0,
                                                     double e7 = 0.0,
                                                     double e8 = 0.0,
                                                     double e9 = 0.0,
                                                     double e10 = 0.0,
                                                     double e11 = 0.0,
                                                     double e12 = 0.0,
                                                     double e13 = 0.0,
                                                     double e14 = 0.0,
                                                     double e15 = 0.0);

      // ********************************************************
      // KINEMATICS OF RADIATIVE DECAYS
      // ********************************************************

    static double radPhotonEnergy(double parentMass, double daughterMass);

    static double radMass(double parentMass, double photonEnergy);

    static double radMassError(double parentMass, double photonEnergy, double photonEnergyError);

};


#endif
