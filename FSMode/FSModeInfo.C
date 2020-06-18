#include <iostream>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <map>
#include "FSBasic/FSControl.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSTree.h"
#include "FSMode/FSModeInfo.h"

// definitions of static variables
map < TString, vector<TString> > FSModeInfo::m_submodeMap;


      // *******************************************************
      // CONSTRUCTORS STARTING FROM:
      //   "modeCode", "modeCode1,modeCode2", "modeString", 
      //     or "modeDescription"
      // *******************************************************


FSModeInfo::FSModeInfo(pair<int,int> mCode){
  m_modeCode = mCode;
  addStandardCategories();
}


FSModeInfo::FSModeInfo(int mCode1, int mCode2){
  m_modeCode = pair<int,int>(mCode1,mCode2);
  addStandardCategories();
}


FSModeInfo::FSModeInfo(TString mString){
  vector<TString> parts = FSString::parseTString(mString);
  vector<TString> extraCategories;
  int code1 = 0;
  int code2 = 0;
  for (unsigned int i = 0; i < parts.size(); i++){
         if (parts[i] == "pi0")    code1 += 1;
    else if (parts[i] == "pi-")    code1 += 10;
    else if (parts[i] == "pi+")    code1 += 100;
    else if (parts[i] == "Ks")     code1 += 1000;
    else if (parts[i] == "K-")     code1 += 10000;
    else if (parts[i] == "K+")     code1 += 100000;
    else if (parts[i] == "gamma")  code1 += 1000000;
    else if (parts[i] == "eta")    code2 += 1;
    else if (parts[i] == "p-")     code2 += 10;
    else if (parts[i] == "p+")     code2 += 100;
    else if (parts[i] == "mu-")    code2 += 1000;
    else if (parts[i] == "mu+")    code2 += 10000;
    else if (parts[i] == "e-")     code2 += 100000;
    else if (parts[i] == "e+")     code2 += 1000000;
    else if (parts[i] == "ALambda")code2 += 10000000;
    else if (parts[i] == "Lambda") code2 += 100000000;
    else if (isModeString(parts[i])){
      vector<TString> codes = FSString::parseTString(parts[i],"_");
      code1 += FSString::TString2int(codes[codes.size()-1]);
      TString scode2(codes[codes.size()-2]);
      int ipow = 1;
      for (unsigned int dig = scode2.Length(); dig > 0; dig--){
        TString sdig(scode2[dig-1]);
        if (sdig == "0" || sdig == "1" || sdig == "2" || sdig == "3" || sdig == "4" 
         || sdig == "0" || sdig == "6" || sdig == "7" || sdig == "8" || sdig == "9"){
          code2 += FSString::TString2int(sdig)*ipow;  ipow *= 10;
        }
        else{
          break;
        }
      }
    }
    else{
      TString cat = parts[i];
      extraCategories.push_back(cat);
    }
  }
  m_modeCode = pair<int,int>(code1,code2);
  addStandardCategories();
  addCategory(extraCategories);
}


      // *******************************************************
      // RETURN INFORMATION ABOUT THIS MODE
      //  modeString recognizes and replaces these strings 
      //    if "original" is given:
      //      "MODESTRING"
      //      "MODEDESCRIPTION"
      //      "MODECODE"
      //      "MODECODE1"
      //      "MODECODE2"
      //      "MODEGLUEX"
      //    if "counter" is also given and non-negative:
      //      "MODECOUNTER"
      //        or "MODECOUNTERXXXX" to pad with zeros
      // *******************************************************


pair<int,int>
FSModeInfo::modeCode(){
  return m_modeCode;
}

int
FSModeInfo::modeCode1(){
  return m_modeCode.first;
}

int
FSModeInfo::modeCode2(){
  return m_modeCode.second;
}

TString
FSModeInfo::modeString(TString original, int counter){
  TString mString("");
  mString += m_modeCode.second;
  mString += "_";
  mString += m_modeCode.first;
  if (original == ""){
    return mString;
  }
  while (original.Contains("MODESTRING")){
    original.Replace(original.Index("MODESTRING"),10,mString);
  }
  while (original.Contains("MODEDESCRIPTION")){
    TString stmp("");  
    stmp += modeDescription();
    original.Replace(original.Index("MODEDESCRIPTION"),15,stmp);
  }
  while (original.Contains("MODECODE1")){
    TString stmp("");  
    stmp += m_modeCode.first;
    original.Replace(original.Index("MODECODE1"),9,stmp);
  }
  while (original.Contains("MODECODE2")){
    TString stmp("");  
    stmp += m_modeCode.second;
    original.Replace(original.Index("MODECODE2"),9,stmp);
  }
  while (original.Contains("MODECODE")){
    original.Replace(original.Index("MODECODE"),8,mString);
  }
  while (original.Contains("MODEGLUEX")){
    TString stmp("");  
    stmp += modeGlueXFormat();
    original.Replace(original.Index("MODEGLUEX"),9,stmp);
  }
  while (original.Contains("MODECOMBO")){
    int minimumIndex = 0;
    TString MODECOMBO("MODECOMBO");
    if (original.Index("MODECOMBO")+9 < original.Length()){
      TString digit = original[original.Index("MODECOMBO")+9];
      if (digit.IsDigit()){
        MODECOMBO += digit;
        minimumIndex = FSString::TString2int(digit);
      }
    }
    TString stmp = modeComboFormat(minimumIndex);
    original.Replace(original.Index(MODECOMBO),MODECOMBO.Length(),stmp);
  }
  while (original.Contains("MODECOUNTER") && counter >= 0){
    int digits = 0;
    while (original.Index("MODECOUNTER")+11+digits < original.Length() &&
           (TString) original[original.Index("MODECOUNTER")+11+digits] == "X") digits++;
    if (digits == 1) digits = 0;
    original.Replace(original.Index("MODECOUNTER"),11+digits,
                     FSString::int2TString(counter,digits));
  }
  return original;
}



TString
FSModeInfo::modeDescription(){
  TString description("");
  int np;
  np = modeNGamma     ();  for (int i = 0; i < np; i++){ description += " gamma "; }
  np = modeNEp        ();  for (int i = 0; i < np; i++){ description += " e+ "; }
  np = modeNEm        ();  for (int i = 0; i < np; i++){ description += " e- "; }
  np = modeNMp        ();  for (int i = 0; i < np; i++){ description += " mu+ "; }
  np = modeNMm        ();  for (int i = 0; i < np; i++){ description += " mu- "; }
  np = modeNPp        ();  for (int i = 0; i < np; i++){ description += " p+ "; }
  np = modeNPm        ();  for (int i = 0; i < np; i++){ description += " p- "; }
  np = modeNLambda    ();  for (int i = 0; i < np; i++){ description += " Lambda "; }
  np = modeNALambda   ();  for (int i = 0; i < np; i++){ description += " ALambda "; }
  np = modeNKp        ();  for (int i = 0; i < np; i++){ description += " K+ "; }
  np = modeNKm        ();  for (int i = 0; i < np; i++){ description += " K- "; }
  np = modeNKs        ();  for (int i = 0; i < np; i++){ description += " Ks "; }
  np = modeNPip       ();  for (int i = 0; i < np; i++){ description += " pi+ "; }
  np = modeNPim       ();  for (int i = 0; i < np; i++){ description += " pi- "; }
  np = modeNPi0       ();  for (int i = 0; i < np; i++){ description += " pi0 "; }
  np = modeNEta       ();  for (int i = 0; i < np; i++){ description += " eta "; }
  return description;
}

TString
FSModeInfo::mcExtrasDescription(TString mcExtras){
  TString description("");
  int imcExtras = FSString::TString2int(mcExtras);
  if (imcExtras <= 0) return description;
  if (imcExtras > 9999) return description;
  int n1 = 1; int n2 = 10;
  for (unsigned int iP = 0; iP < 4; iP++){
    int np = (((imcExtras%n2)-(imcExtras%n1))/n1); n1*=10; n2*=10;
    for (int i = 0; i < np; i++){
      if (iP == 3) description += " nu ";
      if (iP == 2) description += " Kl ";
      if (iP == 1) description += " n ";
      if (iP == 0) description += " nbar ";
    }
  }
  return description;
}


TString
FSModeInfo::modeGlueXFormat(){
  TString gluexFormat("");
  int np;
  np = modeNGamma     ();  for (int i = 0; i < np; i++){ gluexFormat += "_1"; }
  np = modeNEp        ();  for (int i = 0; i < np; i++){ gluexFormat += "_2"; }
  np = modeNEm        ();  for (int i = 0; i < np; i++){ gluexFormat += "_3"; }
  np = modeNMp        ();  if (np != 0){ cout << "No muons in modeGlueXFormat yet" << endl; }
  np = modeNMm        ();  if (np != 0){ cout << "No muons in modeGlueXFormat yet" << endl; }
  np = modeNPi0       ();  for (int i = 0; i < np; i++){ gluexFormat += "_7"; }
  np = modeNPip       ();  for (int i = 0; i < np; i++){ gluexFormat += "_8"; }
  np = modeNPim       ();  for (int i = 0; i < np; i++){ gluexFormat += "_9"; }
  np = modeNKp        ();  for (int i = 0; i < np; i++){ gluexFormat += "_11"; }
  np = modeNKm        ();  for (int i = 0; i < np; i++){ gluexFormat += "_12"; }
  np = modeNKs        ();  for (int i = 0; i < np; i++){ gluexFormat += "_16"; }
  np = modeNEta       ();  for (int i = 0; i < np; i++){ gluexFormat += "_17"; }
  np = modeNPp        ();  for (int i = 0; i < np; i++){ gluexFormat += "_14"; }
  np = modeNPm        ();  for (int i = 0; i < np; i++){ gluexFormat += "_15"; }
  np = modeNLambda    ();  for (int i = 0; i < np; i++){ gluexFormat += "_18"; }
  np = modeNALambda   ();  for (int i = 0; i < np; i++){ gluexFormat += "_26"; }
  return gluexFormat;
}

TString
FSModeInfo::modeComboFormat(int min, TString tag){
  TString cF("");
  int np;
  if (tag != "") tag = "_" + tag + "_";  
  TString ind[10]; for (int i = 0; i < 10; i++){ind[i] = tag + FSString::int2TString(i+min);}
  np = modeNGamma  ();  for (int i = 0; i < np; i++){ cF += "[gamma"  ; cF += ind[i] + "],"; }
  np = modeNEp     ();  for (int i = 0; i < np; i++){ cF += "[e+"     ; cF += ind[i] + "],"; }
  np = modeNEm     ();  for (int i = 0; i < np; i++){ cF += "[e-"     ; cF += ind[i] + "],"; }
  np = modeNMp     ();  for (int i = 0; i < np; i++){ cF += "[mu+"    ; cF += ind[i] + "],"; }
  np = modeNMm     ();  for (int i = 0; i < np; i++){ cF += "[mu-"    ; cF += ind[i] + "],"; }
  np = modeNPi0    ();  for (int i = 0; i < np; i++){ cF += "[pi0"    ; cF += ind[i] + "],"; }
  np = modeNPip    ();  for (int i = 0; i < np; i++){ cF += "[pi+"    ; cF += ind[i] + "],"; }
  np = modeNPim    ();  for (int i = 0; i < np; i++){ cF += "[pi-"    ; cF += ind[i] + "],"; }
  np = modeNKp     ();  for (int i = 0; i < np; i++){ cF += "[K+"     ; cF += ind[i] + "],"; }
  np = modeNKm     ();  for (int i = 0; i < np; i++){ cF += "[K-"     ; cF += ind[i] + "],"; }
  np = modeNKs     ();  for (int i = 0; i < np; i++){ cF += "[Ks"     ; cF += ind[i] + "],"; }
  np = modeNEta    ();  for (int i = 0; i < np; i++){ cF += "[eta"    ; cF += ind[i] + "],"; }
  np = modeNPp     ();  for (int i = 0; i < np; i++){ cF += "[p+"     ; cF += ind[i] + "],"; }
  np = modeNPm     ();  for (int i = 0; i < np; i++){ cF += "[p-"     ; cF += ind[i] + "],"; }
  np = modeNLambda ();  for (int i = 0; i < np; i++){ cF += "[Lambda" ; cF += ind[i] + "],"; }
  np = modeNALambda();  for (int i = 0; i < np; i++){ cF += "[ALambda"; cF += ind[i] + "],"; }
  if (cF.Length() != 0) cF = FSString::subString(cF,0,cF.Length()-1);
  return cF;
}


vector<TString> 
FSModeInfo::particles(){ 
  if (m_particles.size() != 0) return m_particles;
  int np;
  np = modeNLambda    ();  for (int i = 0; i < np; i++){ m_particles.push_back("Lambda"); }
  np = modeNALambda   ();  for (int i = 0; i < np; i++){ m_particles.push_back("ALambda"); }
  np = modeNEp        ();  for (int i = 0; i < np; i++){ m_particles.push_back("e+"); }
  np = modeNEm        ();  for (int i = 0; i < np; i++){ m_particles.push_back("e-"); }
  np = modeNMp        ();  for (int i = 0; i < np; i++){ m_particles.push_back("mu+"); }
  np = modeNMm        ();  for (int i = 0; i < np; i++){ m_particles.push_back("mu-"); }
  np = modeNPp        ();  for (int i = 0; i < np; i++){ m_particles.push_back("p+"); }
  np = modeNPm        ();  for (int i = 0; i < np; i++){ m_particles.push_back("p-"); }
  np = modeNEta       ();  for (int i = 0; i < np; i++){ m_particles.push_back("eta"); }
  np = modeNGamma     ();  for (int i = 0; i < np; i++){ m_particles.push_back("gamma"); }
  np = modeNKp        ();  for (int i = 0; i < np; i++){ m_particles.push_back("K+"); }
  np = modeNKm        ();  for (int i = 0; i < np; i++){ m_particles.push_back("K-"); }
  np = modeNKs        ();  for (int i = 0; i < np; i++){ m_particles.push_back("Ks"); }
  np = modeNPip       ();  for (int i = 0; i < np; i++){ m_particles.push_back("pi+"); }
  np = modeNPim       ();  for (int i = 0; i < np; i++){ m_particles.push_back("pi-"); }
  np = modeNPi0       ();  for (int i = 0; i < np; i++){ m_particles.push_back("pi0"); }
  return m_particles;
}


  // *******************************************************
  // GET NUMBERS OF EACH PARTICLE TYPE
  // *******************************************************


int FSModeInfo::modeNPi0       (){return   (m_modeCode.first%10); }
int FSModeInfo::modeNPim       (){return (((m_modeCode.first%100)-
                                         (m_modeCode.first%10))/10); }
int FSModeInfo::modeNPip       (){return (((m_modeCode.first%1000)-
                                         (m_modeCode.first%100))/100); }
int FSModeInfo::modeNKs        (){return (((m_modeCode.first%10000)-
                                         (m_modeCode.first%1000))/1000); }
int FSModeInfo::modeNKm        (){return (((m_modeCode.first%100000)-
                                         (m_modeCode.first%10000))/10000); }
int FSModeInfo::modeNKp        (){return (((m_modeCode.first%1000000)-
                                         (m_modeCode.first%100000))/100000); }
int FSModeInfo::modeNGamma     (){return (((m_modeCode.first%10000000)-
                                         (m_modeCode.first%1000000))/1000000); }
int FSModeInfo::modeNEta       (){return   (m_modeCode.second%10); }
int FSModeInfo::modeNPm        (){return (((m_modeCode.second%100)-
                                         (m_modeCode.second%10))/10); }
int FSModeInfo::modeNPp        (){return (((m_modeCode.second%1000)-
                                         (m_modeCode.second%100))/100); }
int FSModeInfo::modeNMm        (){return (((m_modeCode.second%10000)-
                                         (m_modeCode.second%1000))/1000); }
int FSModeInfo::modeNMp        (){return (((m_modeCode.second%100000)-
                                         (m_modeCode.second%10000))/10000); }
int FSModeInfo::modeNEm        (){return (((m_modeCode.second%1000000)-
                                         (m_modeCode.second%100000))/100000); }
int FSModeInfo::modeNEp        (){return (((m_modeCode.second%10000000)-
                                         (m_modeCode.second%1000000))/1000000); }
int FSModeInfo::modeNALambda   (){return (((m_modeCode.second%100000000)-
                                         (m_modeCode.second%10000000))/10000000); }
int FSModeInfo::modeNLambda    (){return (((m_modeCode.second%1000000000)-
                                         (m_modeCode.second%100000000))/100000000); }

int FSModeInfo::modeNParticles (){return ( 1*modeNPi0       () +
                                         1*modeNPim       () +
                                         1*modeNPip       () +
                                         1*modeNKs        () +
                                         1*modeNKm        () +
                                         1*modeNKp        () +
                                         1*modeNGamma     () +
                                         1*modeNEta       () +
                                         1*modeNPm        () +
                                         1*modeNPp        () +
                                         1*modeNMm        () +
                                         1*modeNMp        () +
                                         1*modeNEm        () +
                                         1*modeNEp        () +
                                         1*modeNALambda   () +
                                         1*modeNLambda    ()); }

int FSModeInfo::modeCharge     (){return ( 0*modeNPi0       () +
                                        -1*modeNPim       () +
                                         1*modeNPip       () +
                                         0*modeNKs        () +
                                        -1*modeNKm        () +
                                         1*modeNKp        () +
                                         0*modeNGamma     () +
                                         0*modeNEta       () +
                                        -1*modeNPm        () +
                                         1*modeNPp        () +
                                        -1*modeNMm        () +
                                         1*modeNMp        () +
                                        -1*modeNEm        () +
                                         1*modeNEp        () +
                                         0*modeNALambda   () +
                                         0*modeNLambda    ()); }

int FSModeInfo::modeBaryonNumber(){return (-1*modeNALambda  () +
                                          1*modeNLambda   () +
                                         -1*modeNPm       () +
                                          1*modeNPp       ()); }

int FSModeInfo::modeStrangeness (){
  int S = -1*(modeNKp()+modeNALambda()) + 1*(modeNKm()+modeNLambda());
  for (int i = 0; i < modeNKs(); i++){
    if      (S <= 0) S++;
    else if (S >  0) S--;
  }
  return S;
}

double FSModeInfo::modeMass   (){return ( 0.1350*modeNPi0       () +
                                        0.1396*modeNPim       () +
                                        0.1396*modeNPip       () +
                                        0.4976*modeNKs        () +
                                        0.4937*modeNKm        () +
                                        0.4937*modeNKp        () +
                                        0.0000*modeNGamma     () +
                                        0.5479*modeNEta       () +
                                        0.9383*modeNPm        () +
                                        0.9383*modeNPp        () +
                                        0.1057*modeNMm        () +
                                        0.1057*modeNMp        () +
                                        0.0005*modeNEm        () +
                                        0.0005*modeNEp        () +
                                        1.1157*modeNALambda   () +
                                        1.1157*modeNLambda    ()); }


FSModeInfo FSModeInfo::modeCC(){
  TString cc("");
  int np;
  np = modeNGamma     ();  for (int i = 0; i < np; i++){ cc += " gamma "; }
  np = modeNEp        ();  for (int i = 0; i < np; i++){ cc += " e- "; }
  np = modeNEm        ();  for (int i = 0; i < np; i++){ cc += " e+ "; }
  np = modeNMp        ();  for (int i = 0; i < np; i++){ cc += " mu- "; }
  np = modeNMm        ();  for (int i = 0; i < np; i++){ cc += " mu+ "; }
  np = modeNPp        ();  for (int i = 0; i < np; i++){ cc += " p- "; }
  np = modeNPm        ();  for (int i = 0; i < np; i++){ cc += " p+ "; }
  np = modeNLambda    ();  for (int i = 0; i < np; i++){ cc += " ALambda "; }
  np = modeNALambda   ();  for (int i = 0; i < np; i++){ cc += " Lambda "; }
  np = modeNKp        ();  for (int i = 0; i < np; i++){ cc += " K- "; }
  np = modeNKm        ();  for (int i = 0; i < np; i++){ cc += " K+ "; }
  np = modeNKs        ();  for (int i = 0; i < np; i++){ cc += " Ks "; }
  np = modeNPip       ();  for (int i = 0; i < np; i++){ cc += " pi- "; }
  np = modeNPim       ();  for (int i = 0; i < np; i++){ cc += " pi+ "; }
  np = modeNPi0       ();  for (int i = 0; i < np; i++){ cc += " pi0 "; }
  np = modeNEta       ();  for (int i = 0; i < np; i++){ cc += " eta "; }
  return FSModeInfo(cc);
}


int FSModeInfo::modeCCNumber(){
  FSModeInfo micc = modeCC();
  if ((modeCode1() == micc.modeCode1()) &&
      (modeCode2() == micc.modeCode2())) return 0;
  if  (modeCode2()  > micc.modeCode2())  return 1;
  if  (modeCode2()  < micc.modeCode2())  return -1;
  if  (modeCode1()  > micc.modeCode1())  return 1;
  if  (modeCode1()  < micc.modeCode1())  return -1;
  return -2;
}


bool FSModeInfo::modeContains(TString mString){
  FSModeInfo mitest(mString);
  if (mitest.modeNGamma()   > modeNGamma())   return false;
  if (mitest.modeNEp()      > modeNEp())      return false;
  if (mitest.modeNEm()      > modeNEm())      return false;
  if (mitest.modeNMp()      > modeNMp())      return false;
  if (mitest.modeNMm()      > modeNMm())      return false;
  if (mitest.modeNPp()      > modeNPp())      return false;
  if (mitest.modeNPm()      > modeNPm())      return false;
  if (mitest.modeNLambda()  > modeNLambda())  return false;
  if (mitest.modeNALambda() > modeNALambda()) return false;
  if (mitest.modeNKp()      > modeNKp())      return false;
  if (mitest.modeNKm()      > modeNKm())      return false;
  if (mitest.modeNKs()      > modeNKs())      return false;
  if (mitest.modeNPip()     > modeNPip())     return false;
  if (mitest.modeNPim()     > modeNPim())     return false;
  if (mitest.modeNPi0()     > modeNPi0())     return false;
  if (mitest.modeNEta()     > modeNEta())     return false;
  return true;
}


  // *******************************************************
  // GET PARTICLE INDICES
  // *******************************************************

vector<int> 
FSModeInfo::modeParticleIndices(TString particleName){
  vector<TString> pv = particles();
  vector<int> indexVector;
  for (unsigned int i = 0; i < pv.size(); i++){
    if ( (particleName == pv[i]) || 
        ((particleName == "e")  && ((pv[i] == "e+")  || (pv[i] == "e-")))  ||
        ((particleName == "mu") && ((pv[i] == "mu+") || (pv[i] == "mu-"))) ||
        ((particleName == "pi") && ((pv[i] == "pi+") || (pv[i] == "pi-"))) ||
        ((particleName == "K")  && ((pv[i] == "K+")  || (pv[i] == "K-")))  ||
        ((particleName == "p")  && ((pv[i] == "p+")  || (pv[i] == "p-")))  ||
        ((particleName == "tk") && ((pv[i] == "e+")  || (pv[i] == "e-") ||
                                    (pv[i] == "mu+") || (pv[i] == "mu-")||
                                    (pv[i] == "pi+") || (pv[i] == "pi-")||
                                    (pv[i] == "K+")  || (pv[i] == "K-") ||
                                    (pv[i] == "p+")  || (pv[i] == "p-"))) ||
        ((particleName == "tk+")&& ((pv[i] == "e+")  ||
                                    (pv[i] == "mu+") ||
                                    (pv[i] == "pi+") ||
                                    (pv[i] == "K+")  ||
                                    (pv[i] == "p+"))) ||
        ((particleName == "tk-")&& ((pv[i] == "e-")  ||
                                    (pv[i] == "mu-") ||
                                    (pv[i] == "pi-") ||
                                    (pv[i] == "K-")  ||
                                    (pv[i] == "p-"))) ||
        ((particleName == "l")  && ((pv[i] == "e+")  || (pv[i] == "e-") ||
                                    (pv[i] == "mu+") || (pv[i] == "mu-"))) ||
        ((particleName == "l+") && ((pv[i] == "e+")  ||
                                    (pv[i] == "mu+"))) ||
        ((particleName == "l-") && ((pv[i] == "e-")  ||
                                    (pv[i] == "mu-"))) ||
         (particleName == "")) indexVector.push_back(i+1);
  }
  return indexVector;
}

vector<int> FSModeInfo::modePi0Indices     (){return modeParticleIndices ("pi0");}   
vector<int> FSModeInfo::modePimIndices     (){return modeParticleIndices ("pi-");}
vector<int> FSModeInfo::modePipIndices     (){return modeParticleIndices ("pi+");}
vector<int> FSModeInfo::modePiIndices      (){return modeParticleIndices ("pi");}
vector<int> FSModeInfo::modeKsIndices      (){return modeParticleIndices ("Ks");}
vector<int> FSModeInfo::modeKmIndices      (){return modeParticleIndices ("K-");}
vector<int> FSModeInfo::modeKpIndices      (){return modeParticleIndices ("K+");}
vector<int> FSModeInfo::modeKIndices       (){return modeParticleIndices ("K");}
vector<int> FSModeInfo::modeGammaIndices   (){return modeParticleIndices ("gamma");}
vector<int> FSModeInfo::modeEtaIndices     (){return modeParticleIndices ("eta");}
vector<int> FSModeInfo::modePmIndices      (){return modeParticleIndices ("p-");}
vector<int> FSModeInfo::modePpIndices      (){return modeParticleIndices ("p+");}
vector<int> FSModeInfo::modePIndices       (){return modeParticleIndices ("p");}
vector<int> FSModeInfo::modeMmIndices      (){return modeParticleIndices ("mu-");}
vector<int> FSModeInfo::modeMpIndices      (){return modeParticleIndices ("mu+");}
vector<int> FSModeInfo::modeMIndices       (){return modeParticleIndices ("mu");}
vector<int> FSModeInfo::modeEmIndices      (){return modeParticleIndices ("e-");}
vector<int> FSModeInfo::modeEpIndices      (){return modeParticleIndices ("e+");}
vector<int> FSModeInfo::modeEIndices       (){return modeParticleIndices ("e");}
vector<int> FSModeInfo::modeTkmIndices     (){return modeParticleIndices ("tk-");}
vector<int> FSModeInfo::modeTkpIndices     (){return modeParticleIndices ("tk+");}
vector<int> FSModeInfo::modeTkIndices      (){return modeParticleIndices ("tk");}
vector<int> FSModeInfo::modeLmIndices      (){return modeParticleIndices ("l-");}
vector<int> FSModeInfo::modeLpIndices      (){return modeParticleIndices ("l+");}
vector<int> FSModeInfo::modeLIndices       (){return modeParticleIndices ("l");}
vector<int> FSModeInfo::modeALambdaIndices (){return modeParticleIndices ("ALambda");}
vector<int> FSModeInfo::modeLambdaIndices  (){return modeParticleIndices ("Lambda");}




  // *************************************************************
  // USEFUL FOR MAKING CUTS ON INDIVIDIUAL PARTICLE TYPES
  // *************************************************************

//  examples:  AND(EnP[pi+]>0) --> ((EnP1>0)&&(EnP2>0)&&(EnP3>0))
//              OR(EnP[pi+]>0) --> ((EnP1>0)||(EnP2>0)||(EnP3>0))
//   (also allows nested ANDs and ORs although it may not be useful)

TString 
FSModeInfo::modeCuts(TString varString){
  if (!FSString::checkParentheses(varString)) return TString("ERROR");
  TString ANDMARK("AND(");
  TString ORMARK("OR(");
  TString newString(varString);
  while (newString.Contains(ANDMARK) || newString.Contains(ORMARK)){
    int iDeepest = 0;
    int pDeepest = 0;
    TString markDeepest = "";
    int pcount = 0;
    for (int i = 0; i < newString.Length(); i++){
      if ((TString)newString[i] == "(") pcount++;
      if ((TString)newString[i] == ")") pcount--;
      if ((i >= 3) && ((TString)newString[i-3] == "A")
                   && ((TString)newString[i-2] == "N")
                   && ((TString)newString[i-1] == "D")
                   && ((TString)newString[i-0] == "(")){
        if (pcount > pDeepest){
          iDeepest = i;
          pDeepest = pcount;
          markDeepest = ANDMARK;
        }
      }
      if ((i >= 2) && ((TString)newString[i-2] == "O")
                   && ((TString)newString[i-1] == "R")
                   && ((TString)newString[i-0] == "(")){
        if (pcount > pDeepest){
          iDeepest = i;
          pDeepest = pcount;
          markDeepest = ORMARK;
        }
      }
    }
    TString encString = "";
    pcount = 0;
    for (int i = iDeepest; i < newString.Length(); i++){
      if ((TString)newString[i] == "(") pcount++;
      if ((TString)newString[i] == ")") pcount--;
      encString += (TString)newString[i];
      if (pcount == 0) break;
    }
    TString newEncString("(");
    vector<TString> combos = modeCombinatorics(encString);
    for (unsigned int j = 0; j < combos.size(); j++){
      newEncString += combos[j];
      if (j != combos.size() - 1){
        if (markDeepest == ANDMARK) newEncString += "&&";
        if (markDeepest == ORMARK) newEncString += "||";
      }
    }
    if (combos.size() == 0) newEncString += "1==1";
    newEncString += ")";
    newString.Replace(iDeepest - markDeepest.Length() + 1,
                      encString.Length()+markDeepest.Length()-1,
                      newEncString);
  }
  return newString;
}



  // *******************************************************
  // CHECK IF THIS IS A PROPER "modeDescription"
  // *******************************************************

bool 
FSModeInfo::isModeDescription(TString mDescription){
  vector<TString> p = FSString::parseTString(mDescription);
  if (p.size() == 0) return false;
  for (unsigned int i = 0; i < p.size(); i++){
    if ((p[i] != "pi0") && 
        (p[i] != "pi-") &&
        (p[i] != "pi+") &&
        (p[i] != "Ks") &&
        (p[i] != "K-") &&
        (p[i] != "K+") &&
        (p[i] != "gamma") &&
        (p[i] != "eta") &&
        (p[i] != "p-") &&
        (p[i] != "p+") &&
        (p[i] != "mu-") &&
        (p[i] != "mu+") &&
        (p[i] != "e-") &&
        (p[i] != "e+") &&
        (p[i] != "ALambda") &&
        (p[i] != "Lambda")) return false;
  }
  return true;
}



  // *******************************************************
  // CHECK IF THIS IS A PROPER "modeString"
  // *******************************************************

bool 
FSModeInfo::isModeString(TString mString){
  vector<TString> parts = FSString::parseTString(mString);
  if (parts.size() != 1) return false;
  vector<TString> codes = FSString::parseTString(mString,"_");
  if (codes.size() < 2) return false;
  if (codes[codes.size()-2].Length() == 0) return false;
  if (codes[codes.size()-1].Length() == 0) return false;
  if (!codes[codes.size()-1].IsDigit()) return false;
  TString last(codes[codes.size()-2][codes[codes.size()-2].Length()-1]);
  if (!last.IsDigit()) return false;
  return true;
}



  // *************************************************************
  // DEFINE SUBMODES FOR MORE COMPLICATED COMBINATORICS
  // *************************************************************

void 
FSModeInfo::setSubmode (TString particleName,
                      vector<TString> particleDecays){
  m_submodeMap[particleName] = particleDecays;
}



  // *************************************************************
  // HANDLES COMBINATORICS
  // *************************************************************


vector<TString>
FSModeInfo::modeCombinatorics(TString varString, bool printCombinatorics, bool removeDuplicates){
  TString varStringOriginal = varString;
  vector<TString> allCombinatorics;


    // check if there is a requirement that varString contains 
    //   all particles in this final state

  bool completeFinalState = false;
  while (varString.Contains("COMPLETE")){
    varString.Replace(varString.Index("COMPLETE"),8,"ENERGY");
    completeFinalState = true;
  }


    // expand all submode names if there are any

  vector<TString> expandedVarStrings = FSString::expandDefinitions(varString,m_submodeMap,"",true);


    // loop over all submode combinations

  for (unsigned int isubmode = 0; isubmode < expandedVarStrings.size(); isubmode++){

    vector< TString > replaceStrings;
    vector< vector<int> > particleLists;
    vector< vector<int> > combinatorics;
    vector< TString > modifiedStrings;

    if (!FSString::checkParentheses(expandedVarStrings[isubmode],"[","]")){
      cout << "FSModeInfo WARNING: possible problem with brackets in " << 
                 expandedVarStrings[isubmode] << endl; 
    }

       // first make lists like this (for example K+ K+ K- K- pi+ pi- pi0 pi0):
       //    particleLists              replaceStrings
       //      1 2                          [K+]
       //      1 2                          [K+2]
       //      7 8                          [pi0]
       //      5                            [pi+]

    vector<TString> spacers;  spacers.push_back("[");  spacers.push_back("]");
    vector<TString> parts = FSString::parseTString(expandedVarStrings[isubmode],spacers,true);
    if (parts.size() < 3){
      allCombinatorics.push_back(expandedVarStrings[isubmode]);
      continue;
    }
    for (unsigned int i = 0; i < parts.size()-2; i++){
      if ((parts[i] == "[") && (parts[i+2] == "]")){
        TString contents = parts[i+1];
        TString replaceString("");  replaceString += ("[" + contents + "]");
        bool found = false;
        for (unsigned int itmp = 0; itmp < replaceStrings.size(); itmp++){
          if (replaceStrings[itmp] == replaceString){ found = true; break; }
        }
        if (found) continue;
               // (careful with the order of these lines...)
             if (contents.Index("ALL")     == 0){ particleLists.push_back(modeParticleIndices()); }
        else if (contents.Index("Lambda")  == 0){ particleLists.push_back(modeLambdaIndices()); }
        else if (contents.Index("ALambda") == 0){ particleLists.push_back(modeALambdaIndices()); }
        else if (contents.Index("eta")     == 0){ particleLists.push_back(modeEtaIndices()); }
        else if (contents.Index("gamma")   == 0){ particleLists.push_back(modeGammaIndices()); }
        else if (contents.Index("tk+")     == 0){ particleLists.push_back(modeTkpIndices()); }
        else if (contents.Index("tk-")     == 0){ particleLists.push_back(modeTkmIndices()); }
        else if (contents.Index("tk")      == 0){ particleLists.push_back(modeTkIndices()); }
        else if (contents.Index("pi+")     == 0){ particleLists.push_back(modePipIndices()); }
        else if (contents.Index("pi-")     == 0){ particleLists.push_back(modePimIndices()); }
        else if (contents.Index("pi0")     == 0){ particleLists.push_back(modePi0Indices()); }
        else if (contents.Index("pi")      == 0){ particleLists.push_back(modePiIndices()); }
        else if (contents.Index("mu+")     == 0){ particleLists.push_back(modeMpIndices()); }
        else if (contents.Index("mu-")     == 0){ particleLists.push_back(modeMmIndices()); }
        else if (contents.Index("mu")      == 0){ particleLists.push_back(modeMIndices()); }
        else if (contents.Index("Ks")      == 0){ particleLists.push_back(modeKsIndices()); }
        else if (contents.Index("K+")      == 0){ particleLists.push_back(modeKpIndices()); }
        else if (contents.Index("K-")      == 0){ particleLists.push_back(modeKmIndices()); }
        else if (contents.Index("K")       == 0){ particleLists.push_back(modeKIndices()); }
        else if (contents.Index("e+")      == 0){ particleLists.push_back(modeEpIndices()); }
        else if (contents.Index("e-")      == 0){ particleLists.push_back(modeEmIndices()); }
        else if (contents.Index("e")       == 0){ particleLists.push_back(modeEIndices()); }
        else if (contents.Index("l+")      == 0){ particleLists.push_back(modeLpIndices()); }
        else if (contents.Index("l-")      == 0){ particleLists.push_back(modeLmIndices()); }
        else if (contents.Index("l")       == 0){ particleLists.push_back(modeLIndices()); }
        else if (contents.Index("p+")      == 0){ particleLists.push_back(modePpIndices()); }
        else if (contents.Index("p-")      == 0){ particleLists.push_back(modePmIndices()); }
        else if (contents.Index("p")       == 0){ particleLists.push_back(modePIndices()); }
        if (particleLists.size() > replaceStrings.size()) replaceStrings.push_back(replaceString);
      }
    }

      // check for a complete final state

    if (completeFinalState && (replaceStrings.size() > 0) && 
         ((int) replaceStrings.size() != modeNParticles())) continue;

      // now recombine particleLists into all possible combinations
      //   from the example above:
      //       combinatorics
      //             1 1 7 5
      //             1 2 7 5
      //             1 1 8 5
      //             1 2 8 5
      //             2 1 7 5
      //             2 2 7 5
      //             2 1 8 5
      //             2 2 8 5

    int ncombos = 1;
    vector<int> decimals;
    for (unsigned int i = 0; i < particleLists.size(); i++){
      decimals.push_back(ncombos);
      ncombos *= particleLists[i].size();
    }

    for (int i = 0; i < ncombos; i++){
      vector<int> combo;
      int icopy = i;
      for (unsigned int j = decimals.size(); j > 0; j--){
	int idiv = icopy/decimals[j-1];
	combo.push_back(particleLists[j-1][idiv]);
	icopy -= idiv*decimals[j-1];
      }
      vector<int> combo2;
      for (unsigned int j = combo.size(); j > 0; j--){
	combo2.push_back(combo[j-1]);
      }
      combinatorics.push_back(combo2);
    }


      // take out combinations with duplicate indices
      //     from the example above
      //       combinatorics
      //             1 2 7 5
      //             1 2 8 5
      //             2 1 7 5
      //             2 1 8 5

    vector< vector<int> > tempCombinatorics2;
    for (unsigned int i = 0; i < combinatorics.size(); i++){
      bool found = false;
      for (unsigned int j = 0; j < combinatorics[i].size(); j++){
	for (unsigned int k = j+1; k < combinatorics[i].size(); k++){
          if (combinatorics[i][j] == combinatorics[i][k]) found = true;
	}
      }
      if (!found) tempCombinatorics2.push_back(combinatorics[i]);
    }
    combinatorics = tempCombinatorics2;


      // make copies of the original string (expandedVarStrings[isubmode])
      //   with all the [particle] markers replaced

    for (unsigned int i = 0; i < combinatorics.size(); i++){
      modifiedStrings.push_back(expandedVarStrings[isubmode]);
      for (unsigned int j = 0; j < combinatorics[i].size(); j++){
	while (modifiedStrings[i].Contains(replaceStrings[j])){
          TString tmpindex("");  tmpindex += combinatorics[i][j];
          modifiedStrings[i].Replace(modifiedStrings[i].Index(replaceStrings[j]),
                                     replaceStrings[j].Length(), tmpindex);
	}
      }
    }

      // if no combinatorics were found, don't add anything to allCombinatorics

    if (modifiedStrings.size() == 0 && replaceStrings.size() > 0) continue;

      // if we didn't try to make combinations, add the original string

    if (modifiedStrings.size() == 0){
      allCombinatorics.push_back(expandedVarStrings[isubmode]);
      continue;
    }

      // otherwise add all the new combinations

    for (unsigned int i = 0; i < modifiedStrings.size(); i++){
      allCombinatorics.push_back(modifiedStrings[i]);
    }

  }

    // order the macro variables (e.g. MASS(3,2) --> MASS(2,3))

  for (unsigned int i = 0; i < allCombinatorics.size(); i++){
    allCombinatorics[i] = FSTree::reorderVariable(allCombinatorics[i]);
  }

    // remove duplicate combinations

  if (removeDuplicates && allCombinatorics.size() > 1){
    vector<TString> copyOriginal = allCombinatorics;
    allCombinatorics.clear();
    for (unsigned int i = 0; i < copyOriginal.size()-1; i++){
      if (copyOriginal[i] != ""){
        for (unsigned int j = i+1; j < copyOriginal.size(); j++){
          if (copyOriginal[j] != ""){
            if (copyOriginal[i] == copyOriginal[j]) copyOriginal[j] = "";
          }
        }
      }
    }
    for (unsigned int i = 0; i < copyOriginal.size(); i++){
      if (copyOriginal[i] != ""){
        allCombinatorics.push_back(copyOriginal[i]);
      }
    }
  }

    // do a final sort (for aesthetics)

  if (allCombinatorics.size() > 1){
    for (unsigned int i = 0; i < allCombinatorics.size()-1; i++){
    for (unsigned int j = i+1; j < allCombinatorics.size(); j++){
      if (FSString::TString2string(allCombinatorics[j]) < 
          FSString::TString2string(allCombinatorics[i])){
        TString temp = allCombinatorics[i];
        allCombinatorics[i] = allCombinatorics[j];
        allCombinatorics[j] = temp;
      }
    }}
  }

    // print the combinatorics to the screen for debugging

  if (printCombinatorics){
    cout << "*******************************" << endl;
    cout << "*** MODE COMBINATORICS TEST ***" << endl;
    cout << "*******************************" << endl;
    cout << "      mode = " << modeString("MODEDESCRIPTION") << endl;
    cout << "     input = " << varStringOriginal << endl;
    cout << "  combinations:" << endl;
    for (unsigned int i = 0; i < allCombinatorics.size(); i++){
      cout << "         (" << i+1 << ") "  << allCombinatorics[i] << endl;
    }
    cout << "*******************************" << endl;
  }

  return allCombinatorics;

}



vector<TString>
FSModeInfo::categories(){
  return m_categories;
}


void 
FSModeInfo::addStandardCategories(){ 

  if (FSControl::DEBUG)
    cout << "FSModeInfo:  adding standard categories to mode " << modeString() << endl;

  int npi0 = modeNPi0();
  int npim = modeNPim();
  int npip = modeNPip();
  int nks  = modeNKs ();
  int nkm  = modeNKm ();
  int nkp  = modeNKp ();
  int ng   = modeNGamma();
  int ne   = modeNEta();
  int npm  = modeNPm ();
  int npp  = modeNPp ();
  int nmm  = modeNMm();
  int nmp  = modeNMp();
  int nem  = modeNEm();
  int nep  = modeNEp();
  int nal  = modeNALambda();
  int nl   = modeNLambda();

  if (nal+nl > 0) m_categories.push_back("HasLambdas");
  if (nem+nep > 0) m_categories.push_back("HasElectrons");
  if (nmm+nmp > 0) m_categories.push_back("HasMuons");
  if (ng > 0) m_categories.push_back("Radiative");
  if ((nem+nep+nmm+nmp+ng) == 0) m_categories.push_back("Hadronic");
  if ((ne+npi0+ng) > 0) m_categories.push_back("HasGammas");
  if ((npp+npm) > 0) m_categories.push_back("HasProtons");
  if (ne > 0) m_categories.push_back("HasEtas");
  if ((nkp+nkm+nks) > 0) m_categories.push_back("HasKaons");
  if (nks > 0) m_categories.push_back("HasKShorts");
  if (nks > 1) m_categories.push_back("Has2KShorts");
  if ((npip+npim+npi0) > 0) m_categories.push_back("HasPions");
  if (npi0 > 0) m_categories.push_back("HasPi0s");
  if (npi0 > 1) m_categories.push_back("Has2Pi0s");

  if ((modeCode2()==0)&&((ng+nkp+nkm+nks)==0)&&((npip+npim+npi0)>0))  m_categories.push_back("AllPi");
  if ((modeCode2()==0)&&((ng+nkp+nkm+nks+npi0)==0)&&((npip+npim)>0))  m_categories.push_back("AllChargedPi");

  TString sfs("");  
  TString sl("L");
  TString snee("E");    TString snmm("M");     TString sng("G");  
  TString snp("P");     TString sneta("Eta");  TString snk("K");   
  TString snks("Ks");   TString snpi("Pi");    TString snpi0("Pi0");  
  if ((nl+nal) != 0)   { TString sn("");  sn += (nl+nal);    sfs += (sn + sl); }
  if ((nep+nem) != 0)  { TString sn("");  sn += (nep+nem);   sfs += (sn + snee); }
  if ((nmp+nmm) != 0)  { TString sn("");  sn += (nmp+nmm);   sfs += (sn + snmm); }
  if (ng != 0)         { TString sn("");  sn += ng;          sfs += (sn + sng); }
  if ((npp+npm) != 0)  { TString sn("");  sn += (npp+npm);   sfs += (sn + snp); }
  if (ne != 0)         { TString sn("");  sn += ne;          sfs += (sn + sneta); }
  if ((nkp+nkm) != 0)  { TString sn("");  sn += (nkp+nkm);   sfs += (sn + snk); }
  if ((nks) != 0)      { TString sn("");  sn += (nks);       sfs += (sn + snks); }
  if ((npip+npim) != 0){ TString sn("");  sn += (npip+npim); sfs += (sn + snpi); }
  if ((npi0) != 0)     { TString sn("");  sn += (npi0);      sfs += (sn + snpi0); }
  m_categories.push_back(sfs);

  int nbody = npi0+npim+npip+nks+nkm+nkp+ne+npm+npp+ng+nmm+nmp+nem+nep+nal+nl;
  TString sbody("");  sbody += nbody;  sbody += "Body";
  m_categories.push_back(sbody);

  int ngamma = 2*npi0 + 2*ne + ng;
  TString sgamma("");  sgamma += ngamma;  sgamma += "Gamma";
  m_categories.push_back(sgamma);

  m_categories.push_back(modeString("CODE=MODECODE"));
  m_categories.push_back(modeString("CODE1=MODECODE1"));
  m_categories.push_back(modeString("CODE2=MODECODE2"));

  if (FSControl::DEBUG)
    cout << "FSModeInfo:  done adding standard categories to mode " << modeString() << endl;
  
}


void
FSModeInfo::addCategory(TString category){
  if (!hasCategory(category) && category != "") m_categories.push_back(category);
}

void
FSModeInfo::addCategory(vector<TString> lcategories){
  for (unsigned int i = 0; i < lcategories.size(); i++){
    addCategory(lcategories[i]);
  }
}


bool
FSModeInfo::hasCategory(TString category){
  bool found = false;
  for (unsigned int i = 0; i < m_categories.size(); i++){
    if (m_categories[i] == category) found = true;
  }
  return found;
}

void
FSModeInfo::removeCategory(TString category){
  if (category == "") return;
  for (unsigned int i = 0; i < m_categories.size(); i++){
    if (m_categories[i] == category){
      m_categories.erase(m_categories.begin()+i);
      return;
    }
  }
}


void
FSModeInfo::display(int counter){
  cout << counter;
  TString mString = modeString();
  for (int i = mString.Length(); i < 18; i++){ cout << " "; }
  cout << modeString() << "   " << modeDescription();
  for (unsigned int j = 0; j < m_categories.size(); j++){
    if (j%4 == 0){  cout << endl;  cout << "\t\t\t\t\t";  }
    cout << m_categories[j] << "  ";
  }
  cout << endl << endl;
}

