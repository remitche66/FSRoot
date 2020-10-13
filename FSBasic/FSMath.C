#include <iostream>
#include <vector>
#include <utility>
#include "FSBasic/FSMath.h"


double 
FSMath::max(){ 
  return 0.0;
}

double 
FSMath::max(double x1){ 
  return x1;
}

double 
FSMath::max(double x1, double x2){ 
  return x2 > x1 ? x2 : x1;
}

double 
FSMath::max(double x1, double x2, double x3){ 
  double x0 = max(x1,x2);
  return x3 > x0 ? x3 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4){ 
  double x0 = max(x1,x2,x3);
  return x4 > x0 ? x4 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4, double x5){ 
  double x0 = max(x1,x2,x3,x4);
  return x5 > x0 ? x5 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4, double x5, double x6){ 
  double x0 = max(x1,x2,x3,x4,x5);
  return x6 > x0 ? x6 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7){ 
  double x0 = max(x1,x2,x3,x4,x5,x6);
  return x7 > x0 ? x7 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7, double x8){ 
  double x0 = max(x1,x2,x3,x4,x5,x6,x7);
  return x8 > x0 ? x8 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7, double x8, double x9){ 
  double x0 = max(x1,x2,x3,x4,x5,x6,x7,x8);
  return x9 > x0 ? x9 : x0;
}

double 
FSMath::max(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7, double x8, double x9, double x10){ 
  double x0 = max(x1,x2,x3,x4,x5,x6,x7,x8,x9);
  return x10 > x0 ? x10 : x0;
}

double 
FSMath::min(){ 
  return 0.0;
}

double 
FSMath::min(double x1){ 
  return x1;
}

double 
FSMath::min(double x1, double x2){ 
  return x2 < x1 ? x2 : x1;
}

double 
FSMath::min(double x1, double x2, double x3){ 
  double x0 = min(x1,x2);
  return x3 < x0 ? x3 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4){ 
  double x0 = min(x1,x2,x3);
  return x4 < x0 ? x4 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4, double x5){ 
  double x0 = min(x1,x2,x3,x4);
  return x5 < x0 ? x5 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4, double x5, double x6){ 
  double x0 = min(x1,x2,x3,x4,x5);
  return x6 < x0 ? x6 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7){ 
  double x0 = min(x1,x2,x3,x4,x5,x6);
  return x7 < x0 ? x7 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7, double x8){ 
  double x0 = min(x1,x2,x3,x4,x5,x6,x7);
  return x8 < x0 ? x8 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7, double x8, double x9){ 
  double x0 = min(x1,x2,x3,x4,x5,x6,x7,x8);
  return x9 < x0 ? x9 : x0;
}

double 
FSMath::min(double x1, double x2, double x3, double x4, double x5, double x6,
            double x7, double x8, double x9, double x10){ 
  double x0 = min(x1,x2,x3,x4,x5,x6,x7,x8,x9);
  return x10 < x0 ? x10 : x0;
}
