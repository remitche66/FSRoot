#if !defined(FSXYPOINT_H)
#define FSXYPOINT_H

#include <utility>
#include <map>
#include <vector>
#include <cmath>
#include "TString.h"

using namespace std;

// NOTES: 
//  * all errors are absolute unless specified otherwise

class FSXYPoint{

  public:

    double      xValue()         { return m_XV; }
    double      xValueLow()      { return m_XVL; }
    double      xValueHigh()     { return m_XVH; }
    TString     xLabel()         { return m_XL; }
    double      xError()         { return (m_XEL + m_XEH)/2.0; }
    double      xErrorLow()      { return m_XEL; }
    double      xErrorHigh()     { return m_XEH; }
    double      xErrorSyst()     { return (m_XESL + m_XESH)/2.0; }
    double      xErrorSystLow()  { return m_XESL; }
    double      xErrorSystHigh() { return m_XESH; }
    double      xErrorTot()      { return sqrt(xError()    *xError()     + xErrorSyst()    *xErrorSyst()); }
    double      xErrorTotLow()   { return sqrt(xErrorLow() *xErrorLow()  + xErrorSystLow() *xErrorSystLow()); }
    double      xErrorTotHigh()  { return sqrt(xErrorHigh()*xErrorHigh() + xErrorSystHigh()*xErrorSystHigh()); }
    double      yValue()         { return m_YV; }
    double      yError()         { return (m_YEL + m_YEH)/2.0; }
    double      yErrorLow()      { return m_YEL; }
    double      yErrorHigh()     { return m_YEH; }
    double      yErrorSyst()     { return (m_YESL + m_YESH)/2.0; }
    double      yErrorSystLow()  { return m_YESL; }
    double      yErrorSystHigh() { return m_YESH; }
    double      yErrorTot()      { return sqrt(yError()    *yError()     + yErrorSyst()    *yErrorSyst()); }
    double      yErrorTotLow()   { return sqrt(yErrorLow() *yErrorLow()  + yErrorSystLow() *yErrorSystLow()); }
    double      yErrorTotHigh()  { return sqrt(yErrorHigh()*yErrorHigh() + yErrorSystHigh()*yErrorSystHigh()); }

    vector<TString> categories()  { return m_categories; }

    void display(int counter = -1);


  private:

    FSXYPoint();
    FSXYPoint(TString category, double scale, FSXYPoint* xyp); 
    FSXYPoint(TString category, TString operation, FSXYPoint* xyp1, FSXYPoint* xyp2); 

    void clear();
    void setValuesFromString(TString sValues);
    bool hasCategory(TString cat);
    void addCategory(TString cat);

    void setXV  (double  val) { m_XV = val; }
    void setXV  ()            { if (m_XVH > m_XVL){ setXV((m_XVH+m_XVL)/2.0); } 
                                              else{ setXVL(m_XV); setXVH(m_XV); } }
    void setXVL (double  val) { m_XVL = val; }
    void setXVH (double  val) { m_XVH = val; }
    void setXL  (TString lab) { m_XL = lab; }  
    void setXE  (double  val) { setXEL(val); setXEH(val); }
    void setXEL (double  val) { m_XEL = sqrt(m_XEL*m_XEL + val*val); }
    void setXEH (double  val) { m_XEH = sqrt(m_XEH*m_XEH + val*val); }
    void setXES (double  val) { setXESL(val); setXESH(val); }
    void setXESL(double  val) { m_XESL = sqrt(m_XESL*m_XESL + val*val); }
    void setXESH(double  val) { m_XESH = sqrt(m_XESH*m_XESH + val*val); }
    void setYV  (double  val) { m_YV = val; }
    void setYE  (double  val) { setYEL(val); setYEH(val); }
    void setYEL (double  val) { m_YEL = sqrt(m_YEL*m_YEL + val*val); }
    void setYEH (double  val) { m_YEH = sqrt(m_YEH*m_YEH + val*val); }
    void setYES (double  val) { setYESL(val); setYESH(val); }
    void setYESL(double  val) { m_YESL = sqrt(m_YESL*m_YESL + val*val); }
    void setYESH(double  val) { m_YESH = sqrt(m_YESH*m_YESH + val*val); }

    double  m_XV;
    double  m_XVL;
    double  m_XVH;
    TString m_XL; 
    double  m_XEL;
    double  m_XEH;
    double  m_XESL;
    double  m_XESH;
    double  m_YV;
    double  m_YEL;
    double  m_YEH;
    double  m_YESL;
    double  m_YESH;

    vector<TString> m_categories;

    friend class FSXYPointList;

};


#endif
