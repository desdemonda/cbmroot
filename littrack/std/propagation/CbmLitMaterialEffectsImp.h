/**
 * \file CbmLitMaterialEffectsImp.h
 * \brief Calculation of multiple scattering and energy loss.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 */

#ifndef CBMLITMATERIALEFFECTSIMP_H_
#define CBMLITMATERIALEFFECTSIMP_H_

#include "base/CbmLitFloat.h"
#include "interface/CbmLitMaterialEffects.h"

class CbmLitTrackParam;
class CbmLitMaterialInfo;

/**
 * \class CbmLitMaterialEffectsImp
 * \brief Calculation of multiple scattering and energy loss.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 */
class CbmLitMaterialEffectsImp : public CbmLitMaterialEffects
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitMaterialEffectsImp();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitMaterialEffectsImp();

   /**
    * \brief Inherited from CbmLitMaterialEffects.
    */
   LitStatus Update(
      CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat,
      int pdg,
      bool downstream);

// LitStatus Update2(
//       CbmLitTrackParam* par,
//         const CbmLitMaterialInfo* mat);
public:

   void AddThinScatter(
      CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   void AddThickScatter(
      CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   void AddEnergyLoss(
      CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat CalcThetaSq(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat BetheBlochSimple(
      const CbmLitMaterialInfo* mat) const;

   litfloat BetheBloch(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat BetheBlochElectron(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat EnergyLoss(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat dEdx(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat CalcQpAfterEloss(
      litfloat qp,
      litfloat eloss) const;

   litfloat CalcSigmaSqQp(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat CalcSigmaSqQpElectron(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat MPVEnergyLoss(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat BetheHeitler(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat PairProduction(
      const CbmLitTrackParam* par,
      const CbmLitMaterialInfo* mat) const;

   litfloat CalcI(
      litfloat Z) const;
private:
   bool fDownstream; // Propagation direction
   litfloat fMass; // Hypothesis on particle mass
   bool fIsElectron; // True if particle is an electron or positron
   bool fIsMuon; // True if particle is muon
};

#endif /*CBMLITMATERIALEFFECTSIMP_H_*/
