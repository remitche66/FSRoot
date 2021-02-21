#include "helicity.h"

// this is from FSMath:
// ***************************************                                                                                                                                                                
//  HELICITY ANGLES                                                                                                                                                                 
// ***************************************                                                                                                                                                                
//                                                                                                                                                                                                        
//  Calculate the helicity or Gottfried-Jackson angles for particle A produced in either of                                                                                                               
//  these three-step processes:                                                                                                                                                                           
//    (1) D + Q --> S;  S --> R + C;  R --> A + B                                                                                                                                                         
//          examples:                                                                                                                                                                                     
//            e+(D) e-(Q) --> J/psi(S);  J/psi(S) --> rho0(R) pi0(C);  rho0(R) --> pi+(A) pi-(B)                                                                                                          
//            gamma(D) p(Q) --> CM(S);  CM(S) --> phi(R) p'(C);  phi(R) --> K+(A) K-(B)                                                                                                                   
//    (2) Q --> D + S;  S --> R + C;  R --> A + B                                                                                                                                                         
//          example:                                                                                                                                                                                      
//            chi_c1(Q) --> gamma(D) J/psi(S);  J/psi(S) --> rho0(R) pi0(C);  rho0(R) --> pi+(A) pi-(B)                                                                                                   
//  Procedure for helicity angles:                                                                                                                                                                        
//    * First boost all four-vectors to the S rest frame, where R and C are back-to-back [pR = -pC]                                                                                                       
//        and D and Q are either back-to-back (1) or parallel (2) [pD = +-pQ].                                                                                                                            
//    * Rotate all four-vectors so pR and -pC are along the z-axis.                                                                                                                                       
//    * Rotate all four-vectors around the z-axis so pD and +-pQ are in the xz-plane.                                                                                                                     
//    * Now the xz-plane is defined by DQ and RC.                                                                                                                                                         
//    * Boost all four-vectors along the z-axis to the R rest frame.                                                                                                                                      
//    * Return the cosine of the polar angle of pA (helcostheta)                                                                                                                                          
//        or the azimuthal angle of pA (helphi).  
// ***************************************    


TLorentzVector helicity::helvector(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD){
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
      PA.RotateZ(TMath::Pi()-1.0*PD.Phi());
      PR.RotateZ(TMath::Pi()-1.0*PD.Phi());
         // boost to the PR rest frame                                                                                                                                                                         
      PA.Boost(-1.0*PR.BoostVector());
      return PA;
   }

   double helicity::heltheta(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC){
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
      return PA.Theta();
   }

   double helicity::helcostheta(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC){
      return cos(helicity::heltheta(PA,PB,PC));
   }

   double helicity::helphi(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD){
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
      PA.RotateZ(TMath::Pi()-1.0*PD.Phi());
      PR.RotateZ(TMath::Pi()-1.0*PD.Phi());
         // boost to the PR rest frame                                                                                                                                                                         
      PA.Boost(-1.0*PR.BoostVector());
      return PA.Phi();
   }

   double helicity::helrho(TLorentzVector PA, TLorentzVector PB, TLorentzVector PC, TLorentzVector PD){
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
      PA.RotateZ(TMath::Pi()-1.0*PD.Phi());
      PR.RotateZ(TMath::Pi()-1.0*PD.Phi());
         // boost to the PR rest frame                                                                                                                                                                         
      PA.Boost(-1.0*PR.BoostVector());
      return PA.Rho();
   }

TLorentzVector helicity::prodvector(TLorentzVector PR, TLorentzVector PC, TLorentzVector PD){
  TLorentzVector PS = PR + PC;
   // adding the same choice of x-y axes as in PAWIAN:
   TLorentzVector refRecoilTrafo = TLorentzVector(3.0,0.,0.,10.) - PD;
    // boost all needed four-vectors to the S rest frame
  PR.Boost(-1.0*PS.BoostVector());
  PD.Boost(-1.0*PS.BoostVector());
  refRecoilTrafo.Boost(-1.0*PS.BoostVector());
    // rotate so PD is aligned along the z-axis
  PR.RotateZ(-1.0*PD.Phi());
  PR.RotateY(TMath::Pi()-1.0*PD.Theta());
  refRecoilTrafo.RotateZ(-1.0*PD.Phi());
  refRecoilTrafo.RotateY(TMath::Pi()-1.0*PD.Theta());
  //rotate around the z-axis so that refRecoil lies in the x-z plane
  PR.RotateZ(TMath::Pi()-refRecoilTrafo.Phi()); 
  return PR;
}

double helicity::prodtheta(TLorentzVector PR, TLorentzVector PC, TLorentzVector PD){
  TLorentzVector PS = PR + PC;
   // adding the same choice of x-y axes as in PAWIAN:
   TLorentzVector refRecoilTrafo = TLorentzVector(3.0,0.,0.,10.) - PD;
    // boost all needed four-vectors to the S rest frame
  PR.Boost(-1.0*PS.BoostVector());
  PD.Boost(-1.0*PS.BoostVector());
  refRecoilTrafo.Boost(-1.0*PS.BoostVector());
    // rotate so PD is aligned along the z-axis
  PR.RotateZ(-1.0*PD.Phi());
  PR.RotateY(TMath::Pi()-1.0*PD.Theta());
  refRecoilTrafo.RotateZ(-1.0*PD.Phi());
  refRecoilTrafo.RotateY(TMath::Pi()-1.0*PD.Theta());
  //rotate around the z-axis so that refRecoil lies in the x-z plane
  PR.RotateZ(TMath::Pi()-refRecoilTrafo.Phi()); 
  return PR.Theta();
}

double helicity::prodcostheta(TLorentzVector PR, TLorentzVector PC, TLorentzVector PD){
   return cos(helicity::prodtheta(PR,PC,PD));
}

double helicity::prodphi(TLorentzVector PR, TLorentzVector PC, TLorentzVector PD){
  TLorentzVector PS = PR + PC;
   // adding the same choice of x-y axes as in PAWIAN:
   TLorentzVector refRecoilTrafo = TLorentzVector(3.0,0.,0.,10.) - PD;
    // boost all needed four-vectors to the S rest frame
  PR.Boost(-1.0*PS.BoostVector());
  PD.Boost(-1.0*PS.BoostVector());
  refRecoilTrafo.Boost(-1.0*PS.BoostVector());
    // rotate so PD is aligned along the z-axis
  PR.RotateZ(-1.0*PD.Phi());
  PR.RotateY(TMath::Pi()-1.0*PD.Theta());
  refRecoilTrafo.RotateZ(-1.0*PD.Phi());
  refRecoilTrafo.RotateY(TMath::Pi()-1.0*PD.Theta());
  //rotate around the z-axis so that refRecoil lies in the x-z plane
  PR.RotateZ(TMath::Pi()-refRecoilTrafo.Phi()); 
  return PR.Phi();
}

double helicity::prodrho(TLorentzVector PR, TLorentzVector PC, TLorentzVector PD){
  TLorentzVector PS = PR + PC;

   // adding the same choice of x-y axes as in PAWIAN:
   TLorentzVector refRecoilTrafo = TLorentzVector(3.0,0.,0.,10.) - PD;

    // boost all needed four-vectors to the S rest frame
  PR.Boost(-1.0*PS.BoostVector());
  PD.Boost(-1.0*PS.BoostVector());
  refRecoilTrafo.Boost(-1.0*PS.BoostVector());
    // rotate so PD is aligned along the z-axis
  PR.RotateZ(-1.0*PD.Phi());
  PR.RotateY(TMath::Pi()-1.0*PD.Theta());
  refRecoilTrafo.RotateZ(-1.0*PD.Phi());
  refRecoilTrafo.RotateY(TMath::Pi()-1.0*PD.Theta());
  //rotate around the z-axis so that refRecoil lies in the x-z plane
  PR.RotateZ(TMath::Pi()-refRecoilTrafo.Phi()); 
  return PR.Rho();
}


TLorentzVector helicity::heliVec(const TLorentzVector& motherRef,  const TLorentzVector& ref, const TLorentzVector& mother, const TLorentzVector& daughter){
  TLorentzVector result=daughter;  // PA
  TLorentzVector refTrafo=ref;     // R0 = R+C
  TLorentzVector refRecoilTrafo=motherRef-ref; 
  //boost all vectors into the mother rest frame
  result.Boost(-mother.BoostVector());
  refTrafo.Boost(-mother.BoostVector());
  refRecoilTrafo.Boost(-mother.BoostVector());

  //rotate vectors so that refRecoilTrafo moves in the negative direction of the z-axis   
  result.RotateZ(-refTrafo.Phi());
  result.RotateY(TMath::Pi()-refTrafo.Theta());

  refRecoilTrafo.RotateZ(-refTrafo.Phi());
  refRecoilTrafo.RotateY(TMath::Pi()-refTrafo.Theta());

  //rotate around the z-axis so that refRecoil lies in the x-z plane
  result.RotateZ(TMath::Pi()-refRecoilTrafo.Phi()); 
  return result;
}