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
  TString output("");
  output += input;
  return output;
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
  // CONVERT STRINGS TO NUMBERS
  // ********************************************************


int
FSString::TString2int(TString input){
  if (input.IsFloat()){
    string sinput = TString2string(input);
    return (int) atof(sinput.c_str());
  }
  TString embeddedNumber("");
  for (int i = 0; i < input.Length(); i++){
    TString digit(input[i]);
    if (digit.IsFloat() || digit == ".")
      embeddedNumber += digit;
  }
  if (!embeddedNumber.IsFloat()) return 0;
  string sinput = TString2string(embeddedNumber);
  return atoi(sinput.c_str());
}

double
FSString::TString2double(TString input){
  if (input.IsFloat()){
    string sinput = TString2string(input);
    return atof(sinput.c_str());
  }
  TString embeddedNumber("");
  for (int i = 0; i < input.Length(); i++){
    TString digit(input[i]);
    if (digit.IsFloat() || digit == ".")
      embeddedNumber += digit;
  }
  if (!embeddedNumber.IsFloat()) return 0.0;
  string sinput = TString2string(embeddedNumber);
  return atof(sinput.c_str());
}  


  // ********************************************************
  // CONVERT FROM INT TO TSTRING
  // ********************************************************

TString 
FSString::int2TString(int number, int ndigits){
  TString snumber("");  snumber += number;
  TString newsnumber("");
  for (int i = snumber.Length(); i < ndigits; i++){ newsnumber += "0"; }
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


TString 
FSString::double2TString(double x, int precision, bool scientific, bool fixdecimal){

  if (fixdecimal) return matchPrecision(x,pow(10.0,precision),1,scientific);

  if (precision == 0){
    if (x >=  5*pow(10.0,getExponent(x,1))) x += 5*pow(10.0,getExponent(x,1));
    if (x <= -5*pow(10.0,getExponent(x,1))) x -= 5*pow(10.0,getExponent(x,1));
    precision = 1;
  }

  stringstream xstream;
  xstream.precision(precision-1);
  xstream.setf(ios::scientific);
  xstream << x;

  TString snumber = FSString::string2TString(xstream.str());

  if (scientific) return snumber;

  int iexponent = getExponent(snumber);

  snumber.Replace(snumber.Index("e"),4,"");

  TString sign("");
  if (snumber.Contains("-")){
    sign = "-";
    snumber.Replace(snumber.Index("-"),1,"");
  }

  while (iexponent > 0){
    if (snumber.Contains(".")){
      int decimal = snumber.Index(".");
      snumber.Replace(decimal,1,"");
      if (decimal < snumber.Length()){
        snumber.Replace(decimal+1,0,".");
      }
      else{
        snumber.Append("0");
      }
    }
    else{
      snumber.Append("0");
    }
    iexponent--;
  }

  while (iexponent < 0){
    if (snumber.Contains(".")){
      snumber.Replace(snumber.Index("."),1,"");
    }
    snumber = ("0."+snumber);
    iexponent++;
  }


  if (snumber.Contains(".") && snumber.Index(".") == snumber.Length()-1)
    snumber.Replace(snumber.Index("."),1,"");

  return (sign+snumber);

}



TString 
FSString::latexMeasurement(double x, double ex1, 
                                  int precision, bool fixdecimal){

  double ex = 0.0;
  if (!fixdecimal) ex = ex1;
  if (fixdecimal) {ex = pow(10.0,precision); precision = 1;}

  TString sx  = matchPrecision(  x,ex,precision,false);
  TString sex = matchPrecision(ex1,ex,precision,false);

  return (sx+"\\pm"+sex);

}


TString 
FSString::latexMeasurement(double x, TString sign1, double ex1, 
                                   TString sign2, double ex2, 
                                  int precision, bool fixdecimal){

  double ex = 0.0;
  if (!fixdecimal) ex = getLeastPrecise(ex1,ex2,precision);
  if (fixdecimal) {ex = pow(10.0,precision); precision = 1;}

  TString sx    = matchPrecision( x, ex,precision,false);
  TString sex1  = matchPrecision(ex1,ex,precision,false);
  TString sex2  = matchPrecision(ex2,ex,precision,false);

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

  double ex = 0.0;
  if (!fixdecimal) ex = getLeastPrecise(ex1,ex2,ex3,precision);
  if (fixdecimal) {ex = pow(10.0,precision); precision = 1;}

  TString sx    = matchPrecision( x, ex,precision,false);
  TString sex1  = matchPrecision(ex1,ex,precision,false);
  TString sex2  = matchPrecision(ex2,ex,precision,false);
  TString sex3  = matchPrecision(ex3,ex,precision,false);

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

  double ex = 0.0;
  if (!fixdecimal) ex = getLeastPrecise(ex1,ex2,ex3,ex4,precision);
  if (fixdecimal) {ex = pow(10.0,precision); precision = 1;}

  TString sx    = matchPrecision( x, ex,precision,false);
  TString sex1  = matchPrecision(ex1,ex,precision,false);
  TString sex2  = matchPrecision(ex2,ex,precision,false);
  TString sex3  = matchPrecision(ex3,ex,precision,false);
  TString sex4  = matchPrecision(ex4,ex,precision,false);

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



int 
FSString::getExponent(TString input){

  if (!input.Contains("e") || input.Length() < 4){
    cout << "problem in FSString::getExponent" << endl;
    return 0;
  }

  TString sexponent(""); 
  if (input[input.Length()-3] == '-')
    sexponent += input[input.Length()-3];
  if (input[input.Length()-2] != '0')
    sexponent += input[input.Length()-2];
  sexponent += input[input.Length()-1];

  return FSString::TString2int(sexponent);

}



int 
FSString::getExponent(double x, int precision){

  stringstream xstream;
  xstream.precision(precision-1);
  xstream.setf(ios::scientific);
  xstream << x;

  TString snumber = FSString::string2TString(xstream.str());

  return getExponent(snumber);  

}



TString 
FSString::matchPrecision(double x, double reference, int precision, bool scientific){

  TString sx   = double2TString(x,15,true);
  TString sref = double2TString(reference,precision,true);

  int xprecision = getExponent(sx)-getExponent(sref)+precision;

  if (xprecision >= 0){
    if (getExponent(double2TString(x,xprecision,true)) > getExponent(sx)){
      x = FSString::TString2double(double2TString(x,xprecision,true));
      xprecision++;
    }
  }

  if (xprecision > 0){
    return double2TString(x,xprecision,scientific);
  }

  return double2TString(0.0,1,scientific);

}


double 
FSString::getLeastPrecise(double x1, double x2, int precision){
  double x = x1;
  if (getExponent(double2TString(x, precision,true)) < 
      getExponent(double2TString(x2,precision,true))) x = x2;
  return x;
}

double 
FSString::getLeastPrecise(double x1, double x2, double x3, int precision){
  double x = x1;
  if (getExponent(double2TString(x, precision,true)) < 
      getExponent(double2TString(x2,precision,true))) x = x2;
  if (getExponent(double2TString(x, precision,true)) < 
      getExponent(double2TString(x3,precision,true))) x = x3;
  return x;
}

double 
FSString::getLeastPrecise(double x1, double x2, double x3, double x4, int precision){
  double x = x1;
  if (getExponent(double2TString(x, precision,true)) < 
      getExponent(double2TString(x2,precision,true))) x = x2;
  if (getExponent(double2TString(x, precision,true)) < 
      getExponent(double2TString(x3,precision,true))) x = x3;
  if (getExponent(double2TString(x, precision,true)) < 
      getExponent(double2TString(x4,precision,true))) x = x4;
  return x;
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
FSString::parseString(string input, string spacer){
  return parseTString(string2TString(input),string2TString(spacer));
}

vector<TString>
FSString::parseTString(TString input, TString spacer){
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
    if (!s2.Contains(spacer) && (s2.Length() > 0)) words.push_back(s2);
    input = s2;
  }
  if (words.size() == 0) words.push_back(input);
  return words;
}    

vector<TString>
FSString::parseTString(TString input, vector<TString> spacers){
  input = FSString::removeTabs(input);
  vector<TString> words;
  words.push_back(input);
  for (unsigned int ispacer = 0; ispacer < spacers.size(); ispacer++){
    vector<TString> newWords;
    for (unsigned int iword = 0; iword < words.size(); iword++){
      vector<TString> tempWords = parseTString(words[iword],spacers[ispacer]);
      for (unsigned int i = 0; i < tempWords.size(); i++){ newWords.push_back(tempWords[i]); }
    }
    words = newWords;
  }
  return words;
}


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
      // PARSE VERY SIMPLE LOGIC 
      //    evalLogicalTString:
      //      Determines whether or not a list of categories contains
      //        what is in the input logic.
      //      For example, for categories = "A","B","C",
      //        input = "A&B" --> true
      //        input = "A&B&!C" --> false
      //        input = "A&D" --> false
      //        input = "D,!E" --> true
      //      (parentheses are allowed)
      // ********************************************************


bool
FSString::evalLogicalTString(TString input, vector<TString> cats){
  input = FSString::removeWhiteSpace(input);
  if (input == "") return true;
  if (!FSString::checkParentheses(input)){
    cout << "FSString::evalLogicalTString ERROR: parentheses problem in " << input << endl;
    exit(1);
  }
  vector<TString> spacers = {"(",")",",","&","|","!"};
  vector<TString> words = FSString::parseTString(input,spacers);
  for (unsigned int i = 0; i < words.size(); i++){
  for (unsigned int j = i+1; j < words.size(); j++){
    if (words[i].Length() < words[j].Length()){ 
      TString temp = words[i]; words[i] = words[j]; words[j] = temp; }
  }}
  for (unsigned int i = 0; i < words.size(); i++){
    TString found("0");
    for (unsigned int ic = 0; ic < cats.size(); ic++){
      if (cats[ic] == words[i]){ found = "1";  break; }
    }
    while (input.Contains(words[i])){
      input.Replace(input.Index(words[i]),words[i].Length(),found);
    }
  }
  input = ("("+input+")");
  return evalBooleanTString(input);
}



bool 
FSString::evalBooleanTString(TString input){
/*
  TFormula f("ftemp",input);
  double y = f.Eval(0);
  if (y < 0.1) return false;
  return true;
*/
cout << "in here with " << input << endl;
  while (input.Contains("()"))   { input.Replace(input.Index("()"),   2,""); }
  while (input.Contains("!!"))   { input.Replace(input.Index("!!"),   2,""); }
  while (input.Contains("(0)"))  { input.Replace(input.Index("(0)"),  3,"0"); }
  while (input.Contains("(1)"))  { input.Replace(input.Index("(1)"),  3,"1"); }
  while (input.Contains("!0") || 
         input.Contains("!1")){    
     if (input.Contains("!0"))     input.Replace(input.Index("!0"),   2,"1");
     if (input.Contains("!1"))     input.Replace(input.Index("!1"),   2,"0"); }
  while (input.Contains("0&0"))  { input.Replace(input.Index("0&0"),  3,"0"); }
  while (input.Contains("0&1"))  { input.Replace(input.Index("0&1"),  3,"0"); }
  while (input.Contains("1&0"))  { input.Replace(input.Index("1&0"),  3,"0"); }
  while (input.Contains("1&1"))  { input.Replace(input.Index("1&1"),  3,"1"); }
  while (input.Contains("(0,0)")){ input.Replace(input.Index("(0,0)"),5,"0"); }
  while (input.Contains("(0,1)")){ input.Replace(input.Index("(0,1)"),5,"1"); }
  while (input.Contains("(1,0)")){ input.Replace(input.Index("(1,0)"),5,"1"); }
  while (input.Contains("(1,1)")){ input.Replace(input.Index("(1,1)"),5,"1"); }
  while (input.Contains(",0,0)")){ input.Replace(input.Index(",0,0)"),5,",0)"); }
  while (input.Contains(",0,1)")){ input.Replace(input.Index(",0,1)"),5,",1)"); }
  while (input.Contains(",1,0)")){ input.Replace(input.Index(",1,0)"),5,",1)"); }
  while (input.Contains(",1,1)")){ input.Replace(input.Index(",1,1)"),5,",1)"); }
  while (input.Contains("(0,0,")){ input.Replace(input.Index("(0,0,"),5,"(0,"); }
  while (input.Contains("(0,1,")){ input.Replace(input.Index("(0,1,"),5,"(1,"); }
  while (input.Contains("(1,0,")){ input.Replace(input.Index("(1,0,"),5,"(1,"); }
  while (input.Contains("(1,1,")){ input.Replace(input.Index("(1,1,"),5,"(1,"); }
  if (FSControl::DEBUG){ cout << "FSString::evalBooleanTString  (p) " << input << endl; }
  if (!(input.Contains("0") || input.Contains("1")) || (input.Length() == 0) ||
       (input.Contains(",,")) || (input.Contains("!,")) || (input.Contains("&&"))){ 
    cout << "FSString::evalBooleanTString ERROR..." << input << endl; exit(1); }
  if ((input != "0") && (input != "1")) return evalBooleanTString(input);
  if (input == "1") return true;
  return false;
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
  // READ A TSTRING FROM A FILE
  // ********************************************************

TString
FSString::readTStringFromFile(TString filename, int line, int word){
  ifstream infile(filename.Data());
  if (!infile) cout << "can't find " << filename << endl;
  string instring;
  for (int i = 0; i < line; i++){ getline(infile,instring); }
  infile.close();
  vector<TString> words = parseString(instring);
  if (word >= (int)words.size()) return TString("");
  if (word < -1*(int)words.size()) return TString("");
  if (word < 0) word += (int)words.size();
  return words[(unsigned int)word];
}

TString
FSString::readTStringLineFromFile(TString filename, int line){
  ifstream infile(filename.Data());
  if (!infile) cout << "can't find " << filename << endl;
  string instring;
  for (int i = 0; i < line; i++){ getline(infile,instring); }
  infile.close();
  return instring;
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

