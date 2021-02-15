
#include <vector>
#include <cassert>
#include <fstream>
#include <iostream>
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "IUAmpTools/Kinematics.h"
#include "FSAmpToolsDataIO/FSAmpToolsDataReader.h"
#include "TSystem.h"

using namespace std;

FSAmpToolsDataReader::FSAmpToolsDataReader( const vector< string >& args ) :
UserDataReader< FSAmpToolsDataReader >(args),
m_eventCounter( 0 ){
  
  assert(args.size() == 3);
  string inFileName(args[0]);
  string inTreeName(args[1]);
  m_numParticles = atoi(args[2].c_str());
  assert (m_numParticles < 50);

  TH1::AddDirectory( kFALSE );
  gSystem->Load( "libTree" );

  ifstream fileexists( inFileName.c_str() );
  if (fileexists){
    m_inFile = new TFile( inFileName.c_str() );
    m_inTree = static_cast<TTree*>( m_inFile->Get( inTreeName.c_str() ) );
  }
  else{
    cout << "FSAmpToolsDataReader WARNING:  Cannot find file... " << inFileName << endl;
    m_inFile = NULL;
    m_inTree = NULL;
  }

  if (m_inTree){
    for (unsigned int i = 0; i < m_numParticles; i++){
      TString sI("");  sI += (i+1);
      TString sEnPi = "EnP"+sI;
      TString sPxPi = "PxP"+sI;
      TString sPyPi = "PyP"+sI;
      TString sPzPi = "PzP"+sI;
      m_inTree->SetBranchAddress( sEnPi, &m_EnP[i] );
      m_inTree->SetBranchAddress( sPxPi, &m_PxP[i] );
      m_inTree->SetBranchAddress( sPyPi, &m_PyP[i] );
      m_inTree->SetBranchAddress( sPzPi, &m_PzP[i] );
      m_inTree->SetBranchAddress( "weight", &m_weight );
    }
  }

}


void
FSAmpToolsDataReader::resetSource(){
  m_eventCounter = 0;
}


Kinematics*
FSAmpToolsDataReader::getEvent(){
  if( m_eventCounter < numEvents() ){
    m_inTree->GetEntry( m_eventCounter++ );
    vector< TLorentzVector > particleList;
    for (unsigned int i = 0; i < m_numParticles; i++){
      particleList.push_back( TLorentzVector( m_PxP[i], m_PyP[i], m_PzP[i], m_EnP[i] ) );
    }
    return new Kinematics( particleList, m_weight );
  }
  else{
    return NULL;
  }
}


unsigned int
FSAmpToolsDataReader::numEvents() const{
  if (!m_inTree) return 0;
  return static_cast< unsigned int >( m_inTree->GetEntries() );
}

