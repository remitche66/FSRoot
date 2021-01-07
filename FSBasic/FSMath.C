#include <iostream>
#include <vector>
#include <utility>
#include "FSBasic/FSMath.h"



// ***************************************
//  HELICITY AND GOTTFRIED-JACKSON ANGLES
//    (see the header file for details)
// ***************************************


double
FSMath::helcostheta(double PxPA, double PyPA, double PzPA, double EnPA,
                    double PxPB, double PyPB, double PzPB, double EnPB,
                    double PxPC, double PyPC, double PzPC, double EnPC){
  TLorentzVector PA(PxPA,PyPA,PzPA,EnPA);
  TLorentzVector PB(PxPB,PyPB,PzPB,EnPB);
  TLorentzVector PC(PxPC,PyPC,PzPC,EnPC);
  TLorentzVector PR = PA + PB;
  TLorentzVector PS = PR + PC;
    // boost all needed four-vectors to the S rest frame
  PA.Boost(-1.0*PS.BoostVector());
  PR.Boost(-1.0*PS.BoostVector());
    // rotate so PR is aligned along the z-axis
  double thetaR = PR.Theta();
  double phiR = PR.Phi();
  PA.RotateZ(-1.0*phiR);      PR.RotateZ(-1.0*phiR);
  PA.RotateY(-1.0*thetaR);    PR.RotateY(-1.0*thetaR);
    // boost to the PR rest frame
  PA.Boost(-1.0*PR.BoostVector());
  return PA.CosTheta();
}


double
FSMath::helphi(double PxPA, double PyPA, double PzPA, double EnPA,
               double PxPB, double PyPB, double PzPB, double EnPB,
               double PxPC, double PyPC, double PzPC, double EnPC,
               double PxPD, double PyPD, double PzPD, double EnPD){
  TLorentzVector PA(PxPA,PyPA,PzPA,EnPA);
  TLorentzVector PB(PxPB,PyPB,PzPB,EnPB);
  TLorentzVector PC(PxPC,PyPC,PzPC,EnPC);
  TLorentzVector PD(PxPD,PyPD,PzPD,EnPD);
  TLorentzVector PR = PA + PB;
  TLorentzVector PS = PR + PC;
    // boost all needed four-vectors to the S rest frame
  PA.Boost(-1.0*PS.BoostVector());
  PR.Boost(-1.0*PS.BoostVector());
  PD.Boost(-1.0*PS.BoostVector());
    // rotate so PR is aligned along the z-axis
  double thetaR = PR.Theta();
  double phiR = PR.Phi();
  PA.RotateZ(-1.0*phiR);    PR.RotateZ(-1.0*phiR);    PD.RotateZ(-1.0*phiR);
  PA.RotateY(-1.0*thetaR);  PR.RotateY(-1.0*thetaR);  PD.RotateY(-1.0*thetaR);
    // rotate so PD is in the xz-plane
  PA.RotateZ(-1.0*PD.Phi());
  PR.RotateZ(-1.0*PD.Phi());
    // boost to the PR rest frame
  PA.Boost(-1.0*PR.BoostVector());
  return PA.Phi();
}



double
FSMath::gjcostheta(double PxPA, double PyPA, double PzPA, double EnPA,
                   double PxPB, double PyPB, double PzPB, double EnPB,
                   double PxPD, double PyPD, double PzPD, double EnPD){
  TLorentzVector PA(PxPA,PyPA,PzPA,EnPA);
  TLorentzVector PB(PxPB,PyPB,PzPB,EnPB);
  TLorentzVector PD(PxPD,PyPD,PzPD,EnPD);
  TLorentzVector PR = PA + PB;
    // boost all needed four-vectors to the R rest frame
  PA.Boost(-1.0*PR.BoostVector());
  PD.Boost(-1.0*PR.BoostVector());
    // rotate so PD is aligned along the z-axis
  PA.RotateZ(-1.0*PD.Phi());
  PA.RotateY(-1.0*PD.Theta());
  return PA.CosTheta();
}


double
FSMath::gjphi(double PxPA, double PyPA, double PzPA, double EnPA,
              double PxPB, double PyPB, double PzPB, double EnPB,
              double PxPC, double PyPC, double PzPC, double EnPC,
              double PxPD, double PyPD, double PzPD, double EnPD){
  TLorentzVector PA(PxPA,PyPA,PzPA,EnPA);
  TLorentzVector PB(PxPB,PyPB,PzPB,EnPB);
  TLorentzVector PC(PxPC,PyPC,PzPC,EnPC);
  TLorentzVector PD(PxPD,PyPD,PzPD,EnPD);
  TLorentzVector PR = PA + PB;
    // boost all needed four-vectors to the R rest frame
  PA.Boost(-1.0*PR.BoostVector());
  PC.Boost(-1.0*PR.BoostVector());
  PD.Boost(-1.0*PR.BoostVector());
    // rotate so PD is aligned along the z-axis
  PA.RotateZ(-1.0*PD.Phi());    PC.RotateZ(-1.0*PD.Phi());
  PA.RotateY(-1.0*PD.Theta());  PC.RotateY(-1.0*PD.Theta());
    // rotate so PC is in the xz-plane
  PA.RotateZ(-1.0*PC.Phi());
  return PA.Phi();
}


// ***************************************
//  PLANEPHI
//    for three four-vectors P1, P2, and P3, planephi is the rotation of the P2-P3 plane
//     with respect to the P1-P2 plane
// ***************************************


double
FSMath::planephi(double PxP1, double PyP1, double PzP1, double EnP1,
                 double PxP2, double PyP2, double PzP2, double EnP2,
                 double PxP3, double PyP3, double PzP3, double EnP3){
  TLorentzVector P1(PxP1,PyP1,PzP1,EnP1);
  TLorentzVector P2(PxP2,PyP2,PzP2,EnP2);
  TLorentzVector P3(PxP3,PyP3,PzP3,EnP3);
    // rotate so P2 is aligned along the z-axis
  P1.RotateZ(-1.0*P2.Phi());    P3.RotateZ(-1.0*P2.Phi());  
  P1.RotateY(-1.0*P2.Theta());  P3.RotateY(-1.0*P2.Theta());
    // rotate so P1 is in the xz-plane
  P3.RotateZ(-1.0*P1.Phi());
  return P3.Phi();
}


// ***************************************
//  D-FUNCTIONS, ETC.
// ***************************************

int
FSMath::factorial(int N){
  if (N > 15) cout << "WARNING: factorial might be too big" << endl;
  if (N < 0) return 0;
  if (N == 0) return 1;
  int Nf = 1;
  for (int i = 2; i <= N; i++){ Nf *= i; }
  return Nf;
}


double
FSMath::dJM1M2(double theta, double J, double M1, double M2){
  int twoJ  = (int)(fabs(J)*2.0+0.001);   if (J  < 0) twoJ *= -1;
  int twoM1 = (int)(fabs(M1)*2.0+0.001);  if (M1 < 0) twoM1 *= -1;
  int twoM2 = (int)(fabs(M2)*2.0+0.001);  if (M2 < 0) twoM2 *= -1;

    // return the constant case
  if ((twoJ ==  0) && (twoM1 ==  0) && (twoM2 ==  0)) return 1.0;

    // checks on input parameters
  if  ((twoJ - twoM1)%2 != 0) return 0.0;
  if  ((twoJ - twoM2)%2 != 0) return 0.0;
  if ((twoM1 - twoM2)%2 != 0) return 0.0;
  if  (abs(twoM1) > twoJ) return 0.0;
  if  (abs(twoM2) > twoJ) return 0.0;
  if (twoJ < 0) return 0.0;

    // trigonometric functions
  double c2 = cos(theta/2.0);
  double s2 = sin(theta/2.0);

    // numerator from formula in Perl (216)
  int f1 = factorial((twoJ+twoM2)/2); if (f1 == 0) return 0.0;
  int f2 = factorial((twoJ-twoM2)/2); if (f2 == 0) return 0.0;
  int f3 = factorial((twoJ+twoM1)/2); if (f3 == 0) return 0.0;
  int f4 = factorial((twoJ-twoM1)/2); if (f4 == 0) return 0.0;
  double f1234 = (double)f1*(double)f2*(double)f3*(double)f4;
  double sqrtf1234 = sqrt(f1234);

    // loop over N
  double funcTot = 0.0;
  for (int N = 0; N < 1000; N++){
    int twoN = 2*N;
    if (twoN > (twoJ-twoM1) && twoN > (twoJ+twoM2)) break;

      // more from the numerator
    double func = sqrtf1234;
    if (N%2 == 1) func *= -1.0;

      // denominator from Perl (216)
    int f5 = factorial((twoJ-twoM1-twoN)/2);  if (f5 == 0) continue;
    int f6 = factorial((twoJ+twoM2-twoN)/2);  if (f6 == 0) continue;
    int f7 = factorial((twoN+twoM1-twoM2)/2); if (f7 == 0) continue;
    int f8 = factorial(N);                    if (f8 == 0) continue;
    double f5678 = (double)f5*(double)f6*(double)f7*(double)f8;

      // put it together
    func = func / f5678;
    double c2p = pow(c2,twoJ+(twoM2-twoM1)/2-twoN);
    double s2p = pow(-1.0*s2,(twoM1-twoM2)/2+twoN);
    func *= c2p*s2p;
    funcTot += func;
  }

  return funcTot;
}

double
FSMath::ReYLM(double theta, double phi, int L, int M){
  if (L < 0) return 0.0;
  if (abs(M) > L) return 0.0;
  double oneoversqrt4pi = 1.0/sqrt(4.0*3.14159265358979);
  if (L == 0) return oneoversqrt4pi;
  return sqrt(2.0*L+1.0)*oneoversqrt4pi*dJM1M2(theta,L,M,0)*cos(M*phi);
}



// ***************************************
//  MIN AND MAX FUNCTIONS
// ***************************************

double 
FSMath::max(){ 
  return 0.0;
}

double 
FSMath::max(double x1){ 
  return x1;
}

double 
FSMath::max(double x1, double x2){ 
  return x2 > x1 ? x2 : x1;
}

double 
FSMath::max(double x1, double x2, double x3){ 
  double x0 = max(x1,x2);
  return x3 > x0 ? x3 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4){ 
  double x0 = max(x1,x2,x3);
  return x4 > x0 ? x4 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4, double x5){ 
  double x0 = max(x1,x2,x3,x4);
  return x5 > x0 ? x5 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4, double x5, double x6){ 
  double x0 = max(x1,x2,x3,x4,x5);
  return x6 > x0 ? x6 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7){ 
  double x0 = max(x1,x2,x3,x4,x5,x6);
  return x7 > x0 ? x7 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7, double x8){ 
  double x0 = max(x1,x2,x3,x4,x5,x6,x7);
  return x8 > x0 ? x8 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7, double x8, double x9){ 
  double x0 = max(x1,x2,x3,x4,x5,x6,x7,x8);
  return x9 > x0 ? x9 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7, double x8, double x9, double x10){ 
  double x0 = max(x1,x2,x3,x4,x5,x6,x7,x8,x9);
  return x10 > x0 ? x10 : x0;
}

double 
FSMath::min(){ 
  return 0.0;
}

double 
FSMath::min(double x1){ 
  return x1;
}

double 
FSMath::min(double x1, double x2){ 
  return x2 < x1 ? x2 : x1;
}

double 
FSMath::min(double x1, double x2, double x3){ 
  double x0 = min(x1,x2);
  return x3 < x0 ? x3 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4){ 
  double x0 = min(x1,x2,x3);
  return x4 < x0 ? x4 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4, double x5){ 
  double x0 = min(x1,x2,x3,x4);
  return x5 < x0 ? x5 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4, double x5, double x6){ 
  double x0 = min(x1,x2,x3,x4,x5);
  return x6 < x0 ? x6 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7){ 
  double x0 = min(x1,x2,x3,x4,x5,x6);
  return x7 < x0 ? x7 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7, double x8){ 
  double x0 = min(x1,x2,x3,x4,x5,x6,x7);
  return x8 < x0 ? x8 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7, double x8, double x9){ 
  double x0 = min(x1,x2,x3,x4,x5,x6,x7,x8);
  return x9 < x0 ? x9 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7, double x8, double x9, double x10){ 
  double x0 = min(x1,x2,x3,x4,x5,x6,x7,x8,x9);
  return x10 < x0 ? x10 : x0;
}
