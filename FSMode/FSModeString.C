#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "TString.h"
#include "FSBasic/FSString.h"
#include "FSMode/FSModeString.h"


  // ********************************************************
  // CONVERT SYMBOLS TO ROOT FORMAT (e.g. "pi+" TO "#pi^{+}")
  // ********************************************************

TString
FSModeString::rootSymbols(TString input){
  while (input.Contains("\\pm")){
    input.Replace(input.Index("\\pm"),3," #pm ");
  }
  while (input.Contains("omega")){
    input.Replace(input.Index("omega"),5,"TMP");
  }
      while (input.Contains("TMP")){
	input.Replace(input.Index("TMP"),3,"#omega");
      }
  while (input.Contains("phi")){
    input.Replace(input.Index("phi"),3,"TMP");
  }
      while (input.Contains("TMP")){
	input.Replace(input.Index("TMP"),3,"#phi");
      }
  while (input.Contains("etaprime")){
    input.Replace(input.Index("etaprime"),8,"#eta'");
  }
  while (input.Contains("mu+mu-")){
    input.Replace(input.Index("mu+mu-"),6,"#mu^{+}#mu^{-}");
  }
  while (input.Contains("mu+")){
    input.Replace(input.Index("mu+"),3,"#mu^{+}");
  }
  while (input.Contains("mu-")){
    input.Replace(input.Index("mu-"),3,"#mu^{-}");
  }
  while (input.Contains("e+e-")){
    input.Replace(input.Index("e+e-"),4,"e^{+}e^{-}");
  }
  while (input.Contains("e+")){
    input.Replace(input.Index("e+"),2,"e^{+}");
  }
  while (input.Contains("e-")){
    input.Replace(input.Index("e-"),2,"e^{-}");
  }
  while (input.Contains("K+")){
    input.Replace(input.Index("K+"),2,"K^{+}");
  }
  while (input.Contains("K-")){
    input.Replace(input.Index("K-"),2,"K^{-}");
  }
  while (input.Contains("K_S0")){
    input.Replace(input.Index("K_S0"),4,"K_{S}");
  }
  while (input.Contains("Ks")){
    input.Replace(input.Index("Ks"),2,"K_{S}");
  }
  while (input.Contains("pi+")){
    input.Replace(input.Index("pi+"),3,"#pi^{+}");
  }
  while (input.Contains("pi-")){
    input.Replace(input.Index("pi-"),3,"#pi^{-}");
  }
  while (input.Contains("pi0")){
    input.Replace(input.Index("pi0"),3,"#pi^{0}");
  }
  while (input.Contains("eta")){
    input.Replace(input.Index("eta"),3,"TMP");
  }
      while (input.Contains("TMP")){
	input.Replace(input.Index("TMP"),3,"#eta");
      }
  while (input.Contains("#eta+-0")){
    input.Replace(input.Index("#eta+-0"),7,"#eta_{+-0}");
  }
  while (input.Contains("#eta000")){
    input.Replace(input.Index("#eta000"),7,"#eta_{000}");
  }
  while (input.Contains("p+")){
    input.Replace(input.Index("p+"),2,"p^{+}");
  }
  while (input.Contains("p-")){
    input.Replace(input.Index("p-"),2,"p^{-}");
  }
  while (input.Contains("gamma")){
    input.Replace(input.Index("gamma"),5,"TMP");
  }
      while (input.Contains("TMP")){
	input.Replace(input.Index("TMP"),3,"#gamma");
      }
  while (input.Contains("ALambda")){
    input.Replace(input.Index("ALambda"),7,"TMP");
  }
      while (input.Contains("TMP")){
	input.Replace(input.Index("TMP"),3,"#Lambda");
      }
  while (input.Contains("Lambda")){
    input.Replace(input.Index("Lambda"),6,"TMP");
  }
      while (input.Contains("TMP")){
	input.Replace(input.Index("TMP"),3,"#Lambda");
      }
  while (input.Contains("##")){
    input.Replace(input.Index("##"),2,"#");
  }
  return input;
}


  // ********************************************************
  // CONVERT SYMBOLS TO LATEX FORMAT (e.g. "pi+" TO "\pi^{+}")
  // ********************************************************

TString
FSModeString::latexSymbols(TString input){
  while (input.Contains("#pi")){
    input.Replace(input.Index("#pi"),3,"\\pi");
  }
  while (input.Contains("pi+")){
    input.Replace(input.Index("pi+"),3,"\\pi^{+}");
  }
  while (input.Contains("pi-")){
    input.Replace(input.Index("pi-"),3,"\\pi^{-}");
  }
  while (input.Contains("pi0")){
    input.Replace(input.Index("pi0"),3,"\\pi^{0}");
  }
  while (input.Contains("K+")){
    input.Replace(input.Index("K+"),2,"K^{+}");
  }
  while (input.Contains("K-")){
    input.Replace(input.Index("K-"),2,"K^{-}");
  }
  while (input.Contains("K_S0")){
    input.Replace(input.Index("K_S0"),4,"K_{S}");
  }
  while (input.Contains("Ks")){
    input.Replace(input.Index("Ks"),2,"K_{S}");
  }
  while (input.Contains("#eta")){
    input.Replace(input.Index("#eta"),4,"TMP");
  }
  while (input.Contains("eta")){
    input.Replace(input.Index("eta"),3,"TMP");
  }
  while (input.Contains("TMP")){
    input.Replace(input.Index("TMP"),3,"\\eta");
  }
  while (input.Contains("mu+mu-")){
    input.Replace(input.Index("mu+mu-"),6,"\\mu^{+}\\mu^{-}");
  }
  while (input.Contains("mu+")){
    input.Replace(input.Index("mu+"),3,"\\mu^{+}");
  }
  while (input.Contains("mu-")){
    input.Replace(input.Index("mu-"),3,"\\mu^{-}");
  }
  while (input.Contains("e+e-")){
    input.Replace(input.Index("e+e-"),4,"e^{+}e^{-}");
  }
  while (input.Contains("e+")){
    input.Replace(input.Index("e+"),2,"e^{+}");
  }
  while (input.Contains("e-")){
    input.Replace(input.Index("e-"),2,"e^{-}");
  }
  while (input.Contains("#omega")){
    input.Replace(input.Index("#omega"),6,"TMP");
  }
  while (input.Contains("omega")){
    input.Replace(input.Index("omega"),5,"TMP");
  }
  while (input.Contains("TMP")){
    input.Replace(input.Index("TMP"),3,"\\omega");
  }
  while (input.Contains("#phi")){
    input.Replace(input.Index("phi"),4,"TMP");
  }
  while (input.Contains("phi")){
    input.Replace(input.Index("phi"),3,"TMP");
  }
  while (input.Contains("TMP")){
    input.Replace(input.Index("TMP"),3,"\\phi");
  }
  while (input.Contains("etaprime")){
    input.Replace(input.Index("etaprime"),8,"\\eta^{\\prime}");
  }
  while (input.Contains("#eta+-0")){
    input.Replace(input.Index("#eta+-0"),7,"\\eta_{+-0}");
  }
  while (input.Contains("#eta000")){
    input.Replace(input.Index("#eta000"),7,"\\eta_{000}");
  }
  while (input.Contains("p+")){
    input.Replace(input.Index("p+"),2,"p^{+}");
  }
  while (input.Contains("p-")){
    input.Replace(input.Index("p-"),2,"p^{-}");
  }
  while (input.Contains("gamma")){
    input.Replace(input.Index("gamma"),5,"TMP");
  }
  while (input.Contains("TMP")){
    input.Replace(input.Index("TMP"),3,"\\gamma");
  }
  while (input.Contains("#psi")){
    input.Replace(input.Index("#psi"),4,"\\psi");
  }
  while (input.Contains("TMP")){
    input.Replace(input.Index("TMP"),3,"\\psi");
  }
  while (input.Contains("\\\\")){
    input.Replace(input.Index("\\\\"),2,"\\");
  }
  return input;
}

