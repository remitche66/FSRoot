#if !defined(FSMATH_H)
#define FSMATH_H

#include <cmath>
#include "TLorentzVector.h"

using namespace std;


class FSMath{

  public:


// ***************************************************
//  HELICITY, GOTTFRIED-JACKSON, AND PRODUCTION ANGLES
// ***************************************************
//
//  Starting with either of these processes:
//    (1) D + Q --> S;  S --> R + C;  R --> A + B
//          examples:
//            e+(D) e-(Q) --> J/psi(S);  J/psi(S) --> rho0(R) pi0(C);  rho0(R) --> pi+(A) pi-(B)
//            gamma(D) p(Q) --> CM(S);  CM(S) --> phi(R) p'(C);  phi(R) --> K+(A) K-(B)  
//    (2) Q --> D + S;  S --> R + C;  R --> A + B
//          example:
//            chi_c1(Q) --> gamma(D) J/psi(S);  J/psi(S) --> rho0(R) pi0(C);  rho0(R) --> pi+(A) pi-(B)
//    calculate the helicity or Gottfried-Jackson angles for particle A or the
//    production angle for particle R.
//  Procedure for the helicity angles of A:
//    * First boost all four-vectors to the S rest frame, where R and C are back-to-back [pR = -pC]
//        and D and Q are either back-to-back (1) or parallel (2) [pD = +-pQ].
//    * Rotate all four-vectors so pR and -pC are along the z-axis.  
//    * Rotate all four-vectors around the z-axis so pD and +-pQ are in the xz-plane.
//    * Now the xz-plane is defined by DQ and RC.
//    * Boost all four-vectors along the z-axis to the R rest frame.
//    * Return the cosine of the polar angle of pA (helcostheta) 
//        or the azimuthal angle of pA (helphi).
//  Procedure for the Gottried-Jackson angles of A:
//    * First boost all four-vectors to the R rest frame, where A and B are back-to-back [pA = -pB]
//        and S and C are parallel [pS = pC].
//    * Rotate all four-vectors so pD is along the z-axis.  
//    * Rotate all four-vectors around the z-axis so pC and pS are in the xz-plane.
//    * Now the xz-plane is defined by D and CS.
//    * Return the cosine of the polar angle of pA (gjcostheta) 
//        or the azimuthal angle of pA (gjphi).
//  Procecure for the production angle of R:
//    * First boost all four-vectors to the S rest frame, where R and C are back-to-back [pR = -pC]
//        and D and Q are either back-to-back (1) or parallel (2) [pD = +-pQ].
//    * Rotate all four-vectors so pD is along the z-axis.  
//    * Return the cosine of the polar angle of pR (prodcostheta). 
//
// ***************************************


    static double helcostheta(double PxPA, double PyPA, double PzPA, double EnPA,
                              double PxPB, double PyPB, double PzPB, double EnPB,
                              double PxPC, double PyPC, double PzPC, double EnPC);

    static double helphi(double PxPA, double PyPA, double PzPA, double EnPA,
                         double PxPB, double PyPB, double PzPB, double EnPB,
                         double PxPC, double PyPC, double PzPC, double EnPC,
                         double PxPD, double PyPD, double PzPD, double EnPD);

    static double gjcostheta(double PxPA, double PyPA, double PzPA, double EnPA,
                             double PxPB, double PyPB, double PzPB, double EnPB,
                             double PxPD, double PyPD, double PzPD, double EnPD);

    static double gjphi(double PxPA, double PyPA, double PzPA, double EnPA,
                        double PxPB, double PyPB, double PzPB, double EnPB,
                        double PxPC, double PyPC, double PzPC, double EnPC,
                        double PxPD, double PyPD, double PzPD, double EnPD);

    static double prodcostheta(double PxPR, double PyPR, double PzPR, double EnPR,
                               double PxPC, double PyPC, double PzPC, double EnPC,
                               double PxPD, double PyPD, double PzPD, double EnPD);


// ***************************************
//  PLANEPHI
//    for three four-vectors P1, P2, and P3, planephi is the rotation of the P2-P3 plane
//     with respect to the P1-P2 plane
// ***************************************

    static double planephi(double PxP1, double PyP1, double PzP1, double EnP1,
                           double PxP2, double PyP2, double PzP2, double EnP2,
                           double PxP3, double PyP3, double PzP3, double EnP3);


// ***************************************
//  D-FUNCTIONS, ETC.
// ***************************************

      // d function (from Perl 216)
    static double dJM1M2(double theta, double J, double M1, double M2);

    static double ReYLM(double theta, double phi, int L, int M);

    static int factorial(int N);


// ***************************************
//  MIN AND MAX FUNCTIONS
// ***************************************

    static double max();
    static double max(double x1);
    static double max(double x1, double x2);
    static double max(double x1, double x2, double x3);
    static double max(double x1, double x2, double x3, double x4);
    static double max(double x1, double x2, double x3, double x4, double x5);
    static double max(double x1, double x2, double x3, double x4, double x5, double x6);
    static double max(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7);
    static double max(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7, double x8);
    static double max(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7, double x8, double x9);
    static double max(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7, double x8, double x9, double x10);

    static double min();
    static double min(double x1);
    static double min(double x1, double x2);
    static double min(double x1, double x2, double x3);
    static double min(double x1, double x2, double x3, double x4);
    static double min(double x1, double x2, double x3, double x4, double x5);
    static double min(double x1, double x2, double x3, double x4, double x5, double x6);
    static double min(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7);
    static double min(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7, double x8);
    static double min(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7, double x8, double x9);
    static double min(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7, double x8, double x9, double x10);


  private:


};


#endif
