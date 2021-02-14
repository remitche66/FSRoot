#if !defined(FSAMPTOOLSDATAREADER)
#define FSAMPTOOLSDATAREADER

#include <string>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "IUAmpTools/DataReader.h"
#include "IUAmpTools/UserDataReader.h"

using namespace std;

class FSAmpToolsDataReader : public UserDataReader< FSAmpToolsDataReader >{

public:

  FSAmpToolsDataReader() : UserDataReader< FSAmpToolsDataReader >() { }

  FSAmpToolsDataReader( const vector< string >& args );

  string name() const { return "FSAmpToolsDataReader"; }

  virtual Kinematics* getEvent();

  virtual void resetSource();

  virtual unsigned int numEvents() const;

  int eventCounter() const { return m_eventCounter; }


private:

  TFile* m_inFile;
  TTree* m_inTree;
  int m_eventCounter;
  unsigned int m_numParticles;

  double m_EnP[50];
  double m_PxP[50];
  double m_PyP[50];
  double m_PzP[50];
  
  double m_weight;

};

#endif
