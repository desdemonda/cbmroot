#ifndef XMATH_H
#define XMATH_H

namespace xMath {
   inline double Pi()       { return 3.14159265358979323846; }
   inline double GeVtoifm() { return 1./0.197;}//5.06773; }

   // Bessel functions
   double BesselI(int n,double x);  // integer order modified Bessel function I_n(x)
   double BesselK(int n,double x);  // integer order modified Bessel function K_n(x)
   double BesselI0(double x);         // modified Bessel function I_0(x)
   double BesselK0(double x);         // modified Bessel function K_0(x)
   double BesselI1(double x);         // modified Bessel function I_1(x)
   double BesselK1(double x);         // modified Bessel function K_1(x)
   double BesselJ0(double x);         // Bessel function J0(x) for any real x
   double BesselJ1(double x);         // Bessel function J1(x) for any real x
   double BesselY0(double x);         // Bessel function Y0(x) for positive x
   double BesselY1(double x);         // Bessel function Y1(x) for positive x
   double StruveH0(double x);         // Struve functions of order 0
   double StruveH1(double x);         // Struve functions of order 1
   double StruveL0(double x);         // Modified Struve functions of order 0
   double StruveL1(double x);         // Modified Struve functions of order 1

   double BesselK0exp(double x);         // modified Bessel function K_0(x) without exponent
   double BesselK1exp(double x);         // modified Bessel function K_1(x) without exponent
   double BesselKexp(int n,double x);    // integer order modified Bessel function K_n(x) without exponent
}

#endif // XMATH_H
