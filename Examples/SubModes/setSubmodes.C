#include <map>
#include <vector>
#include <iostream>
#include "TString.h"
#include "FSBasic/FSString.h"
#include "FSMode/FSModeInfo.h"

void setSubmodes(){

  vector<TString> D0Decays;
  D0Decays.push_back("[K-2],[pi+2]");
  D0Decays.push_back("[K-2],[pi+2],[pi02]");
  D0Decays.push_back("[K-2],[pi+2],[pi+3],[pi-2]");
  D0Decays.push_back("[K-2],[pi+2],[pi+3],[pi-2],[pi02]");
  D0Decays.push_back("[Ks2],[pi02]");
  D0Decays.push_back("[Ks2],[pi+2],[pi-2]");
  D0Decays.push_back("[Ks2],[pi+2],[pi-2],[pi02]");
  FSModeInfo::setSubmode("[D0]",D0Decays);

  vector<TString> D0barDecays;
  D0barDecays.push_back("[K+6],[pi-6]");
  D0barDecays.push_back("[K+6],[pi-6],[pi06]");
  D0barDecays.push_back("[K+6],[pi-6],[pi-7],[pi+6]");
  D0barDecays.push_back("[K+6],[pi-6],[pi-7],[pi+6],[pi06]");
  D0barDecays.push_back("[Ks6],[pi06]");
  D0barDecays.push_back("[Ks6],[pi-6],[pi+6]");
  D0barDecays.push_back("[Ks6],[pi-6],[pi+6],[pi06]");
  FSModeInfo::setSubmode("[D0bar]",D0barDecays);

  vector<TString> DpDecays;
  DpDecays.push_back("[K-2],[pi+2],[pi+3]");
  DpDecays.push_back("[K-2],[pi+2],[pi+3],[pi02]");
  DpDecays.push_back("[K-2],[pi+2],[pi+3],[pi+4],[pi-2]");
  DpDecays.push_back("[Ks2],[pi+2]");
  DpDecays.push_back("[Ks2],[pi+2],[pi02]");
  DpDecays.push_back("[Ks2],[pi+2],[pi+3],[pi-2]");
  FSModeInfo::setSubmode("[D+]",DpDecays);

  vector<TString> DmDecays;
  DmDecays.push_back("[K+6],[pi-6],[pi-7]");
  DmDecays.push_back("[K+6],[pi-6],[pi-7],[pi06]");
  DmDecays.push_back("[K+6],[pi-6],[pi-7],[pi-8],[pi+6]");
  DmDecays.push_back("[Ks6],[pi-6]");
  DmDecays.push_back("[Ks6],[pi-6],[pi06]");
  DmDecays.push_back("[Ks6],[pi-6],[pi-7],[pi+6]");
  FSModeInfo::setSubmode("[D-]",DmDecays);

  vector<TString> Dstar0Decays;
  Dstar0Decays.push_back("[D0],[pi05]");
  Dstar0Decays.push_back("[D0],[gamma5]");
  FSModeInfo::setSubmode("[D*0]",Dstar0Decays);

  vector<TString> Dstar0barDecays;
  Dstar0barDecays.push_back("[D0bar],[pi09]");
  Dstar0barDecays.push_back("[D0bar],[gamma9]");
  FSModeInfo::setSubmode("[D*0bar]",Dstar0barDecays);

  vector<TString> DstarpDecays;
  DstarpDecays.push_back("[D0],[pi+5]");
  DstarpDecays.push_back("[D+],[pi05]");
  FSModeInfo::setSubmode("[D*+]",DstarpDecays);

  vector<TString> DstarmDecays;
  DstarmDecays.push_back("[D0bar],[pi-9]");
  DstarmDecays.push_back("[D-],[pi09]");
  FSModeInfo::setSubmode("[D*-]",DstarmDecays);


  FSModeInfo mi("K+ K- pi+ pi- pi0");
  mi.modeCombinatorics("MASS([D0]):MASS([D0bar])",true);
  mi.modeCombinatorics("MASS([D*0]):MASS([D0bar])",true);
  mi.modeCombinatorics("MASS([D0]):MASS([D0bar]) MASS([D0]):MASS([D0bar])",true);
  mi.modeCombinatorics("MASS([D*0]):MASS([D0bar]) MASS([D0])",true);

}
