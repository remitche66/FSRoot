#if !defined(FSAMPTOOLSDATAWRITER)
#define FSAMPTOOLSDATAWRITER

#include "IUAmpTools/Kinematics.h"

#include "TTree.h"
#include "TFile.h"

class FSAmpToolsDataWriter{

public:
		
  FSAmpToolsDataWriter( unsigned int numParticles, const string& outFile );
  ~FSAmpToolsDataWriter();

  void writeEvent( const Kinematics& kin );

  int eventCounter() const { return m_eventCounter; }

private:

  TFile* m_outFile;
  TTree* m_outTree;
  int m_eventCounter;
  unsigned int m_numParticles;

  double m_EnP[50];
  double m_PxP[50];
  double m_PyP[50];
  double m_PzP[50];

  double m_weight;
  
  double m_s12;
  double m_s23;

};

#endif
