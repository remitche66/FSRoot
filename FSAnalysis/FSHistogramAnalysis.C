#include <iostream>
#include <map>
#include "TChain.h"
#include "TH1F.h"
#include "TString.h"
#include "FSAnalysis/FSHistogramAnalysis.h"
#include "FSBasic/FSHistogram.h"
#include "FSBasic/FSString.h"


void 
FSHistogramAnalysis::analyze(TChain* nt, string outputfile){


    // Setup the number of cut combinations

  cout << "FSHistogramAnalysis: Setting Number of Cuts..." << endl;

  setNCuts();

  cout << "\tn1cuts: " << n1cuts << endl;
  cout << "\tn2cuts: " << n2cuts << endl;
  cout << "\tn3cuts: " << n3cuts << endl;
  cout << "\tn4cuts: " << n4cuts << endl;
  cout << "\tn5cuts: " << n5cuts << endl;
  cout << "\tn6cuts: " << n6cuts << endl;
  cout << "\tn7cuts: " << n7cuts << endl;
  cout << "\tn8cuts: " << n8cuts << endl;
  cout << "\tn9cuts: " << n9cuts << endl;


    // Setup for Tree Branches

  cout << "FSHistogramAnalysis: Setting Up Branches..." << endl;

  setupBranches(nt);


    // Book Histograms

  cout << "FSHistogramAnalysis: Booking Histograms..." << endl;

  for (int i1 = 0; i1 <= n1cuts; i1++){
  for (int i2 = 0; i2 <= n2cuts; i2++){
  for (int i3 = 0; i3 <= n3cuts; i3++){
  for (int i4 = 0; i4 <= n4cuts; i4++){
  for (int i5 = 0; i5 <= n5cuts; i5++){
  for (int i6 = 0; i6 <= n6cuts; i6++){
  for (int i7 = 0; i7 <= n7cuts; i7++){
  for (int i8 = 0; i8 <= n8cuts; i8++){
  for (int i9 = 0; i9 <= n9cuts; i9++){
    TString key("");key+=i9;key+=i8;key+=i7;key+=i6;key+=i5;key+=i4;key+=i3;key+=i2;key+=i1;
    bookHistograms(key);
  }}}}}}}}}


    // Loop Over the Tree

  cout << "FSHistogramAnalysis: Getting Number of Entries..." << endl;

  Long64_t nentries = nt->GetEntries();

  cout << "\tNumber of Entries = " << nentries << endl;

  cout << "FSHistogramAnalysis: Looping Over Tree..." << endl;

  for (long int ientry = 0; ientry < nentries; ientry++){
    if ((ientry+1)%100000 == 0){
      cout << "\tEntry Number: " << ientry+1 
           << "\t\t(" << 100*((double)(ientry+1))/nentries << " \%)" << endl;
    }
    nt->GetEntry(ientry);

       //for (int ievent = 1; ievent < 1000; ievent++){


      // Initialize the Cut Booleans

    for (int i = 0; i <= n1cuts; i++){ l1[i] = false; }
    for (int i = 0; i <= n2cuts; i++){ l2[i] = false; }
    for (int i = 0; i <= n3cuts; i++){ l3[i] = false; }
    for (int i = 0; i <= n4cuts; i++){ l4[i] = false; }
    for (int i = 0; i <= n5cuts; i++){ l5[i] = false; }
    for (int i = 0; i <= n6cuts; i++){ l6[i] = false; }
    for (int i = 0; i <= n7cuts; i++){ l7[i] = false; }
    for (int i = 0; i <= n8cuts; i++){ l8[i] = false; }
    for (int i = 0; i <= n9cuts; i++){ l9[i] = false; }


      // Setup the Cuts for this Event

    setCuts();


      // Loop over all Cut Combinations and Fill Histograms

    for (int i1 = 0; i1 <= n1cuts; i1++){ if (l1[i1]){
    for (int i2 = 0; i2 <= n2cuts; i2++){ if (l2[i2]){
    for (int i3 = 0; i3 <= n3cuts; i3++){ if (l3[i3]){
    for (int i4 = 0; i4 <= n4cuts; i4++){ if (l4[i4]){
    for (int i5 = 0; i5 <= n5cuts; i5++){ if (l5[i5]){
    for (int i6 = 0; i6 <= n6cuts; i6++){ if (l6[i6]){
    for (int i7 = 0; i7 <= n7cuts; i7++){ if (l7[i7]){
    for (int i8 = 0; i8 <= n8cuts; i8++){ if (l8[i8]){
    for (int i9 = 0; i9 <= n9cuts; i9++){ if (l9[i9]){
      TString key("");key+=i9;key+=i8;key+=i7;key+=i6;key+=i5;key+=i4;key+=i3;key+=i2;key+=i1;
      fillHistograms(key);
    }}}}}}}}}}}}}}}}}}

  }

  cout << "FSHistogramAnalysis: Writing Histograms..." << endl;

  writeHistograms(outputfile);

  cout << "FSHistogramAnalysis: Done." << endl;

}


void 
FSHistogramAnalysis::writeHistograms(string filename){
  TFile* outfile = new TFile(filename.c_str(),"recreate");
  for (map<TString,TH1F*>::iterator mapItr = h1map.begin();
       mapItr != h1map.end(); mapItr++){
    if (mapItr->second){
      TH1F* hist = FSHistogram::getTH1F(mapItr->second);
      outfile->cd();
      hist->Write();
    }
  }
  for (map<TString,TH2F*>::iterator mapItr = h2map.begin();
       mapItr != h2map.end(); mapItr++){
    if (mapItr->second){
      TH2F* hist = FSHistogram::getTH2F(mapItr->second);
      outfile->cd();
      hist->Write();
    }
  }
}


int
FSHistogramAnalysis::cutFromKey(int cutNumber, TString key){
  if ((key.Length() < 9) || (cutNumber < 1) || (cutNumber > 10)){ 
    cout << "FSHistogramAnalysis ERROR: bad key: " << key << " or cutNumber: " << cutNumber << endl; 
    return -1;
  }
  TString digit("");
       if (cutNumber == 1){ for (int i = 8; i < key.Length(); i++){ digit += key[i]; } }
  else if (cutNumber == 2){ digit += key[7]; }
  else if (cutNumber == 3){ digit += key[6]; }
  else if (cutNumber == 4){ digit += key[5]; }
  else if (cutNumber == 5){ digit += key[4]; }
  else if (cutNumber == 6){ digit += key[3]; }
  else if (cutNumber == 7){ digit += key[2]; }
  else if (cutNumber == 8){ digit += key[1]; }
  else if (cutNumber == 9){ digit += key[0]; }
  return FSString::TString2int(digit);
}

