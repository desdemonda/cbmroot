//----------------------------------------------------------------------------
// Implementation of the KFParticle class
// .
// @author  I.Kisel, I.Kulakov, M.Zyzak
// @version 1.0
// @since   20.08.13
// 
// 
//  -= Copyright &copy ALICE HLT and CBM L1 Groups =-
//____________________________________________________________________________


#ifndef KFParticleDatabase_H
#define KFParticleDatabase_H

#include "Vc/Vc"

class KFParticleDatabase
{
 public:
  KFParticleDatabase();

  ~KFParticleDatabase() {};

  float GetMass(const int pdg) const
  {
    int pdgIndex = 2;
    switch ( abs(pdg) )
    {
      case   11: pdgIndex = 0; break;
      case   13: pdgIndex = 1; break;
      case  211: pdgIndex = 2; break;
      case  321: pdgIndex = 3; break;
      case 2212: pdgIndex = 4; break;
      default:   pdgIndex = 2; break;
    }
    
    return fMass[pdgIndex];
  }

  Vc::float_v GetMass(const Vc::int_v& pdg) const
  {
    Vc::int_v pdgIndex(2);
    pdgIndex(Vc::abs(pdg) ==   11) = 0;    
    pdgIndex(Vc::abs(pdg) ==   13) = 1;
    pdgIndex(Vc::abs(pdg) ==  211) = 2;
    pdgIndex(Vc::abs(pdg) ==  321) = 3;
    pdgIndex(Vc::abs(pdg) == 2212) = 4;
    
    Vc::float_v mass(fMass, pdgIndex);
    return mass;
  }

  void GetMotherMass(const Vc::int_v& pdg, Vc::float_v& massMotherPDG, Vc::float_v& massMotherPDGSigma) const
  {
    Vc::int_v pdgIndex(0);
    pdgIndex(pdg ==  310) = 0;    
    pdgIndex(Vc::abs(pdg) == 3122) = 1;
    pdgIndex(Vc::abs(pdg) == 3312) = 2;
    pdgIndex(pdg == 22) = 3;
   
    massMotherPDG.gather(fMassSecPDG, pdgIndex);
    massMotherPDGSigma.gather(fMassSecPDGSigma, pdgIndex);
  }
  
  void GetMotherMass(const int pdg, float& massMotherPDG, float& massMotherPDGSigma) const
  {
    int pdgIndex = 2;
    switch ( abs(pdg) )
    {
      case  310: pdgIndex = 0; break;
      case 3122: pdgIndex = 1; break;
      case 3312: pdgIndex = 2; break;
      default:   pdgIndex = 0; break;
    }
   
    massMotherPDG = fMassSecPDG[pdgIndex];
    massMotherPDGSigma = fMassSecPDGSigma[pdgIndex];
  }
  
  const float& GetPi0Mass() const { return fMassPi0PDG; }
  const float& GetPi0MassSigma() const { return fMassPi0PDGSigma; }
  const float& GetD0Mass() const { return fMassD0PDG; }
  const float& GetD0MassSigma() const { return fMassD0PDGSigma; }
  const float& GetDPlusMass() const { return fMassDPlusPDG; }
  const float& GetDPlusMassSigma() const { return fMassDPlusPDGSigma; }
  
  static const KFParticleDatabase* Instance() { return fDatabase; }

 private:
  float fMass[5];

  float fMassSecPDG[4];
  float fMassSecPDGSigma[4];
  
  float fMassPi0PDG;
  float fMassPi0PDGSigma;

  float fMassD0PDG;
  float fMassD0PDGSigma;
  float fMassDPlusPDG;
  float fMassDPlusPDGSigma;
  
  static KFParticleDatabase* fDatabase;
};

#endif