#if !defined(FSMATH_H)
#define FSMATH_H

#include <cmath>

using namespace std;


class FSMath{

  public:

    static double max();
    static double max(double x1);
    static double max(double x1, double x2);
    static double max(double x1, double x2, double x3);
    static double max(double x1, double x2, double x3, double x4);
    static double max(double x1, double x2, double x3, double x4, double x5);
    static double max(double x1, double x2, double x3, double x4, double x5, double x6);
    static double max(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7);
    static double max(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7, double x8);
    static double max(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7, double x8, double x9);
    static double max(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7, double x8, double x9, double x10);

    static double min();
    static double min(double x1);
    static double min(double x1, double x2);
    static double min(double x1, double x2, double x3);
    static double min(double x1, double x2, double x3, double x4);
    static double min(double x1, double x2, double x3, double x4, double x5);
    static double min(double x1, double x2, double x3, double x4, double x5, double x6);
    static double min(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7);
    static double min(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7, double x8);
    static double min(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7, double x8, double x9);
    static double min(double x1, double x2, double x3, double x4, double x5, double x6,
                      double x7, double x8, double x9, double x10);

};


#endif
