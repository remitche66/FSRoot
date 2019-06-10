#if !defined(FSXYPOINT_H)
#define FSXYPOINT_H

#include <utility>
#include <map>
#include <vector>
#include "TString.h"

using namespace std;

// NOTES: 
//  * all errors are absolute unless specified otherwise

class FSXYPoint{

  public:

    double      xValue()         { return m_xValue; }
    double      xValueLow()      { return m_xValueLow; }
    double      xValueHigh()     { return m_xValueHigh; }

    double      xError()         { return m_xError; }
    double      xErrorLow()      { return m_xErrorLow; }
    double      xErrorHigh()     { return m_xErrorHigh; }

    double      xErrorSyst()     { return m_xErrorSyst; }
    double      xErrorSystLow()  { return m_xErrorSystLow; }
    double      xErrorSystHigh() { return m_xErrorSystHigh; }

    double      yValue()         { return m_yValue; }
    double      yValueLow()      { return m_yValueLow; }
    double      yValueHigh()     { return m_yValueHigh; }

    double      yError()         { return m_yError; }
    double      yErrorLow()      { return m_yErrorLow; }
    double      yErrorHigh()     { return m_yErrorHigh; }

    double      yErrorSyst()     { return m_yErrorSyst; }
    double      yErrorSystLow()  { return m_yErrorSystLow; }
    double      yErrorSystHigh() { return m_yErrorSystHigh; }

    vector<TString> categories()  { return m_categories; }

    void display(int counter = -1);

    void setValuesFromString(TString sValues);

    FSXYPoint();

  private:

    void clear();
    bool hasCategory(TString cat);
    void addCategory(TString cat);

    void setXV(double val);    double  m_xValue;
    void setXVL(double val);   double  m_xValueLow;
    void setXVH(double val);   double  m_xValueHigh;

    double  m_xError;
    double  m_xErrorLow;
    double  m_xErrorHigh;

    double  m_xErrorSyst;
    double  m_xErrorSystLow;
    double  m_xErrorSystHigh;

    double  m_yValue;
    double  m_yValueLow;
    double  m_yValueHigh;

    double  m_yError;
    double  m_yErrorLow;
    double  m_yErrorHigh;

    double  m_yErrorSyst;
    double  m_yErrorSystLow;
    double  m_yErrorSystHigh;

    vector<TString> m_categories;

    friend class FSXYPointList;



};


#endif
