/** CbmLitMaterialEffectsImp.cxx
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 **/
#include "propagation/CbmLitMaterialEffectsImp.h"

#include "base/CbmLitDefaultSettings.h"
#include "data/CbmLitTrackParam.h"
#include "propagation/CbmLitMaterialInfo.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cassert>

CbmLitMaterialEffectsImp::CbmLitMaterialEffectsImp():
   fMass(0.105),
   fDownstream(true),
   fIsElectron(false),
   fIsMuon(true)
{
}

CbmLitMaterialEffectsImp::~CbmLitMaterialEffectsImp()
{
}

LitStatus CbmLitMaterialEffectsImp::Update(
   CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat,
   int pdg,
   bool downstream)
{
   if (mat->GetLength() * mat->GetRho() < 1e-10) { return kLITSUCCESS; }

   fDownstream = downstream;
   TDatabasePDG* db = TDatabasePDG::Instance();
   TParticlePDG* particle = db->GetParticle(pdg);
   assert(particle != NULL);
   fMass = particle->Mass();
   fIsElectron = (std::abs(pdg) == 11) ? true : false;
   fIsMuon = (std::abs(pdg) == 13) ? true : false;

   AddEnergyLoss(par, mat);

//   AddThinScatter(par, mat);
   AddThickScatter(par, mat);

   return kLITSUCCESS;
}

void CbmLitMaterialEffectsImp::AddEnergyLoss(
   CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat) const
{
   if (fIsElectron) {
      litfloat radLength = mat->GetLength() / mat->GetRL();
      litfloat t;

      if (!fDownstream) { t = radLength; }
      else { t = -radLength; }

      litfloat qp = par->GetQp();
      qp *= std::exp(-t);
      par->SetQp(qp);

      litfloat cov = par->GetCovariance(14);
      cov += CalcSigmaSqQpElectron(par, mat);
      par->SetCovariance(14, cov);
   } else {
      litfloat Eloss = EnergyLoss(par, mat);
      par->SetQp(CalcQpAfterEloss(par->GetQp(), Eloss));

      litfloat cov = par->GetCovariance(14);
      cov += CalcSigmaSqQp(par, mat);
      par->SetCovariance(14, cov);
   }
}

void CbmLitMaterialEffectsImp::AddThickScatter(
   CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat) const
{
   if (mat->GetLength() < 1e-10) { return; }

   litfloat tx = par->GetTx();
   litfloat ty = par->GetTy();
   litfloat thickness = mat->GetLength(); //cm
   litfloat thetaSq = CalcThetaSq(par, mat);

   litfloat t = 1 + tx * tx + ty * ty;

   litfloat Q33 = (1 + tx * tx) * t * thetaSq;
   litfloat Q44 = (1 + ty * ty) * t * thetaSq;
   litfloat Q34 = tx * ty * t * thetaSq;

   litfloat T23 = (thickness * thickness) / 3.0;
   litfloat T2 = thickness / 2.0;

   litfloat D = (fDownstream) ? 1. : -1.;

   std::vector<litfloat> C = par->GetCovMatrix();

   C[0] += Q33 * T23;
   C[1] += Q34 * T23;
   C[2] += Q33 * D * T2;
   C[3] += Q34 * D * T2;

   C[5] += Q44 * T23;
   C[6] += Q34 * D * T2;
   C[7] += Q44 * D * T2;

   C[9] += Q33;
   C[10] += Q34;

   C[12] += Q44;

   par->SetCovMatrix(C);
}

void CbmLitMaterialEffectsImp::AddThinScatter(
   CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat) const
{
   if (mat->GetLength() < 1e-10) { return; }
   litfloat tx = par->GetTx();
   litfloat ty = par->GetTy();
   litfloat thetaSq = CalcThetaSq(par, mat);

   litfloat t = 1 + tx * tx + ty * ty;

   litfloat Q33 = (1 + tx * tx) * t * thetaSq;
   litfloat Q44 = (1 + ty * ty) * t * thetaSq;
   litfloat Q34 = tx * ty * t * thetaSq;

   std::vector<litfloat> C = par->GetCovMatrix();
   C[9] += Q33;
   C[12] += Q44;
   C[10] += Q34;
   par->SetCovMatrix(C);
}

litfloat CbmLitMaterialEffectsImp::CalcThetaSq(
   const CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat) const
{
   litfloat p = std::abs(1. / par->GetQp()); //GeV
   litfloat E = std::sqrt(fMass * fMass + p * p);
   litfloat beta = p / E;
   litfloat x = mat->GetLength(); //cm
   litfloat X0 = mat->GetRL(); //cm
   litfloat bcp = beta * p;
   litfloat z = 1.;

   litfloat theta = 0.0136 * (1./bcp) * z * std::sqrt(x/X0) *
               (1. + 0.038 * std::log(x/X0));
   return theta * theta;
}

litfloat CbmLitMaterialEffectsImp::EnergyLoss(
   const CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat) const
{
   litfloat length = mat->GetRho() * mat->GetLength();
   return dEdx(par, mat) * length;
   //return MPVEnergyLoss(par, mat);
}

litfloat CbmLitMaterialEffectsImp::dEdx(
   const CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat) const
{
   litfloat dedx = BetheBloch(par, mat);
// dedx += BetheHeitler(par, mat);
// if (fIsMuon) dedx += PairProduction(par, mat);
   return dedx;
}

litfloat CbmLitMaterialEffectsImp::BetheBloch(
   const CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat) const
{
   litfloat K = 0.000307075; // GeV * g^-1 * cm^2
   litfloat z = (par->GetQp() > 0.) ? 1 : -1.;
   litfloat Z = mat->GetZ();
   litfloat A = mat->GetA();

   litfloat M = fMass;
   litfloat p = std::abs(1. / par->GetQp()); //GeV
   litfloat E = std::sqrt(M * M + p * p);
   litfloat beta = p / E;
   litfloat betaSq = beta * beta;
   litfloat gamma = E / M;
   litfloat gammaSq = gamma * gamma;

   litfloat I = CalcI(Z) * 1e-9; // GeV

   litfloat me = 0.000511; // GeV
   litfloat ratio = me/M;
   litfloat Tmax = (2*me*betaSq*gammaSq) / (1+2*gamma*ratio+ratio*ratio);

   // density correction
   litfloat dc = 0.;
   if (p > 0.5) { // for particles above 1 Gev
      litfloat rho = mat->GetRho();
      litfloat hwp = 28.816 * std::sqrt(rho*Z/A) * 1e-9 ; // GeV
      dc = std::log(hwp/I) + std::log(beta*gamma) - 0.5;
   }

   return K*z*z*(Z/A)*(1./betaSq) * (0.5*std::log(2*me*betaSq*gammaSq*Tmax/(I*I))-betaSq - dc);
}

litfloat CbmLitMaterialEffectsImp::BetheBlochElectron(
   const CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat) const
{
   litfloat K = 0.000307075; // GeV * g^-1 * cm^2
   //myf z = (par->GetQp() > 0.) ? 1 : -1.;
   litfloat Z = mat->GetZ();
   litfloat A = mat->GetA();

   litfloat me = 0.000511; // GeV;
   litfloat p = std::abs(1. / par->GetQp()); //GeV
   litfloat E = std::sqrt(me * me + p * p);
   litfloat gamma = E / me;

   litfloat I = CalcI(Z) * 1e-9; // GeV

   if (par->GetQp() > 0) { // electrons
      return K*(Z/A) * (std::log(2*me/I)+1.5*std::log(gamma) - 0.975);
   } else { //positrons
      return K*(Z/A) * (std::log(2*me/I)+2.*std::log(gamma) - 1.);
   }
}

litfloat CbmLitMaterialEffectsImp::CalcQpAfterEloss(
   litfloat qp,
   litfloat eloss) const
{
   litfloat massSq = fMass*fMass;
   litfloat p = std::abs(1./qp);
   litfloat E = std::sqrt(p*p + massSq);
   litfloat q = (qp>0) ? 1.: -1.;
   if (!fDownstream) { eloss *= -1.0; } // TODO check this
   litfloat Enew = E-eloss;
   litfloat pnew = (Enew > fMass) ? std::sqrt(Enew * Enew - massSq) : 0.;
   if (pnew != 0) { return q/pnew; }
   else { return 1e5; }

   //if (!fDownstream) eloss *= -1.0;
   //if (p > 0.) p -= eloss;
   //else p += eloss;
   //return 1./p;
}

litfloat CbmLitMaterialEffectsImp::CalcSigmaSqQp(
   const CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat) const
{
   litfloat P = std::abs(1. / par->GetQp()); // GeV
   litfloat XMASS = fMass; // GeV
   litfloat E = std::sqrt(P * P + XMASS * XMASS);
   litfloat Z = mat->GetZ();
   litfloat A = mat->GetA();
   litfloat RHO = mat->GetRho();
   litfloat STEP = mat->GetLength();
   litfloat EMASS = 0.511 * 1e-3; // GeV

   litfloat BETA = P/E;
   litfloat GAMMA = E/XMASS;

   // Calculate xi factor (KeV).
   litfloat XI = (153.5*Z*STEP*RHO)/(A*BETA*BETA);

   // Maximum energy transfer to atomic electron (KeV).
   litfloat ETA = BETA*GAMMA;
   litfloat ETASQ = ETA*ETA;
   litfloat RATIO = EMASS/XMASS;
   litfloat F1 = 2.*EMASS*ETASQ;
   litfloat F2 = 1.+2.*RATIO*GAMMA+RATIO*RATIO;
   litfloat EMAX = 1e6 * F1/F2;

   litfloat DEDX2 = XI*EMAX*(1.-(BETA*BETA/2.))*1e-12;

   litfloat SDEDX = (E*E*DEDX2) / std::pow(P, 6);

   return std::abs(SDEDX);
}

litfloat CbmLitMaterialEffectsImp::CalcSigmaSqQpElectron(
   const CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat) const
{
   litfloat x = mat->GetLength(); //cm
   litfloat X0 = mat->GetRL(); //cm
   return par->GetQp() * par->GetQp() *
          (std::exp(-x/X0 * std::log(3.0)/std::log(2.0)) -
           std::exp(-2.0 * x/X0));
}

litfloat CbmLitMaterialEffectsImp::CalcI(
   litfloat Z) const
{
   // mean excitation energy in eV
   if (Z > 16.) { return 10 * Z; }
   else { return 16 * std::pow(Z, 0.9); }
}

litfloat CbmLitMaterialEffectsImp::BetheHeitler(
   const CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat) const
{
   litfloat M = fMass; //GeV
   litfloat p = std::abs(1. / par->GetQp());  // GeV
   litfloat rho = mat->GetRho();
   litfloat X0 = mat->GetRL();
   litfloat me = 0.000511; // GeV
   litfloat E = std::sqrt(M * M + p * p);
   litfloat ratio = me/M;

   return (E*ratio*ratio)/(X0*rho);
}

litfloat CbmLitMaterialEffectsImp::PairProduction(
   const CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat) const
{
   litfloat p = std::abs(1. / par->GetQp());  // GeV
   litfloat M = fMass; //GeV
   litfloat rho = mat->GetRho();
   litfloat X0 = mat->GetRL();
   litfloat E = std::sqrt(M * M + p * p);

   return 7e-5*E/(X0*rho);
}

litfloat CbmLitMaterialEffectsImp::BetheBlochSimple(
   const CbmLitMaterialInfo* mat) const
{
   return lit::CbmLitDefaultSettings::ENERGY_LOSS_CONST * mat->GetZ() / mat->GetA();
}

litfloat CbmLitMaterialEffectsImp::MPVEnergyLoss(
   const CbmLitTrackParam* par,
   const CbmLitMaterialInfo* mat) const
{
   litfloat M = fMass * 1e3; //MeV
   litfloat p = std::abs(1. / par->GetQp()) * 1e3;  // MeV

//   myf rho = mat->GetRho();
   litfloat Z = mat->GetZ();
   litfloat A = mat->GetA();
   litfloat x = mat->GetRho() * mat->GetLength();

   litfloat I = CalcI(Z) * 1e-6; // MeV

   litfloat K = 0.307075; // MeV g^-1 cm^2
   litfloat j = 0.200;

   litfloat E = std::sqrt(M * M + p * p);
   litfloat beta = p / E;
   litfloat betaSq = beta * beta;
   litfloat gamma = E / M;
   litfloat gammaSq = gamma * gamma;

   litfloat ksi = (K/2.)*(Z/A)*(x/betaSq); // MeV

//   myf hwp = 28.816 * std::sqrt(rho*Z/A) * 1e-6 ; // MeV
//   myf dc = std::log(hwp/I) + std::log(beta*gamma) - 0.5;
//   dc *= 2;
   litfloat dc = 0.;

   litfloat eloss = ksi * (std::log(2*M*betaSq*gammaSq / I) + std::log(ksi/I) + j - betaSq - dc);

   return eloss * 1e-3; //GeV
}
