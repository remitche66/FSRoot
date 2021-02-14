#if !(defined FSAMPTOOLSPLOTGENERATOR)
#define FSAMPTOOLSPLOTGENERATOR

#include "IUAmpTools/PlotGenerator.h"

class FitResults;
class Kinematics;

class FSAmpToolsPlotGenerator : public PlotGenerator
{
    
public:
    
  FSAmpToolsPlotGenerator( const FitResults& results );

  enum { 
    khm12 = 0, khm13, khm23, kdltz,
    kNumHists 
  };
    
private:
        
  void projectEvent( Kinematics* kin );

};

#endif
