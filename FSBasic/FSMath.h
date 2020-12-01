#if !defined(FSMATH_H)
#define FSMATH_H

#include <cmath>
#include "TLorentzVector.h"

using namespace std;


class FSMath{

  public:

      // for the decay R --> A + B, helcostheta is the cosine of the polar angle 
      //  of particle A, where the z-axis is defined by particle R, boosted to the R rest frame
      //    [equivalently, helcostheta is the cosine of the angle between 
      //      the momenta of particles A and R, boosted to the R rest frame]

    static double helcostheta(double PxPA, double PyPA, double PzPA, double EnPA,
                              double PxPB, double PyPB, double PzPB, double EnPB){
      TLorentzVector PA(PxPA,PyPA,PzPA,EnPA);
      TLorentzVector PB(PxPB,PyPB,PzPB,EnPB);
      TLorentzVector PR = PA + PB;
      double thetaR = PR.Theta();
      double phiR = PR.Phi();
        // rotate PR and PA so PR is aligned along the z-axis
      PR.RotateZ(-1.0*phiR);      PA.RotateZ(-1.0*phiR);
      PR.RotateY(-1.0*thetaR);    PA.RotateY(-1.0*thetaR);
        // boost PA to the PR rest frame
      PA.Boost(-1.0*PR.BoostVector());
      return PA.CosTheta();
    }


      // for the decay chain R0 --> R + C and R --> A + B, helphi is the azimuthal
      //  angle of particle A, where the z-axis is defined by particle R and the
      //   xz-plane is defined by particles C and R

    static double helphi(double PxPA, double PyPA, double PzPA, double EnPA,
                         double PxPB, double PyPB, double PzPB, double EnPB,
                         double PxPC, double PyPC, double PzPC, double EnPC){
      TLorentzVector PA(PxPA,PyPA,PzPA,EnPA);
      TLorentzVector PB(PxPB,PyPB,PzPB,EnPB);
      TLorentzVector PC(PxPC,PyPC,PzPC,EnPC);
      TLorentzVector PR = PA + PB;
        // rotate (PR and) PC and PA so PR is aligned along the z-axis
      PA.RotateZ(-1.0*PR.Phi());    PC.RotateZ(-1.0*PR.Phi());  
      PA.RotateY(-1.0*PR.Theta());  PC.RotateY(-1.0*PR.Theta());
        // rotate (PR, PC, and) PA so PC is in the xz-plane
      PA.RotateZ(-1.0*PC.Phi());
      return PA.Phi();
    }

      // for the decay R --> A + B, with the z-axis defined by external particle E (like a beam),
      //  gjcostheta is the cosine of the polar angle of particle A, where the z-axis is 
      //  defined by particle E, all boosted to the R rest frame

    static double gjcostheta(double PxPA, double PyPA, double PzPA, double EnPA,
                             double PxPB, double PyPB, double PzPB, double EnPB,
                             double PxPE, double PyPE, double PzPE, double EnPE){
      TLorentzVector PA(PxPA,PyPA,PzPA,EnPA);
      TLorentzVector PB(PxPB,PyPB,PzPB,EnPB);
      TLorentzVector PE(PxPE,PyPE,PzPE,EnPE);
      TLorentzVector PR = PA + PB;
        // boost PA and PE to the PR rest frame
      PA.Boost(-1.0*PR.BoostVector());
      PE.Boost(-1.0*PR.BoostVector());
        // rotate PA (and PE) so that PE is aligned along the z-axis
      PA.RotateZ(-1.0*PE.Phi());
      PA.RotateY(-1.0*PE.Theta());
      return PA.CosTheta();
    }


      // for three four-vectors P1, P2, and P3, planephi is the rotation of the P2-P3 plane
      //  with respect to the P1-P2 plane

    static double planephi(double PxP1, double PyP1, double PzP1, double EnP1,
                           double PxP2, double PyP2, double PzP2, double EnP2,
                           double PxP3, double PyP3, double PzP3, double EnP3){
      TLorentzVector P1(PxP1,PyP1,PzP1,EnP1);
      TLorentzVector P2(PxP2,PyP2,PzP2,EnP2);
      TLorentzVector P3(PxP3,PyP3,PzP3,EnP3);
        // rotate P1 (and P2) and P3 so P2 is aligned along the z-axis
      P1.RotateZ(-1.0*P2.Phi());    P3.RotateZ(-1.0*P2.Phi());  
      P1.RotateY(-1.0*P2.Theta());  P3.RotateY(-1.0*P2.Theta());
        // rotate (P1, P2, and) P3 so P1 is in the xz-plane
      P3.RotateZ(-1.0*P1.Phi());
      return P3.Phi();
    }


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

};


#endif
