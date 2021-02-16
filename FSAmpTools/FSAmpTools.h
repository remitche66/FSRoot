#if !defined(FSAMPTOOLS_H)
#define FSAMPTOOLS_H

#include <cmath>
#include "TString.h"
#include "IUAmpTools/ConfigFileParser.h"
#include "IUAmpTools/AmpToolsInterface.h"
#include "IUAmpTools/FitResults.h"
#include "FSAmpToolsDataIO/FSAmpToolsDataWriter.h"

using namespace std;


class FSAmpTools{

  public:


    static void setupFromFitResults(TString fitResultsFile);

    static void setupFromConfigFile(TString configFile);

    static void showAmpNames(TString ampNameLogic = "*");

      // ampWtName = RE/IM/PH/IN + name
    static void defineAmpWt(TString ampWtName, TString ampNameLogic = "*", bool show = false);

    static void showAmpWts(TString ampWtNameLogic = "*");

    static void clearAmpWts(TString ampWtNameLogic = "*", bool show = false);

    static void makeAmpWts(TString fileName, TString treeName, TString reactionName, 
                           int numParticles, double dataEvents = -1);


    //static void testSystem(TString configFile);



  private:

    static void setAmpNamesFromConfigFile();
    static vector<TString> getAmpNames(vector<TString> ampNames, 
                                       TString ampNameLogic = "*", bool show = false);
    static vector<TString> getAmpNames(TString ampNameLogic = "*", bool show = false);
    static vector<TString> getAmpWtNames(TString ampWtNameLogic = "*", bool show = false);

    static vector< vector<TString> > sortAmpsIntoSums(vector<TString> ampNames);

    static double calcINFromATI(int iEvent, const vector< vector<string> >& ampNames);
    static double calcREFromATI(int iEvent, const vector<string>& ampNames);
    static double calcIMFromATI(int iEvent, const vector<string>& ampNames);
    static double calcPHFromATI(int iEvent, const vector<string>& ampNames);

    static vector<string> vTString2string(vector<TString> vTStrings);
    static vector< vector<string> > vvTString2string(vector< vector<TString> > vvTStrings);

    static ConfigurationInfo* m_configInfo;
    static AmpToolsInterface* m_ATI;
    static vector<TString> m_ampNames;
    static vector<TString> m_ampWtNames;
    static map<TString, TString> m_ampWtMap;

};


#endif
