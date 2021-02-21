#include "BarrierFactor.h"
#include <cassert>
#include <iostream>
#include <string>
#include <complex>
#include <cstdlib>

using namespace std;

const double BarrierFactor::qRDefault = 0.1973; // GeV

BarrierFactor::BarrierFactor(int l, std::complex<double> q0, double qR) :
   _l(l),
   _qR(qR),
   _q0(q0)
{
   _B0 = BlattWeisskopf(_q0);
   if (abs(_B0)<1.E-8){
     if(_B0.real()>0) _B0+=std::complex<double>(1.E-8,0.);
     else _B0+=std::complex<double>(0.,1.E-8);
   }
}

std::complex<double> BarrierFactor::BlattWeisskopf(std::complex<double> q){
   std::complex<double> z0(std::norm(q)/std::norm(_qR));
   return BlattWeisskopf(_l, z0);
//   return BlattWeisskopf(_l, q*q/_qR/_qR);
}



std::complex<double> BarrierFactor::BlattWeisskopf(int l, std::complex<double> q, double qR){
   std::complex<double> z(std::norm(q)/std::norm(qR));
   return BlattWeisskopf(l, z);
//   return BlattWeisskopf(l, q*q/qR/qR);
}

std::complex<double> BarrierFactor::BlattWeisskopfTensor(int l, std::complex<double> q, double qR){
   std::complex<double> z(std::norm(q)/std::norm(qR));
   return BlattWeisskopf(l, z)/pow(q,l);
//   return BlattWeisskopf(l, q*q/qR/qR)/pow(q,l);
}


std::complex<double> BarrierFactor::BlattWeisskopf(int l, std::complex<double> z){

   if(0==l){
     return std::complex<double>(1.0, 0.);
   }
   else if(1==l){
      return std::sqrt(2.*z / (z+1.));
   }
   else if(2==l){
      return sqrt(13.*z*z / (z*z + 3.*z + 9.));
   }
   else if(3==l){
      return sqrt(277.*z*z*z / (z*z*z + 6.*z*z + 45.*z + 225.));
   }
   else if(4==l){
      return sqrt(12746.*z*z*z*z /
                  (z*z*z*z + 10.*z*z*z + 135.*z*z + 1575.*z + 11025.));
   }
   else if(5==l){
      return sqrt(998881.*z*z*z*z*z /
                  (z*z*z*z*z + 15.*z*z*z*z + 315.*z*z*z +
                   6300.*z*z + 99225.*z + 893025.));
   }
   else if(6==l){
      return sqrt(118394977.*z*z*z*z*z*z /
                  (z*z*z*z*z*z + 21.*z*z*z*z*z + 630.*z*z*z*z + 18900.*z*z*z +
                   496125.*z*z + 9823275.*z + 108056025.));
   }
   else if(7==l){
      return sqrt(19727003738.*z*z*z*z*z*z*z /
                  (z*z*z*z*z*z*z + 28.*z*z*z*z*z*z + 1134.*z*z*z*z*z + 47250.*z*z*z*z +
                   1819125.*z*z*z + 58939650.*z*z + 1404728325.*z + 18261468225.));
   }
   else if(8>=l){  //attention work-around for l>=9
      return sqrt(4392846440677.*z*z*z*z*z*z*z*z /
                  (z*z*z*z*z*z*z*z + 36.*z*z*z*z*z*z*z + 1890.*z*z*z*z*z*z +
                   103950.*z*z*z*z*z + 5457375.*z*z*z*z + 255405150.*z*z*z +
                   9833098275.*z*z + 273922023375.*z + 4108830350625.));
   }
   else{
      cout << "Cannot compute BlattWeisskopf factor for l=" << l << endl;
   }
   return 0;
}




std::complex<double> BarrierFactor::BlattWeisskopfRatio(std::complex<double> q){
   return BlattWeisskopf(q) / _B0;
}

std::complex<double> BarrierFactor::BlattWeisskopfRatio(int l, std::complex<double> q, std::complex<double> q0, double qR){
  std::complex<double> denom=BlattWeisskopf(l, q0, qR);
  if (std::abs(denom)<1.E-10){
    if(denom.real()>0.) denom+=std::complex<double>(1.E-10,0.);
    else denom+=std::complex<double>(0.,1.E-10);
  }
  return BlattWeisskopf(l, q, qR) / denom;
  //   return BlattWeisskopf(l, q, qR) / BlattWeisskopf(l, q0, qR);
}

std::complex<double> BarrierFactor::BlattWeisskopfTensorRatio(int l, std::complex<double> q, std::complex<double> q0, double qR){
 std::complex<double> denom=BlattWeisskopfTensor(l, q0, qR);
  if (abs(denom)<1.E-10){
    if(denom.real()>0.) denom+=std::complex<double>(1.E-10,0.);
    else denom+=std::complex<double>(0.,1.E-10);
  }
  return BlattWeisskopfTensor(l, q, qR) / denom;
  //   return BlattWeisskopfTensor(l, q, qR) / BlattWeisskopfTensor(l, q0, qR);
}

