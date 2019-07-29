double mJpsi = 3.096916;
double mpi0 = 0.134977;
double mpi = 0.13957;
double mK = 0.493677;
TString FN1("test_nt0_220.root");     TString NT1("nt0_220");
TString FN2("test_nt0_112.root");     TString NT2("nt0_112");
TString FN3("test_nt0_110002.root");  TString NT3("nt0_110002");
TString FNT("testHistContents.root"); TString NTT("TH1FContents");


void makeTree(double mParent, double mChild1, double mChild2, 
              double mChild3, double mChild4, TString treeName);


void step0_MakeTrees(){
  makeTree(mJpsi,mpi,mpi,mpi,mpi,"nt0_220");
  makeTree(mJpsi,mpi,mpi,mpi0,mpi0,"nt0_112");
  makeTree(mJpsi,mK,mK,mpi0,mpi0,"nt0_110002");
}

void step1_LookAtTrees1(){
  FSHistogram::getTH1F(FN1,NT1,"Chi2DOF","(60,0.0,6.0)","")->Draw();
  FSHistogram::getTH1F(FN2,NT2,"Chi2DOF","(60,0.0,6.0)","Chi2DOF<4.0")->Draw("hist,same");
  FSHistogram::getTH1F(FN3,NT3,"Chi2DOF","(60,0.0,6.0)","Chi2DOF<3.0")->Draw("hist,same");
}

void step2_LookAtTrees2(){
  FSHistogram::getTH1F(FN1,NT1,"MASS(1,2)","(100,0.0,3.0)","")->Draw();
  FSHistogram::getTH1F(FN2,NT2,"MASS(1,2)","(100,0.0,3.0)","Chi2DOF<4.0")->Draw("hist,same");
  FSHistogram::getTH1F(FN3,NT3,"MASS(1,2)","(100,0.0,3.0)","Chi2DOF<3.0")->Draw("hist,same");
}

void step3_SetUpModes(){
  if (FSModeCollection::modeVectorSize() == 0){
    FSModeCollection::addModeInfo("pi+ pi- pi+ pi-  M1");
    FSModeCollection::addModeInfo("pi+ pi- pi0 pi0  M2");
    FSModeCollection::addModeInfo(" K+  K- pi0 pi0  M3");
    //FSModeCollection::display();
  }
}

void step4_WriteHistTree(){
    // make a normal histogram and draw it
  FSHistogram::getTH1F(FN1,NT1,"Chi2DOF","(60,0.0,6.0)","")->Draw();
    // put the contents of the histogram in a TTree
  TTree* histTree = FSHistogram::getTH1FContents(FN1,NT1,"Chi2DOF","(60,0.0,6.0)","");
  histTree->Print();
    // write the TTree to a file
  TFile* tfile = new TFile("testHistContents.root","recreate");
  tfile->cd();
  histTree->Write();
  tfile->Close();
    // draw the histogram from the new TTree
  TString FNT("testHistContents.root");  TString NTT("TH1FContents");
  FSHistogram::getTH1F(FNT,NTT,"x","(60,0.0,6.0)","x<3.0")->Draw("hist,same");
}


void step5_TestContents(TString variable, TString bounds, TString cuts, float scale){
    // make a normal histogram and draw it
  FSHistogram::clearHistogramCache();
  FSHistogram::getTH1F(FN1,NT1,variable,bounds,cuts,"",scale)->Draw();
    // put the contents of the histogram in a TTree
  TTree* histTree = FSHistogram::getTH1FContents(FN1,NT1,variable,bounds,cuts,"",scale);
  //histTree->Print();
    // write the TTree to a file
  TFile* tfile = new TFile("testHistContents.root","recreate");
  tfile->cd();
  histTree->Write();
  tfile->Close();
    // draw the histogram from the new TTree
  FSHistogram::getTH1F(FNT,NTT,"x",bounds,"wt")->Draw("hist,same");
}


void step6_TestModeContents(TString cat, TString variable, TString bounds, TString cuts, float scale){
  FSHistogram::clearHistogramCache();
  step3_SetUpModes();
    // make a normal histogram and draw it
  FSModeHistogram::getTH1F(FN1,NT1,cat,variable,bounds,cuts,"",scale)->Draw();
    // put the contents of the histogram in a TTree
  TTree* histTree = FSModeHistogram::getTH1FContents(FN1,NT1,cat,variable,bounds,cuts,"",scale);
  //histTree->Print();
    // write the TTree to a file
  TFile* tfile = new TFile("testHistContents.root","recreate");
  tfile->cd();
  histTree->Write();
  tfile->Close();
    // draw the histogram from the new TTree
  FSHistogram::getTH1F(FNT,NTT,"x",bounds,"")->Draw("hist,same");
}


void step7_TestModeContents2(TString cat, TString variable, TString bounds, TString cuts, float scale){
  FSHistogram::clearHistogramCache();
  step3_SetUpModes();
  TCanvas* c1 = new TCanvas("c1","c1",1000,500);
  c1->Divide(2,1);
    // make a normal histogram and draw it
  c1->cd(1);
  FSModeHistogram::getTH2F(FN1,NT1,cat,variable,bounds,cuts,"",scale)->Draw("colz");
    // put the contents of the histogram in a TTree
  TTree* histTree = FSModeHistogram::getTH2FContents(FN1,NT1,cat,variable,bounds,cuts,"",scale);
  //histTree->Print();
    // write the TTree to a file
  TFile* tfile = new TFile("testHistContents.root","recreate");
  tfile->cd();
  histTree->Write();
  tfile->Close();
    // draw the histogram from the new TTree
  c1->cd(2);
  FSHistogram::getTH2F(FNT,NTT,"y:x",bounds,"")->Draw("colz");
}












void makeTree(double mParent, double mChild1, double mChild2, 
              double mChild3, double mChild4, TString treeName){

    // ***************************
    // SET UP A ROOT FILE AND TREE
    // ***************************

  TString fileName("test_"); fileName += treeName; fileName += ".root";
  TFile file(fileName,"recreate");
  TTree tree(treeName,treeName);

  double run, event, chi2dof;
  double PxPn[5];
  double PyPn[5];
  double PzPn[5];
  double EnPn[5];

  tree.Branch("Run",    &run,      "Run/D");
  tree.Branch("Event",  &event,    "Event/D");
  tree.Branch("Chi2DOF",&chi2dof,  "Chi2DOF/D");
  tree.Branch("PxPCM",  &PxPn[0],  "PxPCM/D");
  tree.Branch("PyPCM",  &PyPn[0],  "PyPCM/D");
  tree.Branch("PzPCM",  &PzPn[0],  "PzPCM/D");
  tree.Branch("EnPCM",  &EnPn[0],  "EnPCM/D");
  for (int i = 1; i <= 4; i++){
    TString si(""); si += i;
    tree.Branch("PxP"+si,  &PxPn[i],  "PxP"+si+"/D");  
    tree.Branch("PyP"+si,  &PyPn[i],  "PyP"+si+"/D");  
    tree.Branch("PzP"+si,  &PzPn[i],  "PzP"+si+"/D");  
    tree.Branch("EnP"+si,  &EnPn[i],  "EnP"+si+"/D");  
  }

    // ******************************
    // SET UP A PHASE SPACE GENERATOR
    // ******************************

  TGenPhaseSpace generator;
  TLorentzVector pParent(0.0, 0.0, 0.0, mParent);
  double mChildren[4] = {mChild1, mChild2, mChild3, mChild4};
  generator.SetDecay(pParent, 4, mChildren);
  double maxWt = generator.GetWtMax();

    // *********************************************
    // GENERATE PHASE SPACE EVENTS AND FILL THE TREE
    // *********************************************

  for (int i = 0; i < 1000000; i++){
    if (generator.Generate() < drand48() * maxWt) continue;
    run = 10;
    event = i;
    chi2dof = 5*gRandom->Rndm(); 
    PxPn[0] = pParent.Px();
    PyPn[0] = pParent.Py();
    PzPn[0] = pParent.Pz();
    EnPn[0] = pParent.E();
    for (int ip = 1; ip <= 4; ip++){
      PxPn[ip] = generator.GetDecay(ip-1)->Px();
      PyPn[ip] = generator.GetDecay(ip-1)->Py();
      PzPn[ip] = generator.GetDecay(ip-1)->Pz();
      EnPn[ip] = generator.GetDecay(ip-1)->E();
    }
    tree.Fill();
  }
  tree.Write();
  file.Close();

}
