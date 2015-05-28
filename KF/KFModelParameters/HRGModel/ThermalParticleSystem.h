#ifndef THERMALPARTICLESYSTEM_H
#define THERMALPARTICLESYSTEM_H
#include "ThermalParticle.h"
#include <map>
#include <vector>

class ThermalParticleSystem
{
public:
    std::vector<ThermalParticle> fParticles;
    std::map<int,int> PDGtoID;
    std::vector<int> IDtoPDG;
    ThermalParticleSystem(std::string InputFile="");
    ~ThermalParticleSystem(void);
    std::vector<ParticleDecay> GetDecaysFromAntiParticle(const std::vector<ParticleDecay> &Decays) {
        std::vector<ParticleDecay> ret = Decays;
        for(unsigned int i=0;i<ret.size();++i) {
            for(unsigned int j=0;j<ret[i].fDaughters.size();++j) {
                if (PDGtoID.count(-ret[i].fDaughters[j])>0) ret[i].fDaughters[j] = -ret[i].fDaughters[j];
            }
        }
        return ret;
    }
    void FillResonanceDecays();
    void GoResonance(int ind, int startPDG, double BR);
    int NumberOfParticles;
    void LoadTHERMUSDatabase(std::string InputFile="");
    void LoadDatabase(std::string InputFile="");
    void NormalizeBranchingRatios() {
        for(unsigned int i=0;i<fParticles.size();++i) fParticles[i].NormalizeBranchingRatios();
        FillResonanceDecays();
    }
    void RestoreBranchingRatios() {
        for(unsigned int i=0;i<fParticles.size();++i) fParticles[i].RestoreBranchingRatios();
        FillResonanceDecays();
    }
    std::string GetNameFromPDG(int pdgid);
};

std::vector<std::string> split(const std::string &s, char delim);

#endif
