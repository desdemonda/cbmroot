#include "ThermalParticleSystem.h"
#include <fstream>
#include <algorithm>
#include <queue>
#include <iostream>
#include <sstream>

using namespace std;

namespace ThermalParticleSystemNameSpace {
    bool cmpParticleMass(const ThermalParticle &a, const ThermalParticle &b) {
        return (a.fMass < b.fMass);
    }
}

ThermalParticleSystem::ThermalParticleSystem(std::string InputFile):
	fParticles(), PDGtoID(), IDtoPDG(), NumberOfParticles(0)
{
    // NumberOfParticles = 0;
    fParticles.resize(0);
    PDGtoID.clear();
    IDtoPDG.resize(0);
    if (InputFile.size()<4) return;
    if (InputFile.substr(InputFile.size()-4)==".txt") LoadTHERMUSDatabase(InputFile);
    else LoadDatabase(InputFile);
}


ThermalParticleSystem::~ThermalParticleSystem(void)
{
}

void ThermalParticleSystem::FillResonanceDecays() {
    for(unsigned int i=0;i<fParticles.size();++i) {
        fParticles[i].ResonanceBR.resize(0);
    }
    for(int i=fParticles.size()-1;i>=0;i--)
        if (!fParticles[i].fStable) {
            GoResonance(i, i, 1.);
        }
}

void ThermalParticleSystem::GoResonance(int ind, int startind, double BR) {
    if (ind!=startind && fParticles[ind].ResonanceBR.size()>0 && fParticles[ind].ResonanceBR[fParticles[ind].ResonanceBR.size()-1].second==startind)
    {
        fParticles[ind].ResonanceBR[fParticles[ind].ResonanceBR.size()-1].first += BR;
    }
    else if (ind!=startind) fParticles[ind].ResonanceBR.push_back(make_pair(BR, startind));
    if (!fParticles[ind].fStable) {
        for(unsigned int i=0;i<fParticles[ind].fDecays.size();++i)
            for(unsigned int j=0;j<fParticles[ind].fDecays[i].fDaughters.size();++j) {
                GoResonance(PDGtoID[fParticles[ind].fDecays[i].fDaughters[j]], startind, BR*fParticles[ind].fDecays[i].fBratio);
            }
    }
}

void ThermalParticleSystem::LoadTHERMUSDatabase(std::string InputFile) {
    NumberOfParticles = 0;
    ifstream fin(InputFile.c_str());
    fParticles.resize(0);
    PDGtoID.clear();
    IDtoPDG.resize(0);
    string decayprefix = "";
    for(int i=InputFile.size()-1;i>=0;--i)
        if (InputFile[i]=='\\' || InputFile[i]=='/')
        {
            decayprefix = InputFile.substr(0, i+1);
            break;
        }
    if (fin.is_open()) {
        int stable, pdgid, spin, stat, str, bary, chg;
        double mass, width, threshold, abss;
        string name, decayname;
        while (fin >> stable) {
            fin >> name >> pdgid >> spin >> stat >> mass >> str >> bary >> chg >> abss >> width >> threshold;
            if (fin.peek()!='\n') fin >> decayname;
            fParticles.push_back(ThermalParticle(stable, name, pdgid, spin, stat, mass, str, bary, chg, abss, width, threshold, decayname));
            NumberOfParticles++;
            IDtoPDG.push_back(pdgid);
            PDGtoID[pdgid] = IDtoPDG.size()-1;
            fParticles[fParticles.size()-1].ReadDecays(decayprefix + name + "_decay.txt");
            if (!(bary==0 && chg==0 && str==0) || name.substr(0,3)=="K0S" || name.substr(0,3)=="K0L") {
                if (bary==0 && name[name.size()-1]=='+') name[name.size()-1] = '-';
                else if (bary==0 && name[name.size()-1]=='-') name[name.size()-1] = '+';
                else name += string("bar");
                fParticles.push_back(ThermalParticle(stable, name, -pdgid, spin, stat, mass, -str, -bary, -chg, abss, width, threshold, decayname));
                IDtoPDG.push_back(-pdgid);
                PDGtoID[-pdgid] = IDtoPDG.size()-1;
            }
        }
    }
    sort(fParticles.begin(), fParticles.end(), ThermalParticleSystemNameSpace::cmpParticleMass);
    PDGtoID.clear();
    IDtoPDG.resize(0);
    for(unsigned int i=0;i<fParticles.size();++i) {
        IDtoPDG.push_back(fParticles[i].fPDGID);
        PDGtoID[fParticles[i].fPDGID] = IDtoPDG.size()-1;
    }
    for(unsigned int i=0;i<fParticles.size();++i) {
        if (fParticles[i].fPDGID<0) fParticles[i].SetDecays(GetDecaysFromAntiParticle(fParticles[PDGtoID[-fParticles[i].fPDGID]].fDecays));
    }
    fin.close();
    FillResonanceDecays();
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void ThermalParticleSystem::LoadDatabase(std::string InputFile) {
    vector< vector<ParticleDecay> > decays(0);
    vector<int> pdgids(0);
    map<int, int> decaymap;
    decaymap.clear();
    NumberOfParticles = 0;
    fParticles.resize(0);
    PDGtoID.clear();
    IDtoPDG.resize(0);
    string decayprefix = "";

    for(int i=InputFile.size()-1;i>=0;--i)
        if (InputFile[i]=='\\' || InputFile[i]=='/')
        {
            decayprefix = InputFile.substr(0, i+1);
            break;
        }

    ifstream fin((decayprefix + "decays.dat").c_str());
    if (fin.is_open()) {
        int decaypartnumber = 0;
        fin >> decaypartnumber;
        decays.reserve(decaypartnumber);
        pdgids.reserve(decaypartnumber);

        for(unsigned int i=0;i<static_cast<unsigned int>(decaypartnumber);++i) {
            int pdgid, decaysnumber, tmpid, daughters;
            double bratio;
            fin >> pdgid >> decaysnumber;
            decaymap[pdgid] = i;
            decays.push_back(vector<ParticleDecay>(0));
            pdgids.push_back(pdgid);
            for(unsigned int j=0;j<static_cast<unsigned int>(decaysnumber);++j) {
                ParticleDecay decay;
                fin >> bratio;
                decay.fBratio = bratio / 100.;
                fin >> daughters;
                decay.fDaughters.reserve(daughters);
                for(unsigned int k=0;k<static_cast<unsigned int>(daughters);++k) {
                    fin >> tmpid;
                    decay.fDaughters.push_back(tmpid);
                }
                decays[i].push_back(decay);
            }
        }
        fin.close();
    }

    // fin.open(InputFile);
    ifstream fin2(InputFile.c_str());
	if (fin2.is_open()) {
        string tmp;
        char tmpc[500];
        fin2.getline(tmpc, 500);
        tmp = string(tmpc);
        while (1) {
            vector<string> fields = split(tmp, ';');
            if (fields.size()<14) break;
            int stable, pdgid, spin, stat, str, bary, chg, charm;
            double mass, width, threshold, abss, absc, radius = 0.5;
            string name, decayname = "";
            stable = atoi(fields[0].c_str());
            name = fields[1];
            pdgid = atoi(fields[2].c_str());
            spin = atoi(fields[3].c_str());
            stat = atoi(fields[4].c_str());
            mass = atof(fields[5].c_str());
            str = atoi(fields[6].c_str());
            bary = atoi(fields[7].c_str());
            chg = atoi(fields[8].c_str());
            charm = atoi(fields[9].c_str());
            abss = atof(fields[10].c_str());
            absc = atof(fields[11].c_str());
            width = atof(fields[12].c_str());
            threshold = atof(fields[13].c_str());
            if (fields.size()>=15) radius = atof(fields[14].c_str());
            if (fields.size()==16) decayname = fields[15];

            fParticles.push_back(ThermalParticle(stable, name, pdgid, spin, stat, mass, str, bary, chg, abss, width, threshold, decayname, charm, absc, radius));
            NumberOfParticles++;
            IDtoPDG.push_back(pdgid);
            PDGtoID[pdgid] = IDtoPDG.size()-1;

            if (decaymap.count(pdgid)!=0) fParticles[fParticles.size()-1].fDecays = decays[decaymap[pdgid]];

            if (!(bary==0 && chg==0 && str==0 && charm==0) || name.substr(0,3)=="K0S" || name.substr(0,3)=="K0L") {
                if (bary==0 && name[name.size()-1]=='+') name[name.size()-1] = '-';
                else if (bary==0 && name[name.size()-1]=='-') name[name.size()-1] = '+';
                else name += string("bar");
                fParticles.push_back(ThermalParticle(stable, name, -pdgid, spin, stat, mass, -str, -bary, -chg, abss, width, threshold, decayname, -charm, absc, radius));
                IDtoPDG.push_back(-pdgid);
                PDGtoID[-pdgid] = IDtoPDG.size()-1;
            }

            fin2.getline(tmpc, 500);
            tmp = string(tmpc);
        }

        sort(fParticles.begin(), fParticles.end(), ThermalParticleSystemNameSpace::cmpParticleMass);
        PDGtoID.clear();
        IDtoPDG.resize(0);
        for(unsigned int i=0;i<fParticles.size();++i) {
            IDtoPDG.push_back(fParticles[i].fPDGID);
            PDGtoID[fParticles[i].fPDGID] = IDtoPDG.size()-1;
        }
        for(unsigned int i=0;i<fParticles.size();++i) {
            if (fParticles[i].fPDGID<0) fParticles[i].SetDecays(GetDecaysFromAntiParticle(fParticles[PDGtoID[-fParticles[i].fPDGID]].fDecays));
        }
        for(unsigned int i=0;i<fParticles.size();++i) fParticles[i].fDecaysOrig = fParticles[i].fDecays;
        fin2.close();
        FillResonanceDecays();
    }
}

std::string ThermalParticleSystem::GetNameFromPDG(int pdgid) {
    if (PDGtoID.count(pdgid)==0) return string("???");
    else return fParticles[PDGtoID[pdgid]].fName;
}
