#ifndef HAGEDORNSPECTRUM_H
#define HAGEDORNSPECTRUM_H
#include <string>
#include <vector>
#define PI 3.14159265359

class HagedornSpectrum
{
public:
	static const double GeVtoifm;
    double fM0, fTH, fmo, fC;
    HagedornSpectrum(double M0 = 2., double TH = 0.160, double mo = 0.500, double C = 1.):
                    fM0(M0), fTH(TH), fmo(mo), fC(C) 
					{
                    }
    ~HagedornSpectrum(void);
    
    double CalculateParticleDensity(double T, double muB, double mass=-1., double dMu = 0.) const; // if (mass<0) use default mass
    double CalculatePressure(double T, double muB, double mass=-1., double dMu = 0.) const; // if (mass<0) use default mass
    double CalculateEnergyDensity(double T, double muB, double mass=-1., double dMu = 0.) const; // if (mass<0) use default mass
    double CalculateEntropyDensity(double T, double muB, double mass=-1., double dMu = 0.) const; // if (mass<0) use default mass
    double CalculateParticleDensityLaguerre(double T, double muB, double mass=-1., double dMu = 0.) const; // if (mass<0) use default mass
    double CalculateDensity(double T, double muB, int type=0, double dMu = 0.) const; // type: 0 - Particle Density, 1 - Energy Density, 2 - Entropy Density, 3 - Pressure
};

#endif