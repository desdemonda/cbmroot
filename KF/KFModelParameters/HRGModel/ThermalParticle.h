#ifndef THERMALPARTICLE_H
#define THERMALPARTICLE_H
#include <string>
#include <vector>
#define PI 3.14159265359

struct ParticleDecay
{
    double fBratio;
    std::vector<int> fDaughters;
    ParticleDecay(double bratio=0., std::vector<int> daughters=std::vector<int>(0)):fBratio(bratio), fDaughters(daughters) { }
};

class ThermalParticle
{
public:
    static const double GeVtoifm;
    bool fStable;
    std::string fName;
    int fPDGID;
    int fSpinDegeneracy;
    int fStatistics;
    double fMass;
    int fS;
    int fB;
    int fC;
    int fCharm;
    double fAbsS;
    double fAbsC;
    double fWidth;
    double fThreshold;
    double fRadius;
    double GetVo() {
        return 4. * 4. / 3. * PI * fRadius * fRadius * fRadius;
    }
    std::string fDecayName;
    std::vector<ParticleDecay> fDecays;
    std::vector<ParticleDecay> fDecaysOrig;
    std::vector< std::pair<double, int> > ResonanceBR;
    ThermalParticle(bool Stable_=true, std::string Name="hadron", int PDGID=0, int SpinDeg=1, int Stat=0, double Mass=0.,
                    int Strange=0, int Baryon=0, int Charge=0, double AbsS=0., double Width=0., double Threshold=0., std::string DecayName="pi0", int Charm = 0, double AbsC = 0., double radius = 0.5):
                    fStable(Stable_), fName(Name), fPDGID(PDGID), fSpinDegeneracy(SpinDeg), fStatistics(Stat), fMass(Mass),
                    fS(Strange), fB(Baryon), fC(Charge), fCharm(Charm), fAbsS(AbsS), fAbsC(AbsC), fWidth(Width), fThreshold(Threshold), fRadius(radius), fDecayName(DecayName), fDecays(), fDecaysOrig(), ResonanceBR() {
                        ResonanceBR.resize(0);
                        //fWidth = 1e-10;
                    }
    ~ThermalParticle(void);
    void ReadDecays(std::string filename="");
    void SetDecays(const std::vector<ParticleDecay> &Decays) {
        fDecays = Decays;
    }
    void useStatistics(bool enable) {
        if (!enable) fStatistics = 0;
        else {
            if (fB==0) fStatistics = -1;
            else fStatistics = 1;
        }
    }

    void NormalizeBranchingRatios();
    void RestoreBranchingRatios() { fDecays = fDecaysOrig; }
    bool IsMajorana() const {
        if (fName.substr(0,3)=="K0S" || fName.substr(0,3)=="K0L") return 0;
        return (fB==0 && fC==0 && fS==0 && fCharm==0);
    }
    double CalculateParticleDensity(double T, double muB, double muS, double muQ, double gammaS, double mass=-1., double dMu = 0.) const; // if (mass<0) use default mass
    double CalculatePressure(double T, double muB, double muS, double muQ, double gammaS, double mass=-1., double dMu = 0.) const; // if (mass<0) use default mass
    double CalculateEnergyDensity(double T, double muB, double muS, double muQ, double gammaS, double mass=-1., double dMu = 0.) const; // if (mass<0) use default mass
    double CalculateEntropyDensity(double T, double muB, double muS, double muQ, double gammaS, double mass=-1., double dMu = 0.) const; // if (mass<0) use default mass
    double CalculateParticleDensityLaguerre(double T, double muB, double muS, double muQ, double gammaS, double mass=-1., double dMu = 0.) const; // if (mass<0) use default mass
    double CalculateDensity(double T, double muB, double muS, double muQ, double gammaS, int type=0, bool useWidth=0, double dMu = 0.) const; // type: 0 - Particle Density, 1 - Energy Density, 2 - Entropy Density, 3 - Pressure
};

#endif
