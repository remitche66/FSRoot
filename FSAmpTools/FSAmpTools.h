#if !defined(FSAMPTOOLS_H)
#define FSAMPTOOLS_H

#include <cmath>
#include "TString.h"
#include "IUAmpTools/ConfigFileParser.h"
#include "IUAmpTools/AmpToolsInterface.h"
#include "FSAmpToolsDataIO/FSAmpToolsDataWriter.h"

using namespace std;


class FSAmpTools{

  public:


    static void setupFromConfigFile(TString configFile);

    static void showAmpNames(TString ampNameLogic = "*");

      // ampWtName = RE/IM/PH/IN + name
    static void defineAmpWt(TString ampWtName, TString ampNameLogic = "*", bool show = false);

    static void showAmpWts(TString ampWtNameLogic = "*");

    static void clearAmpWts(TString ampWtNameLogic = "*", bool show = false);

    static void makeAmpWts(TString fileName, TString treeName, TString reactionName, int numParticles);


/*

    static void testSystem(TString configFile);
    static void generatePhaseSpace(TString outFile, int numEvents);

*/


  private:

    static void setAmpNamesFromConfigFile();
    static vector<TString> getAmpNames(vector<TString> ampNames, 
                                       TString ampNameLogic = "*", bool show = false);
    static vector<TString> getAmpNames(TString ampNameLogic = "*", bool show = false);
    static vector<TString> getAmpWtNames(TString ampWtNameLogic = "*", bool show = false);

    static vector< vector<TString> > sortAmpsIntoSums(vector<TString> ampNames);

    static double calcINFromATI(int iEvent, const vector< vector<TString> >& ampNames);
    static double calcREFromATI(int iEvent, const vector<TString>& ampNames);
    static double calcIMFromATI(int iEvent, const vector<TString>& ampNames);

    static TString m_configFile;
    static ConfigurationInfo* m_configInfo;
    static AmpToolsInterface* m_ATI;
    static vector<TString> m_ampNames;
    static vector<TString> m_ampWtNames;
    static map<TString, TString> m_ampWtMap;

};


#endif
