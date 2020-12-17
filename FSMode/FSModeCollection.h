#if !defined(FSMODECOLLECTION_H)
#define FSMODECOLLECTION_H

#include <utility>
#include <map>
#include "FSMode/FSModeInfo.h"

using namespace std;


class FSModeCollection{

  public:

      // *******************************************************
      // ADD MODES TO THE COLLECTION
      // *******************************************************

    static FSModeInfo* addModeInfo(pair<int,int> mCode);
    static FSModeInfo* addModeInfo(int mCode1, int mCode2);
    static FSModeInfo* addModeInfo(TString mString);


      // *******************************************************
      // READ MODES FROM A FILE AND ADD TO THE COLLECTION
      // *******************************************************

    static void addModesFromFile(TString filename);


      // *******************************************************
      // FORM A VECTOR OF MODES FROM A SET OF CATEGORIES
      // *******************************************************

    static vector<FSModeInfo*> modeVector(TString category = "");
    static int                 modeVectorSize(TString category = "");
    static FSModeInfo*         modeVectorElement(TString category = "", 
                                               unsigned int index = 0);


      // *******************************************************
      // DEFINE MODE-SPECIFIC CUTS USING FSCUT
      //   Note:  name cuts using keywords like MODESTRING
      //     to distinguish modes (see FSModeInfo::modeString)
      // *******************************************************

    static void defineCuts(TString category, TString cutName, TString cut, 
                           TString cutSideBand = "", double weight = 1.0);


      // *******************************************************
      // PRINT COMBINATORICS TO THE SCREEN FOR TESTING
      // *******************************************************

    static void testCombinatorics(TString category, TString testString,
                                    bool expandModeString = false);


      // *******************************************************
      // PRINT STRINGS WITH REPLACED KEYWORDS TO A FILE
      //  Or print to the screen if outputfile == "".
      //  Use modeStrings to return the same strings as a vector.
      //   See FSModeInfo::modeString(...) for keywords.
      // *******************************************************

    static void printStrings(TString category, TString inputLine, 
                             TString outputFile = "",
                             int counterStart = 1, bool append = false);

    static vector<TString> modeStrings(TString category, TString inputLine, 
                                       int counterStart = 1, bool show = false);


      // *******************************************************
      // DISPLAY MODES IN THE COLLECTION
      // *******************************************************

    static void display(TString category = "");


      // *******************************************************
      // CLEAR THE COLLECTION
      // *******************************************************

    static void clear(TString category = "");


  private:

      // the mode collection
      // [[store the pair as (code2,code1) for sorting]]
    static map< pair<int,int>, FSModeInfo*> m_modeInfoMap;

      // helper function for addModesFromFile
    static TString expandLine(TString line, TString word, TString definition, 
                   map<TString, vector<TString> >& decayMap);


};


#endif
