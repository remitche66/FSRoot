#if !defined(FSAMPTOOLS_H)
#define FSAMPTOOLS_H

#include <cmath>
#include "TString.h"
#include "IUAmpTools/ConfigFileParser.h"
#include "FSAmpToolsDataIO/FSAmpToolsDataWriter.h"

using namespace std;


class FSAmpTools{

  public:

    static void readAmplitudeNames(TString configFile, bool show = false);
    static vector<TString> getAmplitudeNames(TString amplitudeNameLogic = "*", bool show = false);
    static void showAmplitudeNames(TString amplitudeNameLogic = "*");
    static void clearAmplitudeNames(TString amplitudeNameLogic = "*", bool show = false);


    static void testSystem(TString configFile);
    static void generatePhaseSpace(TString outFile, int numEvents);

  private:

    static vector<TString> m_ampNames;

};


#endif
