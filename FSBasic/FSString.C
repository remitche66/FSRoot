#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "TString.h"
#include "TFormula.h"
#include "FSBasic/FSControl.h"
#include "FSBasic/FSString.h"


  // ********************************************************
  // CONVERSIONS FROM STRING TO TSTRING AND BACK
  // ********************************************************

TString
FSString::string2TString(string input){
  return TString(input.c_str());
}

string
FSString::TString2string(TString input){
  string output("");
  for (int i = 0; i < input.Length(); i++){
    output += input[i];
  }
  return output;
}


  // ********************************************************
  // PAD A TSTRING WITH SPACES 
  //   LRC = "L", "R", or "C" for left, right, or center-justified
  // ********************************************************

TString
FSString::padTString(TString input, int length, TString LRC){
  int spacesT = length - input.Length();
  if (spacesT <= 0) return input;
  int spacesL = 0;
  int spacesR = 0;
  if (LRC == "L") spacesR = spacesT;
  if (LRC == "R") spacesL = spacesT;
  if (LRC == "C"){ spacesL = spacesT/2;  spacesR = spacesT - spacesL; }
  for (int i = 0; i < spacesL; i++){ input = " " + input; }
  for (int i = 0; i < spacesR; i++){ input = input + " "; }
  return input;
}


  // ********************************************************
  // COMPARE TSTRINGS WITH WILDCARDS (* and ?)
  // ********************************************************

bool
FSString::compareTStrings(TString inputNoWildCards, TString inputWithWildCards, bool caseSensitive){
  if (!caseSensitive){ inputNoWildCards.ToUpper(); inputWithWildCards.ToUpper(); }
  if (!inputWithWildCards.Contains("*")&&!inputWithWildCards.Contains("?"))
    return (inputNoWildCards == inputWithWildCards);
  while (inputWithWildCards.Contains("**")){
    inputWithWildCards.Replace(inputWithWildCards.Index("**"),2,"*"); }
  string sNone = FSString::TString2string(inputNoWildCards);
  string sWith = FSString::TString2string(inputWithWildCards);
  return compareChars(sNone.c_str(),sWith.c_str());
}

bool
FSString::compareChars(const char* cNone, const char* cWith) { 
  if (*cNone == '\0' && *cWith == '\0') return true;
  if (*cNone == '\0' && *cWith == '*' && *(cWith+1) == '\0') return true;
  if (*cNone == '\0' || *cWith == '\0') return false;
  if (*cNone == *cWith || *cWith == '?') return compareChars(cNone+1,cWith+1); 
  if (*cWith == '*') return compareChars(cNone,cWith+1) || compareChars(cNone+1,cWith); 
  return false;
} 


  // ********************************************************
  // CONVERT STRINGS TO NUMBERS
  // ********************************************************


int
FSString::TString2int(TString input){
  return (int) FSString::TString2double(input);
}

double
FSString::TString2double(TString input){
  input = FSString::removeWhiteSpace(input);
  while (input.Length() > 0){
    TString digit1(""); if (input.Length() > 0) digit1 = input[0];
    TString digit2(""); if (input.Length() > 1) digit2 = input[1];
    TString digit3(""); if (input.Length() > 2) digit3 = input[2];
    if (digit1.IsFloat()) break;
    if ((digit1 == "." || digit1 == "-") && digit2.IsFloat()) break;
    if (digit1 == "-" && digit2 == "." && digit3.IsFloat()) break;
    input.Replace(0,1,"");
  }
  while (input.Length() > 0){
    if (input.IsFloat()){ return atof(FSString::TString2string(input).c_str()); }
    input.Replace(input.Length()-1,1,"");
  }
  return 0.0;
}


  // ********************************************************
  // CONVERT FROM INT TO TSTRING
  // ********************************************************

TString 
FSString::int2TString(int number, int minimumDigits){
  TString snumber("");  snumber += number;
  TString newsnumber("");
  for (int i = snumber.Length(); i < minimumDigits; i++){ newsnumber += "0"; }
  newsnumber += snumber;
  return newsnumber;
}



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

pair<int, pair<double,int> >
FSString::double2SignNumberExponent(double x, int precision, bool fixdecimal, bool show){
  if (!fixdecimal && precision > 12){ 
    cout << "FSString::double2SignNumberExponent: max digits = 12" << endl; exit(0); }
  if (!fixdecimal && precision <  1){ 
    cout << "FSString::double2SignNumberExponent: digits < 1" << endl; exit(0); }
  double ax = abs(x);  ax += ax*1.0e-14;  int exp = 0;
  int isign = 1; if (x < 0.0) isign = -1;
  while (ax >= 10.0 && abs(exp) < 100){ ax /= 10.0; exp++; }
  while (ax  <  1.0 && abs(exp) < 100){ ax *= 10.0; exp--; }
  if (abs(exp) == 100) {isign = 1; ax = 0.0; exp = 0; }
  if (fixdecimal && (exp-precision+1) > 12){ 
    cout << "FSString::double2SignNumberExponent WARNING: sig digits = " 
         << exp-precision+1 << endl;}
  if (!fixdecimal){ ax += ax*1.0e-14; ax = round(pow(10,precision-1)*ax)/pow(10,precision-1);
                    ax += ax*1.0e-14; }
  if (fixdecimal){  ax += ax*1.0e-14; ax = round(pow(10,exp-precision)*ax)/pow(10,exp-precision);
                    ax += ax*1.0e-14; }
  if (ax < 1.0) {isign = 1; ax = 0.0; exp = 0; }
  if (ax >= 10.0) 
    return FSString::double2SignNumberExponent(isign*ax*pow(10.0,exp),precision,fixdecimal,show);
  if (show){ cout.precision(12); cout << isign << " " << std::fixed << ax << " " << exp << endl; }
  return pair<int, pair<double,int> >(isign,pair<double,int>(ax,exp));
}

TString
FSString::double2TString(double x, int precision, bool scientific, bool fixdecimal, bool show){
  pair<int, pair<double,int> > sne = FSString::double2SignNumberExponent(x,precision,fixdecimal,show);
  int isign = sne.first;  double ax = sne.second.first;  int exp = sne.second.second;
  int totalDigits = precision;  if (fixdecimal) totalDigits = exp - precision + 1;
  TString saxInt = "0";  TString saxFrac = "";
  for (int i = 0; i < totalDigits; i++){
    if (i == 0) saxInt   = FSString::int2TString((int)ax);
    if (i != 0) saxFrac += FSString::int2TString((int)ax);
    ax -= floor(ax);  ax *= 10.0;  ax += 1.0e-14;
  }
  TString saxNum = saxInt + saxFrac;
  TString sDouble(""); if (isign < 0) sDouble = "-";
  if (scientific){
    sDouble += saxInt; if (totalDigits > 1){ sDouble += "."; sDouble += saxFrac; }
    if (exp < 0) sDouble += "e-"; if (exp >= 0) sDouble += "e+";
    sDouble += FSString::int2TString(abs(exp),2);
  }
  if (!scientific){
    int firstDigitPlace = precision + totalDigits - 1;  if (!fixdecimal) firstDigitPlace = exp;
    int lastDigitPlace = firstDigitPlace - totalDigits + 1;
    int firstPlace = firstDigitPlace; if (firstPlace < 0) firstPlace = 0;
    int lastPlace = lastDigitPlace; if (lastPlace > 0) lastPlace = 0;
    int iDigitPlace = 0;
    for (int iPlace = -1*firstPlace; iPlace <= -1*lastPlace; iPlace++){
      if (iPlace == 1) sDouble += ".";
      if (iPlace >= -1*firstDigitPlace && iPlace <= -1*lastDigitPlace){ 
            sDouble += saxNum[iDigitPlace++]; }
      else{ sDouble += "0"; }
    }
  }
  return sDouble;
}


TString 
FSString::latexMeasurement(double x, double ex1, 
                                  int precision, bool fixdecimal){
  if (!fixdecimal){
    int ex1precision = -1*precision + 1
          + double2SignNumberExponent(ex1,precision,false).second.second;
    precision = ex1precision;
  }
  TString  sx = double2TString(  x,precision,false,true);
  TString sex = double2TString(ex1,precision,false,true);
  return (sx+"\\pm"+sex);
}


TString 
FSString::latexMeasurement(double x, TString sign1, double ex1, 
                                   TString sign2, double ex2, 
                                  int precision, bool fixdecimal){
  if (!fixdecimal){
    int ex1precision = -1*precision + 1
          + double2SignNumberExponent(ex1,precision,false).second.second;
    int ex2precision = -1*precision + 1
          + double2SignNumberExponent(ex2,precision,false).second.second;
                                  precision = ex1precision;
    if (precision < ex2precision) precision = ex2precision;
  }
  TString  sx  = double2TString(  x,precision,false,true);
  TString sex1 = double2TString(ex1,precision,false,true);
  TString sex2 = double2TString(ex2,precision,false,true);
  TString a1(""); TString b1("");  if (sign1 == "+-"){ a1 = "\\pm"; b1 = "";}
                                   if (sign1 == "+") { a1 = "^{+";  b1 = "}";}
                                   if (sign1 == "-") { a1 = "_{-";  b1 = "}";}
  TString a2(""); TString b2("");  if (sign2 == "+-"){ a2 = "\\pm"; b2 = "";}
                                   if (sign2 == "+") { a2 = "^{+";  b2 = "}";}
                                   if (sign2 == "-") { a2 = "_{-";  b2 = "}";}
  return (sx+a1+sex1+b1+a2+sex2+b2);
}


TString 
FSString::latexMeasurement(double x, TString sign1, double ex1, 
                                   TString sign2, double ex2,
                                   TString sign3, double ex3, 
                                  int precision, bool fixdecimal){
  if (!fixdecimal){
    int ex1precision = -1*precision + 1
          + double2SignNumberExponent(ex1,precision,false).second.second;
    int ex2precision = -1*precision + 1
          + double2SignNumberExponent(ex2,precision,false).second.second;
    int ex3precision = -1*precision + 1
          + double2SignNumberExponent(ex3,precision,false).second.second;
                                  precision = ex1precision;
    if (precision < ex2precision) precision = ex2precision;
    if (precision < ex3precision) precision = ex3precision;
  }
  TString sx    = double2TString( x, precision,false,true);
  TString sex1  = double2TString(ex1,precision,false,true);
  TString sex2  = double2TString(ex2,precision,false,true);
  TString sex3  = double2TString(ex3,precision,false,true);
  TString a1(""); TString b1("");  if (sign1 == "+-"){ a1 = "\\pm"; b1 = "";}
                                   if (sign1 == "+") { a1 = "^{+";  b1 = "}";}
                                   if (sign1 == "-") { a1 = "_{-";  b1 = "}";}
  TString a2(""); TString b2("");  if (sign2 == "+-"){ a2 = "\\pm"; b2 = "";}
                                   if (sign2 == "+") { a2 = "^{+";  b2 = "}";}
                                   if (sign2 == "-") { a2 = "_{-";  b2 = "}";}
  TString a3(""); TString b3("");  if (sign3 == "+-"){ a3 = "\\pm"; b3 = "";}
                                   if (sign3 == "+") { a3 = "^{+";  b3 = "}";}
                                   if (sign3 == "-") { a3 = "_{-";  b3 = "}";}
  return (sx+a1+sex1+b1+a2+sex2+b2+a3+sex3+b3);
}


TString 
FSString::latexMeasurement(double x, TString sign1, double ex1, 
                                   TString sign2, double ex2,
                                   TString sign3, double ex3,
                                   TString sign4, double ex4,
                                  int precision, bool fixdecimal){
  if (!fixdecimal){
    int ex1precision = -1*precision + 1
          + double2SignNumberExponent(ex1,precision,false).second.second;
    int ex2precision = -1*precision + 1
          + double2SignNumberExponent(ex2,precision,false).second.second;
    int ex3precision = -1*precision + 1
          + double2SignNumberExponent(ex3,precision,false).second.second;
    int ex4precision = -1*precision + 1
          + double2SignNumberExponent(ex4,precision,false).second.second;
                                  precision = ex1precision;
    if (precision < ex2precision) precision = ex2precision;
    if (precision < ex3precision) precision = ex3precision;
    if (precision < ex4precision) precision = ex4precision;
  }
  TString sx    = double2TString( x, precision,false,true);
  TString sex1  = double2TString(ex1,precision,false,true);
  TString sex2  = double2TString(ex2,precision,false,true);
  TString sex3  = double2TString(ex3,precision,false,true);
  TString sex4  = double2TString(ex4,precision,false,true);
  TString a1(""); TString b1("");  if (sign1 == "+-"){ a1 = "\\pm"; b1 = "";}
                                   if (sign1 == "+") { a1 = "^{+";  b1 = "}";}
                                   if (sign1 == "-") { a1 = "_{-";  b1 = "}";}
  TString a2(""); TString b2("");  if (sign2 == "+-"){ a2 = "\\pm"; b2 = "";}
                                   if (sign2 == "+") { a2 = "^{+";  b2 = "}";}
                                   if (sign2 == "-") { a2 = "_{-";  b2 = "}";}
  TString a3(""); TString b3("");  if (sign3 == "+-"){ a3 = "\\pm"; b3 = "";}
                                   if (sign3 == "+") { a3 = "^{+";  b3 = "}";}
                                   if (sign3 == "-") { a3 = "_{-";  b3 = "}";}
  TString a4(""); TString b4("");  if (sign4 == "+-"){ a4 = "\\pm"; b4 = "";}
                                   if (sign4 == "+") { a4 = "^{+";  b4 = "}";}
                                   if (sign4 == "-") { a4 = "_{-";  b4 = "}";}
  return (sx+a1+sex1+b1+a2+sex2+b2+a3+sex3+b3+a4+sex4+b4);
}



  // ********************************************************
  // CONVERT ROOT SYMBOLS TO LATEX FORMAT (e.g. "#pi^{+}" TO "\pi^{+}")
  // ********************************************************

TString
FSString::root2latexSymbols(TString input){
  while (input.Contains("#")){
    input.Replace(input.Index("#"),1,"\\");
  }
  return input;
}

  // ********************************************************
  // CONVERT SYMBOLS TO ROOT FORMAT (e.g. "pi+" TO "#pi^{+}")
  // ********************************************************

TString
FSString::rootSymbols(TString input){
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
  // PARSE STRINGS INTO WORDS
  // ********************************************************

vector<TString>
FSString::parseTString(TString input, TString spacer, bool recordSpacers, bool display){
  input = FSString::removeTabs(input);
  spacer = FSString::removeTabs(spacer);
  vector<TString> words;
  while (input.Contains(spacer)){
    int index = input.Index(spacer);
    int size = spacer.Length();
    TString s1("");  TString s2("");
    for (int i = 0;          i < index;          i++){ TString digit(input[i]); s1 += digit; }
    for (int i = index+size; i < input.Length(); i++){ TString digit(input[i]); s2 += digit; }
    if (!s1.Contains(spacer) && (s1.Length() > 0)) words.push_back(s1);
    if (recordSpacers) words.push_back(spacer);
    if (!s2.Contains(spacer) && (s2.Length() > 0)) words.push_back(s2);
    input = s2;
  }
  if ((words.size() == 0) && (input.Length() > 0)) words.push_back(input);
  if (display) cout << "FSString::parseTString input:" << endl;
  if (display) cout << " " << input << endl;
  if (display) cout << "FSString::parseTString output:" << endl;
  if (display) for (unsigned int i = 0; i < words.size(); i++){ cout << " " << words[i] << endl; }
  return words;
}    

vector<TString>
FSString::parseTString(vector<TString> inputs, TString spacer, bool recordSpacers, bool display){
  vector<TString> words;
  for (unsigned int i = 0; i < inputs.size(); i++){
    vector<TString> wordsTemp = FSString::parseTString(inputs[i],spacer,recordSpacers);
    for (unsigned int j = 0; j < wordsTemp.size(); j++){ words.push_back(wordsTemp[j]); }
  }
  if (display) cout << "FSString::parseTString input:" << endl;
  if (display) for (unsigned int i = 0; i < inputs.size(); i++){ cout << " " << inputs[i] << endl; }
  if (display) cout << "FSString::parseTString output:" << endl;
  if (display) for (unsigned int i = 0; i < words.size(); i++){ cout << " " << words[i] << endl; }
  return words;
}

vector<TString>
FSString::parseTString(TString input, vector<TString> spacers, bool recordSpacers, bool display){
  input = FSString::removeTabs(input);
  vector<TString> words;
  words.push_back(input);
        // try to account for tricky special cases by sorting, or throw a warning
      if (spacers.size() > 1){
        for (unsigned int i = 0; i < spacers.size()-1; i++){
          for (unsigned int j = i+1; j < spacers.size(); j++){
            if (recordSpacers && (spacers[i].Contains(spacers[j]) ||
                                  spacers[j].Contains(spacers[i])))
              cout << "FSString::parseTString WARNING: spacers within spacers" << endl;
            if (spacers[i].Length() < spacers[j].Length()){
              TString temp = spacers[i];
              spacers[i] = spacers[j];
              spacers[j] = temp;
      } } } }
  for (unsigned int i = 0; i < spacers.size(); i++){
    words = FSString::parseTString(words,spacers[i],recordSpacers);
  }
  if (display) cout << "FSString::parseTString input:" << endl;
  if (display) cout << " " << input << endl;
  if (display) cout << "FSString::parseTString output:" << endl;
  if (display) for (unsigned int i = 0; i < words.size(); i++){ cout << " " << words[i] << endl; }
  return words;
}

vector<TString>
FSString::parseTString(vector<TString> inputs, vector<TString> spacers, bool recordSpacers, bool display){
  vector<TString> words;
  for (unsigned int i = 0; i < inputs.size(); i++){
    vector<TString> tempWords = FSString::parseTString(inputs[i],spacers,recordSpacers);
    for (unsigned int j = 0; j < tempWords.size(); j++){ words.push_back(tempWords[j]); }
  }
  if (display) cout << "FSString::parseTString input:" << endl;
  if (display) for (unsigned int i = 0; i < inputs.size(); i++){ cout << " " << inputs[i] << endl; }
  if (display) cout << "FSString::parseTString output:" << endl;
  if (display) for (unsigned int i = 0; i < words.size(); i++){ cout << " " << words[i] << endl; }
  return words;
}


   // ********************************************************
   // PARSE STRING INTO A MAP ACCORDING TO KEYS
   // ********************************************************

map<TString,vector<TString> >
FSString::parseTStringToMap2(TString input, vector<TString> keys, bool display){
  map<TString,vector<TString> > wordMap;
  vector<TString> words = parseTString(input,keys,true);
  TString key = ""; TString value = "";
  for (unsigned int i = 0; i < words.size(); i++){
    bool isKey = false; 
    for (unsigned int j = 0; j < keys.size(); j++){ if (words[i] == keys[j]){ isKey = true; break; } }
    if (isKey){
      if (key != "") wordMap[key].push_back(value);
      key = words[i]; value = "";
    }
    else{
      value = words[i];
    }
    if ((i == words.size()-1) && (key != "")) wordMap[key].push_back(value);
  }
  if (display) cout << "FSString::parseTStringToMap2 input:" << endl;
  if (display) cout << " " << input << endl;
  if (display) cout << "FSString::parseTStringToMap2 output:" << endl;
  if (display){for (map<TString, vector<TString> >::iterator it = wordMap.begin();
                    it != wordMap.end(); it++){
                 for (unsigned int i = 0; i < it->second.size(); i++){
                   cout << it->first << " = " << it->second[i] << endl;
                 }}}
  return wordMap;
}

map<TString,TString>
FSString::parseTStringToMap1(TString input, vector<TString> keys, bool display){
  map<TString,vector<TString> > wordMap2 = parseTStringToMap2(input,keys);
  map<TString,TString> wordMap1;
  for (map<TString,vector<TString> >::iterator it = wordMap2.begin(); it != wordMap2.end(); it++){
    wordMap1[it->first] = it->second[0];
  }
  if (display) cout << "FSString::parseTStringToMap1 input:" << endl;
  if (display) cout << " " << input << endl;
  if (display) cout << "FSString::parseTStringToMap1 output:" << endl;
  if (display){for (map<TString,TString>::iterator it = wordMap1.begin();
                    it != wordMap1.end(); it++){
                   cout << it->first << " = " << it->second << endl;
                 }}
  return wordMap1;
}


  // ********************************************************
  // CAPTURE SUBSTRINGS
  // ********************************************************

TString
FSString::subString(TString input, int startIndex, int endIndex){
  TString output("");
  if (startIndex < 0) return output;
  for (int i = startIndex; i < input.Length() && i < endIndex; i++){
    output += (TString)input[i];
  }
  return output;
}


TString
FSString::captureParentheses(TString input, int startIndex, TString opening, TString closing){
  if (startIndex < 0) return TString("");
  TString searchInput = FSString::subString(input,startIndex,input.Length());
  if (!searchInput.Contains(opening)) return TString("");
  if (!searchInput.Contains(closing)) return TString("");
  int index1 = searchInput.Index(opening);
  int index2 = -1;
  int pcount = 1;
  for (int i = index1+opening.Length(); i < searchInput.Length(); i++){
    if (FSString::subString(searchInput,i,i+opening.Length()) == opening) pcount++;
    if (FSString::subString(searchInput,i,i+closing.Length()) == closing) pcount--;
    if (pcount == 0){ index2 = i; break; }
  }
  return FSString::subString(searchInput,index1+opening.Length(),index2);
}


      // ********************************************************
      // PARSE LOGIC 
      // ********************************************************


bool
FSString::evalLogicalTString(TString input, vector<TString> cats){
  input = FSString::removeWhiteSpace(input);
  if (input == "") return true;
  if (!FSString::checkParentheses(input)){
    cout << "FSString::evalLogicalTString ERROR: parentheses problem in " << input << endl;
    exit(1);
  }
  vector<TString> spacers;
  spacers.push_back("(");
  spacers.push_back(")");
  spacers.push_back(",");
  spacers.push_back("&");
  spacers.push_back("|");
  spacers.push_back("!");
  vector<TString> words = FSString::parseTString(input,spacers);
  for (unsigned int i = 0; i < words.size(); i++){
  for (unsigned int j = i+1; j < words.size(); j++){
    if (words[i].Length() < words[j].Length()){ 
      TString temp = words[i]; words[i] = words[j]; words[j] = temp; }
  }}
  for (unsigned int i = 0; i < words.size(); i++){
    TString found("(1==0)");
    for (unsigned int ic = 0; ic < cats.size(); ic++){
      if (FSString::compareTStrings(cats[ic],words[i])){ found = "(1==1)";  break; }
    }
    while (input.Contains(words[i])){
      input.Replace(input.Index(words[i]),words[i].Length(),found);
    }
  }
  return evalBooleanTString(input);
}


bool 
FSString::evalBooleanTString(TString input){
  while (input.Contains("&&")) input.Replace(input.Index("&&"),2,"%%");
  while (input.Contains("&"))  input.Replace(input.Index("&"), 1,"%%");
  while (input.Contains("%%")) input.Replace(input.Index("%%"),2,"&&");
  while (input.Contains(","))  input.Replace(input.Index(","), 1,"||");
  TFormula fBool("fBool",input);
  if (fBool.Eval(0) < 0.1) return false;
  return true;
}


  // ********************************************************
  // LATEX UTILITIES
  // ********************************************************

void
FSString::latexHeader(TString filename, bool append){
  std::ios_base::openmode mode = ios::out;
  if (append) mode = ios::app;
  ofstream outfile(TString2string(filename).c_str(),mode);
  outfile << "\\documentclass[11pt]{article}" << endl;
  outfile << "\\usepackage{graphicx}" << endl;
  outfile << "\\usepackage{amssymb}" << endl;
  outfile << "\\usepackage{color}" << endl;
  outfile << "\\usepackage[]{epsfig}" << endl;
  outfile << endl;
  outfile << "\\textheight 8.0in" << endl;
  outfile << "\\topmargin 0.0in" << endl;
  outfile << "\\textwidth 6.0in" << endl;
  outfile << "\\oddsidemargin 0.25in" << endl;
  outfile << "\\evensidemargin 0.25in" << endl;
  outfile << endl;
  outfile << "\\newcommand{\\gevc}{\\mathrm{GeV/c}}" << endl;
  outfile << endl;
  outfile << "\\setlength{\\parindent}{0pt}" << endl;
  outfile << "\\setlength{\\parskip}{11pt}" << endl;
  outfile << endl;
  outfile << "\\begin{document}" << endl;
  outfile << endl;
  outfile << "\%\\title{}" << endl;
  outfile << "\%\\author{}" << endl;
  outfile << "\%\\maketitle" << endl;
  outfile << endl;
  outfile << "\%\\abstract{}" << endl;
  outfile.close();
}


void
FSString::latexFigure(TString filename, TString figurename, 
                             TString width, TString caption, bool append){
  std::ios_base::openmode mode = ios::out;
  if (append) mode = ios::app;
  ofstream outfile(TString2string(filename).c_str(),mode);
  outfile << endl;
  //outfile << "\\newpage" << endl;
  outfile << "\\begin{figure}[htb]" << endl;
  outfile << "\\includegraphics*[width= " << width <<"\\columnwidth]{"
                                          << figurename << "}" << endl;
  outfile << "\\caption{" << caption << "}" << endl;
  outfile << "\\end{figure}" << endl;
  outfile << endl;
  outfile.close();
}

void
FSString::latexLine(TString filename, TString text, bool append){
  std::ios_base::openmode mode = ios::out;
  if (append) mode = ios::app;
  ofstream outfile(TString2string(filename).c_str(),mode);
  outfile << endl;
  outfile << text << endl;
  outfile << endl;
  outfile.close();
}

void
FSString::latexCloser(TString filename, bool append){
  std::ios_base::openmode mode = ios::out;
  if (append) mode = ios::app;
  ofstream outfile(TString2string(filename).c_str(),mode);
  outfile << endl;
  outfile << "\\end{document}" << endl;
  outfile << endl;
  outfile.close();
}

void
FSString::writeTStringToFile(TString filename, TString text, bool append){
  std::ios_base::openmode mode = ios::out;
  if (append) mode = ios::app;
  ofstream outfile(TString2string(filename).c_str(),mode);
  outfile << text << endl;
  outfile.close();
}


  // ********************************************************
  // MAKE A LATEX TABLE
  //   pass in a 2d array with tableContents[nrows][ncols]
  //    or pass in a 1d array with tableContents[ncols*nrows]
  //      that fills left to right, top to bottom
  // ********************************************************

void
FSString::latexTable(int nrows, int ncols, TString* tableContents, 
                            TString filename, bool append){
  std::ios_base::openmode mode = ios::out;
  if (append) mode = ios::app;
  ofstream outfile(TString2string(filename).c_str(),mode);
  outfile << "\\begin{tabular}{|"; for (int k = 0; k < ncols; k++){ outfile << "c|"; } outfile << "}" << endl;
  outfile << "\\hline" << endl;
  int i = 0;
  for (int j = 0; j < nrows; j++){
    for (int k = 0; k < ncols; k++){
      outfile << tableContents[i++] << "  ";
      if (k != ncols - 1) outfile << "&  ";
    }
    outfile << "\\\\" << endl;
    outfile << "\\hline" << endl;
  }
  outfile << "\\end{tabular}" << endl;
  outfile << endl;
  outfile.close();
}  


  // ********************************************************
  // READ LINES FROM A FILE
  // ********************************************************

vector<TString>
FSString::readLinesFromFile(TString filename){
  vector<TString> lines;  string instring;
  ifstream infile(filename.Data());
  if (!infile){
    cout << "FSString::readLinesFromFile: can't find " << filename << endl;
    return lines;
  }
  while (getline(infile,instring)){ 
    lines.push_back(FSString::string2TString(instring)); 
  }
  infile.close();
  return lines;
}


  // *************************************************************
  // MAKE ALL PERMUTATIONS OF INPUTSTRING USING DEFINITIONS
  //   definitions has the format:
  //     NAME1 DEFN11 DEFN12 DEFN13
  //     NAME2 DEFN21 DEFN22 DEFN23 DEFN24
  //     NAME3 DEFN31 DEFN32
  // *************************************************************

vector<TString> 
FSString::expandDefinitions(TString inputString, 
                                   map< TString, vector<TString> > definitions,
                                   TString spacer, bool synchronizeDefinitions){

  vector<TString> expanded;
  expanded.push_back(inputString);


    // first order the definitions map from most to least fundamental
    //  (only makes a difference for synchronizeDefinitions = true)
    //  (use a vector to control the order)

  vector< pair<TString, vector<TString> > > defVector;
  while (!definitions.empty()){
    bool independent = true;
    for (map< TString, vector<TString> >::iterator mapItr = definitions.begin();
         mapItr != definitions.end(); mapItr++){
      independent = true;
      vector<TString> vMeanings = mapItr->second;
      for (unsigned int i = 0; i < vMeanings.size(); i++){
        if (definitions.find(vMeanings[i]) != definitions.end()){
          independent = false;
          break;
        }
      }
      if (independent){
        defVector.push_back(pair<TString,vector<TString> >(mapItr->first,mapItr->second));
        definitions.erase(mapItr);
        break;
      }
    }
    if (!independent){
      cout << "FSString::expandDefinitions...";
      cout << "check for circular definitions...  " << endl;
      return vector<TString>();
    }
  }


    // if using whitespace as the spacer, remove all tabs

  if (spacer == " "){
    for (unsigned int i = 0; i < defVector.size(); i++){
      defVector[i].first = FSString::removeTabs(defVector[i].first);
      for (unsigned int j = 0; j < defVector[i].second.size(); j++){
        defVector[i].second[j] = FSString::removeTabs(defVector[i].second[j]);
      }
    }
  }


    // expand definitions

  bool done = false;
  int iteration = 0;
  while (!done){
    done = true;

      // look for an element of "expanded" that contains a definition

    vector<TString>::iterator vIter;
    vector< pair< TString, vector<TString> > >::reverse_iterator mIter;
    for (vIter = expanded.begin(); 
          vIter != expanded.end(); vIter++){
      for (mIter = defVector.rbegin(); mIter != defVector.rend(); mIter++){
        TString element(spacer+*vIter+spacer);
        if (element.Contains(spacer+(*mIter).first+spacer)) break;
      }
      if (mIter != defVector.rend()) break;
    }

      // if one is found, remove it and add new elements with 
      //   expanded definitions, then repeat

    if (vIter != expanded.end()){
      done = false;
      TString element = *vIter;
      expanded.erase(vIter);
      for (unsigned int i = 0; i < (*mIter).second.size(); i++){
        TString newElement(element);
        if (!synchronizeDefinitions){
          TString newElementCopy(spacer+newElement+spacer);
          int index = newElementCopy.Index(spacer+(*mIter).first+spacer);
          newElement.Replace(index,(*mIter).first.Length(), (*mIter).second[i]);
          expanded.push_back(newElement);
        }
        if (synchronizeDefinitions){
          TString newElementCopy(spacer+newElement+spacer);
          while (newElementCopy.Contains(spacer+(*mIter).first+spacer)){
            int index = newElementCopy.Index(spacer+(*mIter).first+spacer);
            newElement.Replace(index,(*mIter).first.Length(), (*mIter).second[i]);
            newElementCopy = spacer+newElement+spacer;
          }
          expanded.push_back(newElement);
        }
      }
    }

      // infinite loop check

    if (++iteration > 10000){
      cout << "FSString::expandDefinitions...  ";
      cout << "quitting after 10000 iterations...  ";
      cout << "check for an infinite loop... " << endl;
      return vector<TString>();
    }

  }

  return expanded;

}


  // ********************************************************
  // PARSE HISTOGRAM BOUNDS
  // ********************************************************

int    
FSString::parseBoundsNBinsX(TString bounds){
  vector<TString> boundVector = parseBounds(bounds);
  if (boundVector.size() < 3) return 0;
  return TString2int(boundVector[0]);
}

double
FSString::parseBoundsLowerX(TString bounds){
  vector<TString> boundVector = parseBounds(bounds);
  if (boundVector.size() < 3) return 0.0;
  return TString2double(boundVector[1]);
}

double
FSString::parseBoundsUpperX(TString bounds){
  vector<TString> boundVector = parseBounds(bounds);
  if (boundVector.size() < 3) return 0.0;
  return TString2double(boundVector[2]);
}

int    
FSString::parseBoundsNBinsY(TString bounds){
  vector<TString> boundVector = parseBounds(bounds);
  if (boundVector.size() != 6) return 0;
  return TString2int(boundVector[3]);
}

double
FSString::parseBoundsLowerY(TString bounds){
  vector<TString> boundVector = parseBounds(bounds);
  if (boundVector.size() != 6) return 0.0;
  return TString2double(boundVector[4]);
}

double
FSString::parseBoundsUpperY(TString bounds){
  vector<TString> boundVector = parseBounds(bounds);
  if (boundVector.size() != 6) return 0.0;
  return TString2double(boundVector[5]);
}

double
FSString::parseBoundsBinSizeX(TString bounds){
  vector<TString> boundVector = parseBounds(bounds);
  if (boundVector.size() < 3) return 0;
  return (parseBoundsUpperX(bounds)-parseBoundsLowerX(bounds))/
          parseBoundsNBinsX(bounds);
}

vector<TString>
FSString::parseBounds(TString bounds){
  bounds = FSString::removeWhiteSpace(bounds);
  vector<TString> boundVector = parseTString(bounds,",");
  vector<TString> emptyVector;
  if ((boundVector.size() != 3 && boundVector.size() != 6) ||
      (boundVector[0].Index("(") != 0) ||
      (boundVector[boundVector.size()-1].Index(")") !=
       boundVector[boundVector.size()-1].Length()-1)){
    cout << "FSString: wrong histogram bounds format " << bounds << endl;
    return emptyVector;
  }
  boundVector[0].Replace(boundVector[0].Index("("),1,"");
  boundVector[boundVector.size()-1].Replace(boundVector[boundVector.size()-1].Index(")"),1,"");
  return boundVector;
}

bool
FSString::checkBounds(int dimension, TString bounds){
  bounds = FSString::removeWhiteSpace(bounds);
  vector<TString> spacers; spacers.push_back(","); spacers.push_back("(");  spacers.push_back(")");
  vector<TString> parts = parseTString(bounds,spacers,true);
  if (dimension == 1 && parts.size() == 7){
    if ((parts[0] != "(") || (parts[6] != ")")) return false;
    if (!parts[1].IsFloat() || parts[1].Contains(".")) return false;
    if ((parts[2] != ",") || (parts[4] != ",")) return false;
    if (!parts[3].IsFloat() || !parts[5].IsFloat()) return false;
  }
  else if (dimension == 2 && parts.size() == 13){
    if ((parts[0] != "(") || (parts[12] != ")")) return false;
    if (!parts[1].IsFloat() || parts[1].Contains(".")) return false;
    if (!parts[7].IsFloat() || parts[7].Contains(".")) return false;
    if ((parts[2] != ",") || (parts[4] != ",")) return false;
    if ((parts[6] != ",") || (parts[8] != ",") || (parts[10] != ",")) return false;
    if (!parts[3].IsFloat() || !parts[5].IsFloat()) return false;
    if (!parts[9].IsFloat() || !parts[11].IsFloat()) return false;
  }
  else{ return false; }
  return true;
}

TString
FSString::makeBounds(int nbinsX, double lowerX, double upperX, int nbinsY, double lowerY, double upperY){
  TString newBounds("");
  if (nbinsY < 0)  newBounds = "(" + FSString::int2TString(nbinsX) + ","
                                   + FSString::double2TString(lowerX,8) + ","
                                   + FSString::double2TString(upperX,8) + ")";
  if (nbinsY >= 0) newBounds = "(" + FSString::int2TString(nbinsX) + ","
                                   + FSString::double2TString(lowerX,8) + ","
                                   + FSString::double2TString(upperX,8) + ","
                                   + FSString::int2TString(nbinsY) + ","
                                   + FSString::double2TString(lowerY,8) + ","
                                   + FSString::double2TString(upperY,8) + ")";
  return newBounds;
}

TString
FSString::makeBounds(int dimension){
  if (dimension == 1) return TString("(10,0.0,1.0)");
  if (dimension == 2) return TString("(10,0.0,1.0,10,0.0,1.0)");
  return TString("");
}

  // ********************************************************
  // STRIP EXTRA WHITESPACE AROUND STRINGS AND REMOVE TABS
  // ********************************************************

bool
FSString::containsWhiteSpace(TString input){
  for (int i = 0; i < input.Length(); i++){
    TString digit(input[i]);
    if (digit.IsWhitespace()) return true;
    if (digit == "\t") return true;
    if (digit == " ")  return true;
    //if (digit == "\ ") return true;
  }
  return false;
}

TString
FSString::stripWhiteSpace(TString input){
  TString output = input;
  while ((output.Length() > 0) && (containsWhiteSpace(output[0]))){
    TString tempstring("");
    for (int i = 1; i < output.Length(); i++){ tempstring += output[i]; }
    output = tempstring;
  }
  while ((output.Length() > 0) && (containsWhiteSpace(output[output.Length()-1]))){
    TString tempstring("");
    for (int i = 0; i < output.Length()-1; i++){ tempstring += output[i]; }
    output = tempstring;
  }
  return output;
}

TString
FSString::removeTabs(TString input){
  TString output("");
  for (int i = 0; i < input.Length(); i++){
    TString digit(input[i]);
    if (containsWhiteSpace(digit)){ output += " "; }
    else{ output += digit; }
  }
  return output;
}

TString
FSString::removeWhiteSpace(TString input){
  TString output("");
  for (int i = 0; i < input.Length(); i++){
    TString digit(input[i]);
    if (!containsWhiteSpace(digit)) output += digit;
  }
  return output;
}


  // ********************************************************
  // CHECK OPENING AND CLOSING PARENTHESES
  // ********************************************************

bool
FSString::checkParentheses(TString input, TString opening, TString closing){
  int pcount = 0;
  for (int i = 0; i < input.Length(); i++){
    if (FSString::subString(input,i,i+opening.Length()) == opening) pcount++;
    if (FSString::subString(input,i,i+closing.Length()) == closing) pcount--;
    if (pcount < 0){
      cout << "FSString: problem with parentheses in input = " << input << endl;
      return false;
    }
  }
  if (pcount == 0) return true;
  cout << "FSString: problem with parentheses in input = " << input << endl;
  return false;
}


TString
FSString::expandSUM(TString inputString){
  inputString = FSString::removeWhiteSpace(inputString);
  if (!FSString::checkParentheses(inputString,"{","}")){
    cout << "FSString::expandSUM ERROR: problem with curly brackets" << endl;
    exit(0);
  }
  while (inputString.Contains("SUM{")){
    for (int i = 0; i < inputString.Length()-4; i++){
      if (FSString::subString(inputString,i,i+4) == "SUM{"){
        TString word1 = captureParentheses(inputString,i+3,"{","}");
        TString word2 = captureParentheses(inputString,i+4+word1.Length()+1,"{","}");
        if (((TString)inputString[i+4+word1.Length()+1] != "{") ||
            (word1 == "") || (word2 == "")){ 
          cout << "FSString::expandSUM ERROR: formatting problem" << endl;
          exit(0);
        }
        TString bigword1 = "SUM{" + word1 + "}";
        TString bigword2 =    "{" + word2 + "}";
        if ((!word1.Contains("SUM{")) && (!word2.Contains("SUM{"))){
          vector<TString> words1 = FSString::parseTString(word1,":");
          if (words1.size() != 2){
            cout << "FSString::expandSUM ERROR: no variable definition" << endl;
            exit(0);
          }
          TString name = words1[0];
          vector<TString> defns = FSString::parseTString(words1[1],",");
          TString newword2("");
          for (unsigned int id = 0; id < defns.size(); id++){
            TString word2temp = word2;
            while (word2temp.Contains(name)){
              word2temp.Replace(word2temp.Index(name),name.Length(),defns[id]);
            }
            newword2 += word2temp;
            if (id != defns.size()-1) newword2 += "+";
          }
          inputString.Replace(inputString.Index(bigword1),bigword1.Length(),"");
          inputString.Replace(inputString.Index(bigword2),bigword2.Length(),"("+newword2+")");
          break;
        }
      }
    }
  }
  return inputString;
}

