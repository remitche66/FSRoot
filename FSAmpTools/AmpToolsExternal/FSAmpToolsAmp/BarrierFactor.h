#if !defined(BARRIERFACTOR)
#define BARRIERFACTOR

#include <cmath>
#include <complex>

#pragma once

class BarrierFactor
{

public:
   BarrierFactor(int l, std::complex<double> q0, double qR);
   std::complex<double> BlattWeisskopfRatio(std::complex<double> q);
   std::complex<double> BlattWeisskopf(std::complex<double> q);
   static std::complex<double> BlattWeisskopfRatio(int l, std::complex<double> q, std::complex<double> q0, double qR);
   static std::complex<double> BlattWeisskopfTensorRatio(int l, std::complex<double> q, std::complex<double> q0, double qR);
   static std::complex<double> BlattWeisskopf(int l, std::complex<double> q, double qR);
   static std::complex<double> BlattWeisskopf(int l, std::complex<double> z);
   static std::complex<double> BlattWeisskopfTensor(int l, std::complex<double> q, double qR);
   static const double qRDefault;

private:
   double _l;
   double _qR;
   std::complex<double> _q0;
   std::complex<double> _B0;
 };


#endif
