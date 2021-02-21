#if !defined(ANGDIST)
#define ANGDIST

#pragma once

#include <cmath>
#include <complex>
#include "TLorentzVector.h"
#include "TMath.h"
#include "FSAmpToolsAmp/helicity.h"
#include "FSAmpToolsAmp/clebschGordan.h"
#include "FSAmpToolsAmp/wignerD.h"
#include "FSAmpToolsAmp/BarrierFactor.h"

class angDist
{

public:
    // LS scheme
    static complex<double> ampDecay(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD,
                               int J, int lam, int JA, int lamA, int JB, int lamB, int gl, int gs);
    static complex<double> ampProd(TLorentzVector PA, TLorentzVector PB, TLorentzVector PD,
                               int J, int lam, int JA, int lamA, int JB, int lamB, int gl, int gs);
    static complex<double> ampDecay(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD,
                               double J, double lam, double JA, double lamA, double JB, double lamB, double gl, double gs);
    static complex<double> ampProd(TLorentzVector PA, TLorentzVector PB, TLorentzVector PD,
                               double J, double lam, double JA, double lamA, double JB, double lamB, double gl, double gs);
    // helicity states
    static complex<double> helampDecay(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD,
                               int J, int lam, int lamA, int lamB);
    static complex<double> helampProd(TLorentzVector PA, TLorentzVector PB, TLorentzVector PD,
                               int J, int lam, int lamA, int lamB, int gl=0);
    static complex<double> helampDecay(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD,
                               double J, double lam, double lamA, double lamB);
    static complex<double> helampProd(TLorentzVector PA, TLorentzVector PB, TLorentzVector PD,
                               double J, double lam, double lamA, double lamB, double gl=0);

    // radiative multipole basis
    static complex<double> multipoleDecay(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD,
                               int J, int lam, int J_r, int mu_r, int J_gam, int mu_gam, string type);
    static complex<double> multipoleProd(TLorentzVector PA, TLorentzVector PB, TLorentzVector PD,
                               int J, int lam, int J_r, int mu_r, int J_gam, int mu_gam, string type);
    static complex<double> multipoleDecay(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD,
                               double J, double lam, double J_r, double mu_r, double J_gam, double mu_gam, string type);
    static complex<double> multipoleProd(TLorentzVector PA, TLorentzVector PB, TLorentzVector PD,
                               double J, double lam, double J_r, double mu_r, double J_gam, double mu_gam, string type);

private:

};



#endif