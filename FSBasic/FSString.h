#if !defined(FSSTRING_H)
#define FSSTRING_H

#include <cmath>
#include <cstdlib>
#include <vector>
#include <utility>
#include <string>
#include <map>
#include "TString.h"

using namespace std;


class FSString{

  public:

      // ********************************************************
      // CONVERT FROM STRING TO TSTRING AND BACK
      // ********************************************************

    static TString string2TString(string input);
    static string TString2string(TString input);


      // ********************************************************
      // PAD A TSTRING WITH SPACES 
      //   LRC = "L", "R", or "C" for left, right, or center-justified
      // ********************************************************

    static TString padTString(TString input, int length, TString LRC = "R");


      // ********************************************************
      // COMPARE TSTRINGS WITH WILDCARDS (* and ?)
      // ********************************************************

    static bool compareTStrings(TString inputNoWildCards, TString inputWithWildCards, bool caseSensitive = true);


      // ********************************************************
      // CONVERT FROM TSTRING TO INT
      // ********************************************************

    static int TString2int(TString input);


      // ********************************************************
      // CONVERT FROM TSTRING TO DOUBLE
      // ********************************************************

    static double TString2double(TString input);


      // ********************************************************
      // CONVERT FROM INT TO TSTRING
      //   also add leading zeros if the length is less than minimumDigits
      // ********************************************************

    static TString int2TString(long int number, int minimumDigits = 0, bool useCommas = false);


      // ********************************************************
      // ROUTINES TO CONVERT FROM DOUBLE TO TSTRING
      //   scientific == true:
      //       put the result in scentific format by adding eXX 
      //   fixdecimal == false: 
      //       *precision* will be the number of significant digits
      //   fixdecimal == true: 
      //       the last digit will be in the pow(10,*precision*) place
      //   sign:  can be "+-", "+", or "-"
      // ********************************************************


    static TString double2TString(double x, int precision = 3, int scientific = 0, 
                                          bool fixdecimal = false, bool show = false);

    static TString latexMeasurement(double x, double ex1, 
                                    int precision = 2, bool fixdecimal = false);

    static TString latexMeasurement(double x, TString sign1, double ex1, 
                                              TString sign2, double ex2, 
                                    int precision = 2, bool fixdecimal = false);

    static TString latexMeasurement(double x, TString sign1, double ex1, 
                                	      TString sign2, double ex2,
                                	      TString sign3, double ex3,
                                    int precision = 2, bool fixdecimal = false);

    static TString latexMeasurement(double x, TString sign1, double ex1, 
                                	      TString sign2, double ex2,
                                	      TString sign3, double ex3,
                                	      TString sign4, double ex4,
                                    int precision = 2, bool fixdecimal = false);


      // ********************************************************
      // CONVERT ROOT SYMBOLS TO LATEX FORMAT (e.g. "#pi^{+}" TO "\pi^{+}")
      // ********************************************************

    static TString root2latexSymbols(TString input);
    static TString latexSymbols(TString input);


      // ********************************************************
      // CONVERT SYMBOLS TO ROOT FORMAT (e.g. "pi+" TO "#pi^{+}")
      // ********************************************************

    static TString rootSymbols(TString input);


      // ********************************************************
      // PARSE STRINGS INTO WORDS
      // ********************************************************

    static vector<TString> parseTString(TString input, TString spacer = " ", 
                                        bool recordSpacers = false, bool display = false);
    static vector<TString> parseTString(vector<TString> inputs, TString spacer = " ", 
                                        bool recordSpacers = false, bool display = false);
    static vector<TString> parseTString(TString input, vector<TString> spacers,
                                        bool recordSpacers = false, bool display = false);
    static vector<TString> parseTString(vector<TString> inputs, vector<TString> spacers,
                                        bool recordSpacers = false, bool display = false);

      // ********************************************************
      // PARSE STRING INTO A MAP ACCORDING TO KEYS
      // ********************************************************

    static map<TString, TString> 
       parseTStringToMap1(TString input, vector<TString> keys, bool display = false);

    static map<TString, vector<TString> > 
       parseTStringToMap2(TString input, vector<TString> keys, bool display = false);


      // ********************************************************
      // CAPTURE SUBSTRINGS
      // ********************************************************

    static TString subString(TString input, int startIndex, int endIndex);
    static TString captureParentheses(TString input, int startIndex, 
                                      TString opening = "(", TString closing = ")");
    static TString captureMarker(TString input, TString templateString, TString marker);


      // ********************************************************
      // PARSE LOGIC 
      //    evalLogicalTString:
      //      Determines whether or not a list of categories contains
      //        what is in the input logic.
      //      For "and" use "&" or "&&".
      //      For "or"  use "," or "||".
      //      For "not" use "!".
      //      For example, for categories = {"A","B","C"}:
      //        input = "(A&B),D"  returns true
      //        input = "(A&B)&D"  returns false
      //        input = "(A&B)&!D" returns true
      // ********************************************************

    static bool evalLogicalTString(TString input, vector<TString> categories);


      // ********************************************************
      // LATEX UTILITIES
      // ********************************************************

    static void latexHeader(TString filename, bool append = false, TString title = "TITLE", 
                             TString author = "RM", TString date = "\\today");
    static void latexFigure(TString filename, TString figurename, 
                            TString width = "1.0", TString caption = "", 
                            bool append = true);
    static void latexLine(TString filename, TString text, bool append = true);
    static void latexCloser(TString filename, bool append = true);
    static void latexTemplate(TString filename, TString title = "TITLE", 
                             TString author = "RM", TString date = "\\today");


      // ********************************************************
      // MAKE A LATEX TABLE
      //   pass in a 2d array with tableContents[nrows][ncols]
      //    or pass in a 1d array with tableContents[ncols*nrows]
      //      that fills left to right, top to bottom
      // ********************************************************

    static void latexTable(int nrows, int ncols, TString* tableContents, 
                           TString filename, bool append = false, TString caption = "");


      // ********************************************************
      // READ LINES FROM A FILE
      // ********************************************************

    static vector<TString> readLinesFromFile(TString filename);


      // ********************************************************
      // WRITE A TSTRING TO A FILE
      // ********************************************************

    static void writeTStringToFile(TString filename, TString text, bool append = true);


      // ********************************************************
      // MAKE ALL PERMUTATIONS OF INPUTSTRING USING DEFINITIONS
      //   definitions has the format:
      //     NAME1 DEFN11 DEFN12 DEFN13
      //     NAME2 DEFN21 DEFN22 DEFN23 DEFN24
      //     NAME3 DEFN31 DEFN32
      // ********************************************************

    static vector<TString> expandDefinitions(TString inputString, 
                                             map<TString, vector<TString> > definitions,
                                             TString spacer = " ",
                                             bool synchronizeDefinitions = false);


      // ********************************************************
      // PARSE HISTOGRAM INPUTS
      // ********************************************************

    static int    parseBoundsNBinsX(TString bounds);
    static double parseBoundsLowerX(TString bounds);
    static double parseBoundsUpperX(TString bounds);
    static int    parseBoundsNBinsY(TString bounds);
    static double parseBoundsLowerY(TString bounds);
    static double parseBoundsUpperY(TString bounds);
    static double parseBoundsBinSizeX(TString bounds);
    static TString makeBounds(int nbinsX,      double lowerX,      double upperX,
                              int nbinsY = -1, double lowerY = -1, double upperY = -1);
    static TString makeBounds(int dimension);
    static bool checkBounds(int dimension, TString bounds);

    static TString parseVariableX(TString variable);
    static TString parseVariableY(TString variable);
    static bool checkVariable(int dimension, TString variable);


      // ********************************************************
      // STRIP EXTRA WHITESPACE AROUND STRINGS AND REMOVE TABS
      // ********************************************************

    static bool containsWhiteSpace(TString input);
    static TString removeTabs(TString input);
    static TString removeWhiteSpace(TString input);


      // ********************************************************
      // EXPAND THE KEYWORD "SUM"
      //   "SUM{xx:1,2,3}{xx*yy}" ==> "(1*yy+2*yy+3*yy)"
      //   "SUM{xx:1,2,3}{SUM{yy:4,5}{xx*yy}}" ==> "((1*4+1*5)+(2*4+2*5)+(3*4+3*5))"
      // ********************************************************

    static TString expandSUM(TString inputString);


      // ********************************************************
      // EXPAND INTEGERS (USEFUL FOR COMBINATORICS)
      //   e.g. "a12b" ==> "a00b","a01b","a02b","a10b","a11b","a12b"
      // ********************************************************

    static vector<TString> expandIntegers(TString input, bool show = false);


      // ********************************************************
      // VARIOUS CHECKS TO HELP DEBUGGING
      // ********************************************************

    static bool checkParentheses(TString input, TString opening = "(", TString closing = ")");


      // ********************************************************
      // REORDER A MATH EXPRESSION
      //   known problem:  breaks for negatives in exponents like 1e-6
      // ********************************************************

    static TString reorderMath(TString input, bool show = false);

  private:

    static bool compareChars(const char* cNone, const char* cWith);

    static vector<TString> parseBounds(TString bounds);

    static bool evalBooleanTString(TString input);

      // helper function for double2TString

    static pair<int, pair<double,int> > 
       double2SignNumberExponent(double x, int precision = 12, 
                                 bool fixdecimal = false, bool show = false);

      // helper function for expandIntegers

    static vector< vector<int> >
      pushBackToEach(vector< vector<int> > originalList, vector<int> newPart);

      // helper functions for reorderMath

    static vector<TString> m_reorderMathSpacers;
    static int nMathSpacer(TString part);
    static TString reorderMathHelp(vector<TString> parts, bool sort);

};



#endif

