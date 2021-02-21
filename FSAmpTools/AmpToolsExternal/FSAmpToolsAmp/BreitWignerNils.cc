#include <cassert>
#include <iostream>
#include <string>
#include <complex>
#include <cstdlib>

#include "TLorentzVector.h"
#include "IUAmpTools/Kinematics.h"
#include "FSAmpToolsAmp/BreitWignerNils.h"
#include "FSAmpToolsAmp/BarrierFactor.h"
//#include "FSAmpToolsAmp/PVectorFactory.h"

BreitWignerNils::BreitWignerNils( const vector< string >& args ) :
UserAmplitude< BreitWignerNils >(args)
{

  assert( args.size() >= 4 && args.size()<6 );

  m_decaystring = args[0];
  m_mass = AmpParameter(args[1]);
  m_width = AmpParameter(args[2]);
  m_l = atoi(args[3].c_str());
  m_radius = 0.1973;
  if(args.size()==5)
    m_radius = atof(args[4].c_str());  
	list_of_resonances = fitConfig::decayTree(m_decaystring);

  // check if this resonance decays to only two final state particles
  if(std::count(m_decaystring.begin(),m_decaystring.end(),',')==1)
    mode = 1;
  else
    mode = -1;

  // need to register any free parameters so the framework knows about them
  registerParameter( m_mass );
  registerParameter( m_width );

}


void
BreitWignerNils::calcUserVars( GDouble** pKin, GDouble* userVars ) const {
  
	TLorentzVector p4[10];


	for(int i=0;i<list_of_resonances.at(0).n_daug;i++)
		p4[list_of_resonances.at(0).i_daug.at(i)] = TLorentzVector(pKin[list_of_resonances.at(0).i_daug.at(i)-1][1], pKin[list_of_resonances.at(0).i_daug.at(i)-1][2], pKin[list_of_resonances.at(0).i_daug.at(i)-1][3], pKin[list_of_resonances.at(0).i_daug.at(i)-1][0]);

  TLorentzVector P1, P2;
	P1 = TLorentzVector(0,0,0,0);
	P2 = TLorentzVector(0,0,0,0);
  
  if(mode<0){
    for(int j=0;j<list_of_resonances.at(1).n_daug;j++){
      P1 += p4[list_of_resonances.at(1).i_daug.at(j)];
    }
    for(int j=0;j<list_of_resonances.at(1).n_rec;j++){
      P2 += p4[list_of_resonances.at(1).i_rec.at(j)];	
    }
  }
  else{
    P1 = p4[list_of_resonances.at(0).i_daug.at(0)];
    P2 = p4[list_of_resonances.at(0).i_daug.at(1)];
  }
  
  TLorentzVector CM = P1+P2;
  P1.Boost(-CM.BoostVector());
  double rho = P1.Rho();

  userVars[kMass2] = (CM).M2();
  userVars[kRho] = rho;
  userVars[kM1] = P1.M();
  userVars[kM2] = P2.M();

}


complex< GDouble >
BreitWignerNils::calcAmplitude( GDouble** pKin, GDouble* userVars ) const {


  GDouble mass2 = userVars[kMass2];
  //complex<double> rho0 = PVectorFactory::breakupMom(m_mass*m_mass,userVars[kM1],userVars[kM2]);
  complex<double> rho0 = 1.0;
  complex<double> amp = complex<GDouble>(1.0,0.0) /
                        complex<GDouble>( mass2 - m_mass*m_mass, m_mass*m_width);

  if(m_l)
    amp *= BarrierFactor::BlattWeisskopfRatio(m_l,userVars[kRho],rho0,m_radius);

  return amp;
}



#ifdef GPU_ACCELERATION
void
BreitWignerNils::launchGPUKernel( dim3 dimGrid, dim3 dimBlock, GPU_AMP_PROTO ) const {
  
  BreitWignerNils_exec( dimGrid,  dimBlock, GPU_AMP_ARGS, 
                       m_mass, m_width, m_daughter1, m_daughter2 );

}
#endif //GPU_ACCELERATION
