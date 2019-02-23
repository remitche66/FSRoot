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
    static FSModeInfo* addModeInfo(int mNumber);


      // *******************************************************
      // REMOVE MODES FROM THE COLLECTION
      // *******************************************************

    static void removeModeInfo(pair<int,int> mCode);
    static void removeModeInfo(int mCode1, int mCode2);
    static void removeModeInfo(TString mString);
    static void removeModeInfo(int mNumber);


      // *******************************************************
      // READ MODES FROM A FILE AND ADD TO THE COLLECTION
      // *******************************************************

    static map<TString, vector<TString> > 
      addModesFromFile(TString filename);


      // *******************************************************
      // RETRIEVE MODES FROM THE CACHE
      // *******************************************************

    static FSModeInfo* modeInfo(pair<int,int> mCode);
    static FSModeInfo* modeInfo(int mCode1, int mCode2);
    static FSModeInfo* modeInfo(TString mString);
    static FSModeInfo* modeInfo(int mNumber);


      // *******************************************************
      // FORM A VECTOR OF MODES FROM A SET OF CATEGORIES
      // *******************************************************

    static vector<FSModeInfo*> modeVector(TString category = "");
    static int               modeVectorSize(TString category = "");
    static FSModeInfo*         modeVectorElement(TString category = "", 
                                               unsigned int index = 0);


      // *******************************************************
      // NUMBER OF MODES THAT HAVE AN ASSOCIATED MODENUMBER;
      //   LOOP OVER ALL NUMBERED MODES LIKE:
      //     for (int i = 0; i < FSModeCollection::numberedModes(); i++){
      //       FSModeInfo mi(i);
      //       mi.display();
      //     }
      // *******************************************************

    static int numberedModes();


      // *******************************************************
      // TELL DIFFERENT ANALYSIS ALGORITHMS WHICH MODES TO FIND
      // *******************************************************

    static void printBESModes(TString category, TString outputFile, 
                              int nstart = 1, TString tag = "EXC");
    static void printCLEOModes(TString category, TString outputFile);


      // *******************************************************
      // TELL EVTGEN WHICH DECAYS TO GENERATE
      // *******************************************************

    static void printEvtGen(TString category, TString outputFile);


      // *******************************************************
      // PRINT A LIST OF MODES TO A FILE
      // *******************************************************

    static void printDescriptions(TString category, TString outputFile);


      // *******************************************************
      // PRINT STRINGS WITH REPLACED KEYWORDS 
      //   (see FSModeInfo::modeString) TO A FILE
      // *******************************************************

    static void printStrings(TString category, TString inputLine, TString outputFile,
                             int counterStart = 1, bool append = false);


      // *******************************************************
      // DISPLAY MODES IN THE CACHE
      // *******************************************************

    static void display(TString category = "");


      // *******************************************************
      // CLEAR THE CACHE
      // *******************************************************

    static void clear();


  private:

      // the mode cache
      // [[store the pair as (code2,code1) for sorting]]
    static map< pair<int,int>, FSModeInfo*> m_modeInfoMap;

      // the number of modes that have an associated number
    static int m_numberedModes;

};


#endif
