#if !defined(FSMATH_H)
#define FSMATH_H

#include <cmath>
#include "TLorentzVector.h"

using namespace std;


class FSMath{

  public:

      // for the decay R --> A + B, helcostheta is the cosine of the polar angle 
      //  of particle A in the R rest frame, where the z-axis is defined by the
      //  boost vector to the R rest frame
      //    [equivalently, helcostheta is the cosine of the angle between 
      //      the momenta of particles A and R, boosted to the R rest frame]

    static double helcostheta(double PxPA, double PyPA, double PzPA, double EnPA,
                              double PxPB, double PyPB, double PzPB, double EnPB);


      // for the decay chain R0 --> R + C and R --> A + B, helphi is the azimuthal
      //  angle of particle A, where the z-axis is defined by particle R and the
      //   xz-plane is defined by particles C and R
      //     [helphi is invariant to boosts along R]

    static double helphi(double PxPA, double PyPA, double PzPA, double EnPA,
                         double PxPB, double PyPB, double PzPB, double EnPB,
                         double PxPC, double PyPC, double PzPC, double EnPC);


      // for the decay R --> A + B, with the z-axis defined by external particle E (like a beam),
      //  gjcostheta is the cosine of the polar angle of particle A, where the z-axis is 
      //  defined by particle E, all boosted to the R rest frame

    static double gjcostheta(double PxPA, double PyPA, double PzPA, double EnPA,
                             double PxPB, double PyPB, double PzPB, double EnPB,
                             double PxPE, double PyPE, double PzPE, double EnPE);

      // for the decay chain R0 --> R + C and R --> A + B and the z-axis defined by 
      //  external particle E (like a beam), gjphi is the azimuthal angle of particle A 
      //  in the R frame, where the z-axis is defined by E in the R frame
      //  and the xz-plane is defined by C and E in the R frame

    static double gjphi(double PxPA, double PyPA, double PzPA, double EnPA,
                        double PxPB, double PyPB, double PzPB, double EnPB,
                        double PxPC, double PyPC, double PzPC, double EnPC,
                        double PxPE, double PyPE, double PzPE, double EnPE);

      // for three four-vectors P1, P2, and P3, planephi is the rotation of the P2-P3 plane
      //  with respect to the P1-P2 plane

    static double planephi(double PxP1, double PyP1, double PzP1, double EnP1,
                           double PxP2, double PyP2, double PzP2, double EnP2,
                           double PxP3, double PyP3, double PzP3, double EnP3);

      // d function (from Perl 216)

    static double dJM1M2(double theta, double J, double M1, double M2);

    static double ReYLM(double theta, double phi, int L, int M);

    static int factorial(int N);

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
