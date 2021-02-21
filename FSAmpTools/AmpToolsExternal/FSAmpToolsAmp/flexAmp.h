#if !defined(FLEXAMP)
#define FLEXAMP

#include "IUAmpTools/Amplitude.h"
#include "IUAmpTools/UserAmplitude.h"
#include "GPUManager/GPUCustomTypes.h"
#include "IUAmpTools/AmpParameter.h"

#include <string>
#include <complex>
#include <vector>
#include "TLorentzVector.h"
#include "TLorentzRotation.h"
#include "FSAmpToolsAmp/fitConfig.h"

using std::complex;
using namespace std;

struct FACTORF{
  int index;
  AmpParameter real;
  AmpParameter imag;
};



#ifdef GPU_ACCELERATION

void flexAmp_exec( dim3 dimGrid, dim3 dimBlock, GPU_AMP_PROTO,
                          int mJpsi, int ml, int ms,
                          int daughter1, int daughter2 );

#endif // GPU_ACCELERATION

class Kinematics;

class flexAmp : public UserAmplitude<flexAmp>
{
    
public:
  flexAmp() : UserAmplitude<flexAmp>() {}
  flexAmp(const vector<string> &args); 
  
  string name() const { return "flexAmp"; }
  
  complex< GDouble > calcAmplitude( GDouble** pKin, GDouble *userVars ) const;
  void printEvent( GDouble** pKin ) const;

  // **********************
  // The following lines are optional and can be used to precalcualte
  // user-defined data that the amplitudes depend on.
  
  // Use this for indexing a user-defined data array and notifying
  // the framework of the number of user-defined variables.
  
  enum UserVars { kReal=0, kImag,
                  kNumUserVars };
                              
  unsigned int numUserVars() const { return kNumUserVars; }
  
  // This function needs to be defined -- see comments and discussion
  // in the .cc file.
  void calcUserVars( GDouble** pKin, GDouble* userVars ) const;

  
  // This is an optional addition if the calcAmplitude routine
  // can run with only the user-defined data and not the original
  // four-vectors.  It is used to optimize memory usage in GPU
  // based fits.
  bool needsUserDataOnly() const { return !b_useFactor; }
  // **  end of optional lines **

#ifdef GPU_ACCELERATION

  void launchGPUKernel( dim3 dimGrid, dim3 dimBlock, GPU_AMP_PROTO ) const;

  bool isGPUEnabled() const { return true; }

#endif // GPU_ACCELERATION
  
 private:
  // indices of the four vectors
  int m_A1;
  int m_B1;
  int m_A2;
  int m_B2;
  int m_A3;
  int m_B3;
  vector<res> list_of_resonances;
	vector<double> qn_mother;
	string qn_mother_rad;
	vector<double> qn_res[10];
	string qn_res_rad[10];
	vector<double> qn_daug[20];

  bool b_useFactor;
  vector<FACTORF> extra_factors;

  map<int,string> typeToDecay;


  int type;
  bool m_print = false;
  
  TLorentzVector p4Ep;

static int m_ryandebug;
};

#endif
