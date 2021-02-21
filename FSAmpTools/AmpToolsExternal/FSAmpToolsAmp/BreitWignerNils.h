#if !defined(BREITWIGNERNILS)
#define BREITWIGNERNILS

#include "IUAmpTools/Amplitude.h"
#include "IUAmpTools/UserAmplitude.h"
#include "IUAmpTools/AmpParameter.h"
#include "GPUManager/GPUCustomTypes.h"
#include "FSAmpToolsAmp/fitConfig.h"

#include <utility>
#include <string>
#include <complex>
#include <vector>
#include <algorithm>

#ifdef GPU_ACCELERATION

void BreitWignerNils_exec( dim3 dimGrid, dim3 dimBlock, GPU_AMP_PROTO,
                          GDouble mass, GDouble width,
                          int daughter1, int daughter2 );

#endif // GPU_ACCELERATION

using std::complex;
using namespace std;

class Kinematics;

class BreitWignerNils : public UserAmplitude< BreitWignerNils >{

public:
  
  BreitWignerNils() : UserAmplitude< BreitWignerNils >() { }

  BreitWignerNils( const vector< string >& args );

  ~BreitWignerNils(){}

  string name() const { return "BreitWignerNils"; }

  complex< GDouble > calcAmplitude( GDouble** pKin, GDouble* userVars ) const;
  
  // **********************
  // The following lines are optional and can be used to precalcualte
  // user-defined data that the amplitudes depend on.
  
  // Use this for indexing a user-defined data array and notifying
  // the framework of the number of user-defined variables.
  enum UserVars { kMass2 = 0, kRho, kM1, kM2, kNumUserVars };
  unsigned int numUserVars() const { return kNumUserVars; }
  
  // This function needs to be defined -- see comments and discussion
  // in the .cc file.
  void calcUserVars( GDouble** pKin, GDouble* userVars ) const;
  
  // This is an optional addition if the calcAmplitude routine
  // can run with only the user-defined data and not the original
  // four-vectors.  It is used to optimize memory usage in GPU
  // based fits.
  //bool needsUserVarsOnly() const { return true; }
  bool needsUserVarsOnly() const { return true; }
  // **  end of optional lines **
  
#ifdef GPU_ACCELERATION

  void launchGPUKernel( dim3 dimGrid, dim3 dimBlock, GPU_AMP_PROTO ) const;

  bool isGPUEnabled() const { return true; }

#endif // GPU_ACCELERATION
  
private:
	
  string m_decaystring;
  AmpParameter m_mass;
  AmpParameter m_width;
  vector<res> list_of_resonances;
  int m_l;
  double m_radius;
  int mode;
};

#endif
