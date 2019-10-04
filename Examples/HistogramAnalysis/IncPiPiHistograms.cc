#include <iostream>
#include <map>
#include "TChain.h"
#include "TH1F.h"
#include "TString.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSTree.h"
#include "FSBasic/FSHistogram.h"
#include "FSAnalysis/FSHistogramAnalysis.h"
#include "IncPiPiHistograms.h"

double EnP1, PxP1, PyP1, PzP1;
double EnP2, PxP2, PyP2, PzP2;
double TkCosThetaP1, TkCosThetaP2;
double TkProbPiP1, TkProbKP1, TkProbPP1, TkProbEP1;
double TkProbPiP2, TkProbKP2, TkProbPP2, TkProbEP2;
double TkRVtxP1, TkZVtxP1;
double TkRVtxP2, TkZVtxP2;
double VChi2;
double BeamEnergy;
double MCDecayParticle1, MCDecayParticle2, MCDecayParticle3;

double RecoilMass;
double Mass;
double Momentum1, Momentum2;
double OpeningAngle;



using namespace std;

int main(int argc, char **argv){

  if (argc < 4){
    cout << "Usage: IncPiPiHistograms <input file name (trees)> <tree name> <output file name (histograms)>" << endl;
    exit(0);
  }

  TString inFileName(argv[1]);
  TString inTreeName(argv[2]);
  TString outFileName(argv[3]);

  cout << "**********************************" << endl;
  cout << "MAKING HISTOGRAMS FROM TREES" << endl;
  cout << "**********************************" << endl;
  cout << "  input file name =  " << inFileName << endl;
  cout << "  input tree name =  " << inTreeName << endl;
  cout << " output file name =  " << outFileName << endl;
  cout << "**********************************" << endl;
  cout << endl << endl;

  TChain* nt = FSTree::getTChain(inFileName,inTreeName);

  IncPiPiHistograms incpipi;
  incpipi.analyze(nt,FSString::TString2string(outFileName));

  return 0;
}




void 
IncPiPiHistograms::setupBranches(TChain* nt){
  if (nt){
    nt->SetBranchAddress("EnP1",&EnP1);
    nt->SetBranchAddress("PxP1",&PxP1);
    nt->SetBranchAddress("PyP1",&PyP1);
    nt->SetBranchAddress("PzP1",&PzP1);
    nt->SetBranchAddress("EnP2",&EnP2);
    nt->SetBranchAddress("PxP2",&PxP2);
    nt->SetBranchAddress("PyP2",&PyP2);
    nt->SetBranchAddress("PzP2",&PzP2);
    nt->SetBranchAddress("TkCosThetaP1",&TkCosThetaP1);
    nt->SetBranchAddress("TkCosThetaP2",&TkCosThetaP2);
    nt->SetBranchAddress("TkProbPiP1",&TkProbPiP1);
    nt->SetBranchAddress("TkProbKP1",&TkProbKP1);
    nt->SetBranchAddress("TkProbPP1",&TkProbPP1);
    nt->SetBranchAddress("TkProbEP1",&TkProbEP1);
    nt->SetBranchAddress("TkProbPiP2",&TkProbPiP2);
    nt->SetBranchAddress("TkProbKP2",&TkProbKP2);
    nt->SetBranchAddress("TkProbPP2",&TkProbPP2);
    nt->SetBranchAddress("TkProbEP2",&TkProbEP2);
    nt->SetBranchAddress("TkRVtxP1",&TkRVtxP1);
    nt->SetBranchAddress("TkZVtxP1",&TkZVtxP1);
    nt->SetBranchAddress("TkRVtxP2",&TkRVtxP2);
    nt->SetBranchAddress("TkZVtxP2",&TkZVtxP2);
    nt->SetBranchAddress("VChi2",&VChi2);
    nt->SetBranchAddress("BeamEnergy",&BeamEnergy);
/*
    if (m_type == 1){
      nt->SetBranchAddress("MCDecayParticle1",&MCDecayParticle1);
      nt->SetBranchAddress("MCDecayParticle2",&MCDecayParticle2);
      nt->SetBranchAddress("MCDecayParticle3",&MCDecayParticle3);
    }
*/
  }
}

void 
IncPiPiHistograms::setNCuts(){
  n1cuts = 13;
  n2cuts = 0;
  n3cuts = 2;
  n4cuts = 0;
  n5cuts = 0;
  n6cuts = 0;
  n7cuts = 0;
  n8cuts = 0;
  n9cuts = 0;
  //if (m_type == 1) n9cuts = 4;
}

void 
IncPiPiHistograms::bookHistograms(TString key){
  h1map["h100"+key] = new TH1F("h100"+key,"#pi^{+}#pi^{-} Recoil Mass",         2500,  2.5,  5.0);
  h1map["h200"+key] = new TH1F("h200"+key,"#pi^{+}#pi^{-} Mass",                2500,  0.0,  2.5);
  h1map["h301"+key] = new TH1F("h301"+key,"#pi^{+} Cosine(#theta)",             1000, -1.0,  1.0);
  h1map["h302"+key] = new TH1F("h302"+key,"#pi^{-} Cosine(#theta)",             1000, -1.0,  1.0);
  h1map["h401"+key] = new TH1F("h401"+key,"#pi^{+} Momentum",                   1500,  0.0,  1.5);
  h1map["h402"+key] = new TH1F("h402"+key,"#pi^{-} Momentum",                   1500,  0.0,  1.5);
  h1map["h500"+key] = new TH1F("h500"+key,"#pi^{+}#pi^{-} Opening Angle",       1000, -1.0,  1.0);
  h1map["h600"+key] = new TH1F("h600"+key,"#pi^{+}#pi^{-} Vertex Fit #chi^{2}",  500,  0.0, 50.0);
  h1map["h701"+key] = new TH1F("h701"+key,"#pi^{+} R Vertex",                   1000,  0.0, 20.0);
  h1map["h702"+key] = new TH1F("h702"+key,"#pi^{-} R Vertex",                   1000,  0.0, 20.0);
  h1map["h801"+key] = new TH1F("h801"+key,"#pi^{+} Z Vertex",                   2000,-20.0, 20.0);
  h1map["h802"+key] = new TH1F("h802"+key,"#pi^{-} Z Vertex",                   2000,-20.0, 20.0);
}


void 
IncPiPiHistograms::setCuts(){

  RecoilMass = (sqrt(pow(((2.0*BeamEnergy)               -(EnP1+EnP2)),2)-
                     pow((1.0*sin(0.011)*(2.0*BeamEnergy)-(PxP1+PxP2)),2)-
                     pow((0.0                            -(PyP1+PyP2)),2)-
                     pow((0.0                            -(PzP1+PzP2)),2)));

  Mass = (sqrt(pow((EnP1+EnP2),2)-
               pow((PxP1+PxP2),2)-
               pow((PyP1+PyP2),2)-
               pow((PzP1+PzP2),2)));

  Momentum1 = (sqrt(pow((PxP1),2)+
                    pow((PyP1),2)+
                    pow((PzP1),2)));

  Momentum2 = (sqrt(pow((PxP2),2)+
                    pow((PyP2),2)+
                    pow((PzP2),2)));

  OpeningAngle = ((PxP1)*(PxP2)+(PyP1)*(PyP2)+(PzP1)*(PzP2))
                  /Momentum1/Momentum2;


  l1[0] = true;
  if (abs((2.0*BeamEnergy)-3.810) < 0.002) l1[1]   = true;
  if (abs((2.0*BeamEnergy)-3.900) < 0.002) l1[2]   = true;
  if (abs((2.0*BeamEnergy)-4.090) < 0.002) l1[3]   = true;
  if (abs((2.0*BeamEnergy)-4.190) < 0.002) l1[4]   = true;
  if (abs((2.0*BeamEnergy)-4.210) < 0.002) l1[5]   = true;
  if (abs((2.0*BeamEnergy)-4.220) < 0.002) l1[6]   = true;
  if (abs((2.0*BeamEnergy)-4.230) < 0.002) l1[7]   = true;
  if (abs((2.0*BeamEnergy)-4.245) < 0.002) l1[8]   = true;
  if (abs((2.0*BeamEnergy)-4.260) < 0.002) l1[9]   = true;
  if (abs((2.0*BeamEnergy)-4.310) < 0.002) l1[10]  = true;
  if (abs((2.0*BeamEnergy)-4.360) < 0.002) l1[11]  = true;
  if (abs((2.0*BeamEnergy)-4.390) < 0.002) l1[12]  = true;
  if (abs((2.0*BeamEnergy)-4.420) < 0.002) l1[13]  = true;

  l2[0] = true;

  l3[0] = true;
  if (abs(OpeningAngle)<0.99){
  if ((TkRVtxP1<0.2)&&(TkRVtxP2<0.2)){
  if ((abs(TkZVtxP1)<3.0)&&(abs(TkZVtxP2)<3.0)){
  if ((abs(TkCosThetaP1)<0.93)&&(abs(TkCosThetaP2)<0.93)){
  if (VChi2<10.0){  
    l3[1] = true;
    if ((Mass < (0.4976 - 0.008)) || (Mass > (0.4976 + 0.005))){
      l3[2] = true;
    }
  }}}}}

  l4[0] = true;
  l5[0] = true;
  l6[0] = true;
  l7[0] = true;
  l8[0] = true;
  l9[0] = true;
/*
  if (m_type == 1){
    if (MCDecayParticle1==-211 && MCDecayParticle2==211 && MCDecayParticle3==443)    l9[1] = true;
    if (MCDecayParticle1==-211 && MCDecayParticle2==211 && MCDecayParticle3==10443)  l9[2] = true;
    if (MCDecayParticle1==-211 && MCDecayParticle2==211 && MCDecayParticle3==100443) l9[3] = true;
    if (MCDecayParticle1==22 && MCDecayParticle2==100443)                            l9[4] = true;
  }
*/

}

void 
IncPiPiHistograms::fillHistograms(TString key){
  h1map["h100"+key]->Fill(RecoilMass);
  h1map["h200"+key]->Fill(Mass);
  h1map["h301"+key]->Fill(TkCosThetaP1);
  h1map["h302"+key]->Fill(TkCosThetaP2);
  h1map["h401"+key]->Fill(Momentum1);
  h1map["h402"+key]->Fill(Momentum2);
  h1map["h500"+key]->Fill(OpeningAngle);
  h1map["h600"+key]->Fill(VChi2);
  h1map["h701"+key]->Fill(TkRVtxP1);
  h1map["h702"+key]->Fill(TkRVtxP2);
  h1map["h801"+key]->Fill(TkZVtxP1);
  h1map["h802"+key]->Fill(TkZVtxP2);
}




