
// *********************************************************************
// Example Tree for psi(2S) --> pi+ pi- J/psi with J/psi --> pi+ pi- pi0
// *********************************************************************

void makeTree3PI(){
  TString fileName("ExampleTree_0_221.root");
  TString treeName("ntExampleTree_0_221");
  cout << endl;
  cout << "---------------------" << endl;
  cout << "MAKING EXAMPLE TREE 1" << endl;
  cout << "---------------------" << endl;
  cout << "  REACTION: psi(2S) --> pi+ pi- J/psi with \n"
          "              J/psi --> pi+ pi- pi0 and \n"
          "                pi0 --> gamma gamma\n";
  cout << " FILE NAME: " << fileName << endl;
  cout << " TREE NAME: " << treeName << endl;
  cout << "  BRANCHES: " << endl;
  cout << "                               Run:  run number\n"
          "                             Event:  event number\n"
          "                           Chi2DOF:  chi2/dof of a kinematic fit\n"
          "           PxPCM PyPCM PzPCM EnPCM:  four-vector of the CM system\n"
          "   PxP[CD] PyP[CD] PzP[CD] EnP[CD]:  four-vector of particle [CD],\n"
          "                                     where [CD] = 1 for a pi+\n"
          "                                                  2 for the other pi+\n"
          "                                                  3 for a pi-\n"
          "                                                  4 for the other pi-\n"
          "                                                  5 for the pi0\n"
          "                                                  5a for a gamma from the pi0\n"
          "                                                  5b for the other gamma from the pi0\n"
          "                       TkChi2P[CD]:  track chi2 of particle [CD],\n"
          "                                     where [CD] = 1,2,3,4, as above\n" << endl;
  TFile file(fileName,"recreate");
  TTree tree(treeName,treeName);
  double Run, Event, Chi2DOF;
  double PxPCM, PyPCM, PzPCM, EnPCM;
  double PxP1,  PyP1,  PzP1,  EnP1, TkChi2P1;
  double PxP2,  PyP2,  PzP2,  EnP2, TkChi2P2;
  double PxP3,  PyP3,  PzP3,  EnP3, TkChi2P3;
  double PxP4,  PyP4,  PzP4,  EnP4, TkChi2P4;
  double PxP5,  PyP5,  PzP5,  EnP5;
  double PxP5a, PyP5a, PzP5a, EnP5a;
  double PxP5b, PyP5b, PzP5b, EnP5b;
  tree.Branch("Run",    &Run,      "Run/D");
  tree.Branch("Event",  &Event,    "Event/D");
  tree.Branch("Chi2DOF",&Chi2DOF,  "Chi2DOF/D");
  tree.Branch("PxPCM",  &PxPCM,    "PxPCM/D");
  tree.Branch("PyPCM",  &PyPCM,    "PyPCM/D");
  tree.Branch("PzPCM",  &PzPCM,    "PzPCM/D");
  tree.Branch("EnPCM",  &EnPCM,    "EnPCM/D");
  tree.Branch("PxP1",   &PxP1,     "PxP1/D");
  tree.Branch("PyP1",   &PyP1,     "PyP1/D");
  tree.Branch("PzP1",   &PzP1,     "PzP1/D");
  tree.Branch("EnP1",   &EnP1,     "EnP1/D");
  tree.Branch("PxP2",   &PxP2,     "PxP2/D");
  tree.Branch("PyP2",   &PyP2,     "PyP2/D");
  tree.Branch("PzP2",   &PzP2,     "PzP2/D");
  tree.Branch("EnP2",   &EnP2,     "EnP2/D");
  tree.Branch("PxP3",   &PxP3,     "PxP3/D");
  tree.Branch("PyP3",   &PyP3,     "PyP3/D");
  tree.Branch("PzP3",   &PzP3,     "PzP3/D");
  tree.Branch("EnP3",   &EnP3,     "EnP3/D");
  tree.Branch("PxP4",   &PxP4,     "PxP4/D");
  tree.Branch("PyP4",   &PyP4,     "PyP4/D");
  tree.Branch("PzP4",   &PzP4,     "PzP4/D");
  tree.Branch("EnP4",   &EnP4,     "EnP4/D");
  tree.Branch("PxP5",   &PxP5,     "PxP5/D");
  tree.Branch("PyP5",   &PyP5,     "PyP5/D");
  tree.Branch("PzP5",   &PzP5,     "PzP5/D");
  tree.Branch("EnP5",   &EnP5,     "EnP5/D");
  tree.Branch("PxP5a",  &PxP5a,    "PxP5a/D");
  tree.Branch("PyP5a",  &PyP5a,    "PyP5a/D");
  tree.Branch("PzP5a",  &PzP5a,    "PzP5a/D");
  tree.Branch("EnP5a",  &EnP5a,    "EnP5a/D");
  tree.Branch("PxP5b",  &PxP5b,    "PxP5b/D");
  tree.Branch("PyP5b",  &PyP5b,    "PyP5b/D");
  tree.Branch("PzP5b",  &PzP5b,    "PzP5b/D");
  tree.Branch("EnP5b",  &EnP5b,    "EnP5b/D");
  tree.Branch("TkChi2P1", &TkChi2P1, "TkChi2P1/D");
  tree.Branch("TkChi2P2", &TkChi2P2, "TkChi2P2/D");
  tree.Branch("TkChi2P3", &TkChi2P3, "TkChi2P3/D");
  tree.Branch("TkChi2P4", &TkChi2P4, "TkChi2P4/D");
  TGenPhaseSpace generator1;
  TGenPhaseSpace generator2;
  TGenPhaseSpace generator3;
  int nEvents = 0;
  while (nEvents < 10000){
      // psi(2S) --> pi+ pi- J/psi
    TLorentzVector pPsi2S(0.0, 0.0, 0.0, 3.686);
    double mJPsi = gRandom->Gaus(FSPhysics::XMJpsi,0.02);
    double mChildren1[3] = {FSPhysics::XMpi, FSPhysics::XMpi, mJPsi};
    generator1.SetDecay(pPsi2S, 3, mChildren1);
    double maxWt1 = generator1.GetWtMax();
    while (generator1.Generate() < gRandom->Uniform() * maxWt1){}
    TLorentzVector* pPip1 = generator1.GetDecay(0);
    TLorentzVector* pPim1 = generator1.GetDecay(1);
    TLorentzVector* pJPsi = generator1.GetDecay(2);
      // J/psi --> pi+ pi- pi0
    double mChildren2[3] = {FSPhysics::XMpi, FSPhysics::XMpi, FSPhysics::XMpi0};
    generator2.SetDecay(*pJPsi, 3, mChildren2);
    double maxWt2 = generator2.GetWtMax();
    while (generator2.Generate() < gRandom->Uniform() * maxWt2){}
    TLorentzVector* pPip2 = generator2.GetDecay(0);
    TLorentzVector* pPim2 = generator2.GetDecay(1);
    TLorentzVector* pPi01 = generator2.GetDecay(2);
      // pi0 --> gamma gamma
    double mChildren3[2] = {0.0,0.0};
    generator3.SetDecay(*pPi01, 2, mChildren3);
    double maxWt3 = generator3.GetWtMax();
    while (generator3.Generate() < gRandom->Uniform() * maxWt3){}
    TLorentzVector* pGam1 = generator2.GetDecay(0);
    TLorentzVector* pGam2 = generator2.GetDecay(1);
      // fill variables
    Run   = (int)(gRandom->Uniform(10)+1200.0);
    Event = (int)(gRandom->Uniform(100)+1.0);
    Chi2DOF  = (pow(gRandom->Gaus(),2.0)+pow(gRandom->Gaus(),2.0)+pow(gRandom->Gaus(),2.0)
               +pow(gRandom->Gaus(),2.0)+pow(gRandom->Gaus(),2.0)+pow(gRandom->Gaus(),2.0))/6.0*2.0;
    TkChi2P1 = pow(gRandom->Gaus(),2.0);
    TkChi2P2 = pow(gRandom->Gaus(),2.0);
    TkChi2P3 = pow(gRandom->Gaus(),2.0);
    TkChi2P4 = pow(gRandom->Gaus(),2.0);
    PxPCM = pPsi2S.Px();  PyPCM = pPsi2S.Py();  PzPCM = pPsi2S.Pz();  EnPCM = pPsi2S.E();
    PxP1  = pPip1->Px();  PyP1  = pPip1->Py();  PzP1  = pPip1->Pz();  EnP1  = pPip1->E();
    PxP2  = pPip2->Px();  PyP2  = pPip2->Py();  PzP2  = pPip2->Pz();  EnP2  = pPip2->E();
    PxP3  = pPim1->Px();  PyP3  = pPim1->Py();  PzP3  = pPim1->Pz();  EnP3  = pPim1->E();
    PxP4  = pPim2->Px();  PyP4  = pPim2->Py();  PzP4  = pPim2->Pz();  EnP4  = pPim2->E();
    PxP5  = pPi01->Px();  PyP5  = pPi01->Py();  PzP5  = pPi01->Pz();  EnP5  = pPi01->E();
    PxP5a = pGam1->Px();  PyP5a = pGam1->Py();  PzP5a = pGam1->Pz();  EnP5a = pGam1->E();
    PxP5b = pGam2->Px();  PyP5b = pGam2->Py();  PzP5b = pGam2->Pz();  EnP5b = pGam2->E();
    nEvents++;
    tree.Fill();
  }
  tree.Write();
  file.Close();
}


// *********************************************************************
// Example Tree for psi(2S) --> pi+ pi- J/psi with J/psi --> K+ Ks pi-
// *********************************************************************

void makeTreeKKPI(){
  TString fileName("ExampleTree_0_101120.root");
  TString treeName("ntExampleTree_0_101120");
  cout << endl;
  cout << "---------------------" << endl;
  cout << "MAKING EXAMPLE TREE 2" << endl;
  cout << "---------------------" << endl;
  cout << "  REACTION: psi(2S) --> pi+ pi- J/psi with \n"
          "              J/psi --> K+ Ks pi- and \n"
          "                 Ks --> pi+ pi-\n";
  cout << " FILE NAME: " << fileName << endl;
  cout << " TREE NAME: " << treeName << endl;
  cout << "  BRANCHES: " << endl;
  cout << "                               Run:  run number\n"
          "                             Event:  event number\n"
          "                           Chi2DOF:  chi2/dof of a kinematic fit\n"
          "           PxPCM PyPCM PzPCM EnPCM:  four-vector of the CM system\n"
          "   PxP[CD] PyP[CD] PzP[CD] EnP[CD]:  four-vector of particle [CD],\n"
          "                                     where [CD] = 1 for the K+\n"
          "                                                  2 for the Ks\n"
          "                                                  2a for the pi+ from the Ks\n"
          "                                                  2b for the pi- from the Ks\n"
          "                                                  3 for the pi+\n"
          "                                                  4 for a pi-\n"
          "                                                  5 for the other pi-\n"
          "                       TkChi2P[CD]:  track chi2 of particle [CD],\n"
          "                                     where [CD] = 1,2a,2b,3,4,5, as above\n" << endl;
  TFile file(fileName,"recreate");
  TTree tree(treeName,treeName);
  double Run, Event, Chi2DOF;
  double PxPCM, PyPCM, PzPCM, EnPCM;
  double PxP1,  PyP1,  PzP1,  EnP1, TkChi2P1;
  double PxP2,  PyP2,  PzP2,  EnP2;
  double PxP2a, PyP2a, PzP2a, EnP2a, TkChi2P2a;
  double PxP2b, PyP2b, PzP2b, EnP2b, TkChi2P2b;
  double PxP3,  PyP3,  PzP3,  EnP3, TkChi2P3;
  double PxP4,  PyP4,  PzP4,  EnP4, TkChi2P4;
  double PxP5,  PyP5,  PzP5,  EnP5, TkChi2P5;
  tree.Branch("Run",    &Run,      "Run/D");
  tree.Branch("Event",  &Event,    "Event/D");
  tree.Branch("Chi2DOF",&Chi2DOF,  "Chi2DOF/D");
  tree.Branch("PxPCM",  &PxPCM,    "PxPCM/D");
  tree.Branch("PyPCM",  &PyPCM,    "PyPCM/D");
  tree.Branch("PzPCM",  &PzPCM,    "PzPCM/D");
  tree.Branch("EnPCM",  &EnPCM,    "EnPCM/D");
  tree.Branch("PxP1",   &PxP1,     "PxP1/D");
  tree.Branch("PyP1",   &PyP1,     "PyP1/D");
  tree.Branch("PzP1",   &PzP1,     "PzP1/D");
  tree.Branch("EnP1",   &EnP1,     "EnP1/D");
  tree.Branch("PxP2",   &PxP2,     "PxP2/D");
  tree.Branch("PyP2",   &PyP2,     "PyP2/D");
  tree.Branch("PzP2",   &PzP2,     "PzP2/D");
  tree.Branch("EnP2",   &EnP2,     "EnP2/D");
  tree.Branch("PxP2a",  &PxP2a,    "PxP2a/D");
  tree.Branch("PyP2a",  &PyP2a,    "PyP2a/D");
  tree.Branch("PzP2a",  &PzP2a,    "PzP2a/D");
  tree.Branch("EnP2a",  &EnP2a,    "EnP2a/D");
  tree.Branch("PxP2b",  &PxP2b,    "PxP2b/D");
  tree.Branch("PyP2b",  &PyP2b,    "PyP2b/D");
  tree.Branch("PzP2b",  &PzP2b,    "PzP2b/D");
  tree.Branch("EnP2b",  &EnP2b,    "EnP2b/D");
  tree.Branch("PxP3",   &PxP3,     "PxP3/D");
  tree.Branch("PyP3",   &PyP3,     "PyP3/D");
  tree.Branch("PzP3",   &PzP3,     "PzP3/D");
  tree.Branch("EnP3",   &EnP3,     "EnP3/D");
  tree.Branch("PxP4",   &PxP4,     "PxP4/D");
  tree.Branch("PyP4",   &PyP4,     "PyP4/D");
  tree.Branch("PzP4",   &PzP4,     "PzP4/D");
  tree.Branch("EnP4",   &EnP4,     "EnP4/D");
  tree.Branch("PxP5",   &PxP5,     "PxP5/D");
  tree.Branch("PyP5",   &PyP5,     "PyP5/D");
  tree.Branch("PzP5",   &PzP5,     "PzP5/D");
  tree.Branch("EnP5",   &EnP5,     "EnP5/D");
  tree.Branch("TkChi2P1", &TkChi2P1, "TkChi2P1/D");
  tree.Branch("TkChi2P2a", &TkChi2P2a, "TkChi2P2a/D");
  tree.Branch("TkChi2P2b", &TkChi2P2b, "TkChi2P2b/D");
  tree.Branch("TkChi2P3", &TkChi2P3, "TkChi2P3/D");
  tree.Branch("TkChi2P4", &TkChi2P4, "TkChi2P4/D");
  tree.Branch("TkChi2P5", &TkChi2P5, "TkChi2P5/D");
  TGenPhaseSpace generator1;
  TGenPhaseSpace generator2;
  TGenPhaseSpace generator3;
  int nEvents = 0;
  while (nEvents < 10000){
      // psi(2S) --> pi+ pi- J/psi
    TLorentzVector pPsi2S(0.0, 0.0, 0.0, 3.686);
    double mJPsi = gRandom->Gaus(FSPhysics::XMJpsi,0.02);
    double mChildren1[3] = {FSPhysics::XMpi, FSPhysics::XMpi, mJPsi};
    generator1.SetDecay(pPsi2S, 3, mChildren1);
    double maxWt1 = generator1.GetWtMax();
    while (generator1.Generate() < gRandom->Uniform() * maxWt1){}
    TLorentzVector* pPip1 = generator1.GetDecay(0);
    TLorentzVector* pPim1 = generator1.GetDecay(1);
    TLorentzVector* pJPsi = generator1.GetDecay(2);
      // J/psi --> K+ Ks pi-
    double mChildren2[3] = {FSPhysics::XMK, FSPhysics::XMKs, FSPhysics::XMpi};
    generator2.SetDecay(*pJPsi, 3, mChildren2);
    double maxWt2 = generator2.GetWtMax();
    while (generator2.Generate() < gRandom->Uniform() * maxWt2){}
    TLorentzVector* pKp1  = generator2.GetDecay(0);
    TLorentzVector* pKs1  = generator2.GetDecay(1);
    TLorentzVector* pPim2 = generator2.GetDecay(2);
      // Ks --> pi+ pi-
    double mChildren3[2] = {FSPhysics::XMpi,FSPhysics::XMpi};
    generator3.SetDecay(*pKs1, 2, mChildren3);
    double maxWt3 = generator3.GetWtMax();
    while (generator3.Generate() < gRandom->Uniform() * maxWt3){}
    TLorentzVector* pKs1a = generator2.GetDecay(0);
    TLorentzVector* pKs1b = generator2.GetDecay(1);
      // fill variables
    Run   = (int)(gRandom->Uniform(10)+1200.0);
    Event = (int)(gRandom->Uniform(100)+1.0);
    Chi2DOF  = (pow(gRandom->Gaus(),2.0)+pow(gRandom->Gaus(),2.0)+pow(gRandom->Gaus(),2.0)
               +pow(gRandom->Gaus(),2.0)+pow(gRandom->Gaus(),2.0)+pow(gRandom->Gaus(),2.0))/6.0*2.0;
    TkChi2P1 = pow(gRandom->Gaus(),2.0);
    TkChi2P2a = pow(gRandom->Gaus(),2.0);
    TkChi2P2b = pow(gRandom->Gaus(),2.0);
    TkChi2P3 = pow(gRandom->Gaus(),2.0);
    TkChi2P4 = pow(gRandom->Gaus(),2.0);
    TkChi2P5 = pow(gRandom->Gaus(),2.0);
    PxPCM = pPsi2S.Px();  PyPCM = pPsi2S.Py();  PzPCM = pPsi2S.Pz();  EnPCM = pPsi2S.E();
    PxP1  = pKp1->Px();   PyP1  = pKp1->Py();   PzP1  = pKp1->Pz();   EnP1  = pKp1->E();
    PxP2  = pKs1->Px();   PyP2  = pKs1->Py();   PzP2  = pKs1->Pz();   EnP2  = pKs1->E();
    PxP2a = pKs1a->Px();  PyP2a = pKs1a->Py();  PzP2a = pKs1a->Pz();  EnP2a = pKs1a->E();
    PxP2b = pKs1b->Px();  PyP2b = pKs1b->Py();  PzP2b = pKs1b->Pz();  EnP2b = pKs1b->E();
    PxP3  = pPip1->Px();  PyP3  = pPip1->Py();  PzP3  = pPip1->Pz();  EnP3  = pPip1->E();
    PxP4  = pPim1->Px();  PyP4  = pPim1->Py();  PzP4  = pPim1->Pz();  EnP4  = pPim1->E();
    PxP5  = pPim2->Px();  PyP5  = pPim2->Py();  PzP5  = pPim2->Pz();  EnP5  = pPim2->E();
    nEvents++;
    tree.Fill();
  }
  tree.Write();
  file.Close();
}

// **********************
// Make All Example Trees
// **********************

void makeExampleTrees(){
  makeTree3PI();
  makeTreeKKPI();
}

