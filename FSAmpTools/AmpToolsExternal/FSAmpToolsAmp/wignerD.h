#include <complex>
#include <math.h>
#include <cstdlib>
#include <cmath>
#include "GPUManager/GPUCustomTypes.h"

using std::complex;
using namespace std;

GDouble wignerDSmall( double aj, double am, double an, double beta );
complex< GDouble > wignerD( int l, int m, int n, double cosTheta, double phi );
complex< GDouble > wignerD_1( int l, int m, int n, double theta, double phi );
complex< GDouble > wignerD_1( int l, int m, double n, double theta, double phi );
complex< GDouble > wignerDC_1( int l, int m, int n, double theta, double phi ); 
complex< GDouble > wignerDC_1( int l, int m, double n, double theta, double phi );
complex< GDouble > wignerDC_1( double l, double m, double n, double theta, double phi );
complex< GDouble > wignerD( int l, int m, int n, double cosTheta, double phi1, double phi2 );
complex< GDouble > wignerDC( int l, int m, int n, double cosTheta, double phi1, double phi2 );
complex< GDouble > Y( int l, int m, double cosTheta, double phi );

