#if !(defined HELICITY)
#define HELICITY

#pragma once
#include <cmath>
#include <complex>
#include "TLorentzVector.h"

class helicity
{

public:
    static TLorentzVector helvector(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD);
    static double heltheta(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC);
    static double helcostheta(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC);
    static double helphi(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD);
    static double helrho(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD);

    static TLorentzVector prodvector(TLorentzVector PR, TLorentzVector PC, TLorentzVector PD);
    static double prodtheta(TLorentzVector PR, TLorentzVector PC, TLorentzVector PD);
    static double prodcostheta(TLorentzVector PR, TLorentzVector PC, TLorentzVector PD);
    static double prodphi(TLorentzVector PR, TLorentzVector PC, TLorentzVector PD);
    static double prodrho(TLorentzVector PR, TLorentzVector PC, TLorentzVector PD);

    // from PAWIAN for comparison
    static TLorentzVector heliVec(const TLorentzVector& motherRef,  const TLorentzVector& ref, const TLorentzVector& mother, const TLorentzVector& daughter);
    

private:

 };

#endif