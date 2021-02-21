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

    static void showConfigInfo();

    static void showAmpNames(TString ampNameLogic = "*", TString rctNameLogic = "*");

    static void showRctNames(TString rctNameLogic = "*");

      // ampWtName = RE/IM/PH/IN + name
    static void defineAmpWt(TString ampWtName, TString ampNameLogic = "*", bool show = false);

    static void showAmpWts(TString ampWtNameLogic = "*", TString rctnNameLogic = "*", int iEvent = -1);

    static void clearAmpWts(TString ampWtNameLogic = "*", bool show = false);

    static void makeAmpWts(TString fileName, TString treeName, TString reactionName, 
                           int numParticles, double numGeneratedMCEvents = -1);

    static double getAmpWt(TString ampWtName, int iEvent);


    //static void testSystem(TString configFile);


  private:

    static void setAmpNamesFromConfigFile();
    static vector<TString> getAmpNames(TString ampNameLogic = "*", 
                                       TString rctNameLogic = "*", bool show = false);

    static void setRctNamesFromConfigFile();
    static vector<TString> getRctNames(TString rctNameLogic = "*", bool show = false);

    static vector<TString> getAmpWtNames(TString ampWtNameLogic = "*", TString rctNameLogic = "*",
                                         int iEvent = -1, bool show = false);



     // vector< pair< pair< ampWtName,ampWtType>, vector< vector<ampName> > > > 
    static vector< pair< pair<TString,TString>, vector< vector<string> > > > 
                      groupAmpsForWts(TString reactionName, TString ampWtNameLogic = "*");
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
    static vector<TString> m_rctNames;
    static vector<TString> m_ampWtNames;
    static map<TString, TString> m_ampWtMap;
    static bool m_parametersFromFit;
    static TString m_loadedATIReaction;

};


#endif
