/**
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2015
 * @version 1.0
 **/


#ifndef CBM_LMVM_UTILS_H
#define CBM_LMVM_UTILS_H

#include "CbmMCTrack.h"
#include "CbmLmvmCandidate.h"
#include "CbmStsTrack.h"
#include "CbmKFVertex.h"
#include "CbmL1PFFitter.h"

class CbmLmvmUtils{
public:

	/*
	 * Calculates and set track parameters to CbmLmvmCandidate.
	 * The following parameters are set: fChi2sts, fChi2Prim, fPosition, fMomentum, fMass, fCharge, fEnergy, fRapidity
	 */
	static void CalculateAndSetTrackParamsToCandidate(
		CbmLmvmCandidate* cand,
		CbmStsTrack* stsTrack,
		CbmKFVertex& kfVertex)
	{
		CbmL1PFFitter fPFFitter;
		vector<CbmStsTrack> stsTracks;
		stsTracks.resize(1);
		stsTracks[0] = *stsTrack;
		vector<L1FieldRegion> vField;
		vector<float> chiPrim;
		fPFFitter.GetChiToVertex(stsTracks, vField, chiPrim, kfVertex, 3e6);
		cand->fChi2sts = stsTracks[0].GetChiSq() / stsTracks[0].GetNDF();
		cand->fChi2Prim = chiPrim[0];
		const FairTrackParam* vtxTrack = stsTracks[0].GetParamFirst();

		vtxTrack->Position(cand->fPosition);
		vtxTrack->Momentum(cand->fMomentum);

		cand->fMass = TDatabasePDG::Instance()->GetParticle(11)->Mass();
		cand->fCharge = (vtxTrack->GetQp() > 0) ?1 :-1;
		cand->fEnergy = sqrt(cand->fMomentum.Mag2() + cand->fMass * cand->fMass);
		cand->fRapidity = 0.5*TMath::Log((cand->fEnergy + cand->fMomentum.Z()) / (cand->fEnergy - cand->fMomentum.Z()));
	}

	/*
	 * Armenteros - Podolansky plot
	 */
    static void CalculateArmPodParams(
    	CbmLmvmCandidate* cand1,
		CbmLmvmCandidate* cand2,
        Double_t &alpha,
        Double_t &ptt)
    {
		alpha = ptt = 0.;
		Double_t spx = cand1->fMomentum.X() + cand2->fMomentum.X();
		Double_t spy = cand1->fMomentum.Y() + cand2->fMomentum.Y();
		Double_t spz = cand1->fMomentum.Z() + cand2->fMomentum.Z();
		Double_t sp  = sqrt(spx*spx + spy*spy + spz*spz);

		if( sp == 0.0) return;
		Double_t pn, pp, pln, plp;
		if(cand1->fCharge < 0.) {
			pn = cand1->fMomentum.Mag();
			pp = cand2->fMomentum.Mag();
			pln  = (cand1->fMomentum.X()*spx + cand1->fMomentum.Y()*spy + cand1->fMomentum.Z()*spz)/sp;
			plp  = (cand2->fMomentum.X()*spx + cand2->fMomentum.Y()*spy + cand2->fMomentum.Z()*spz)/sp;
		}else{
			pn = cand2->fMomentum.Mag();
			pp = cand1->fMomentum.Mag();
			pln  = (cand2->fMomentum.X()*spx + cand2->fMomentum.Y()*spy + cand2->fMomentum.Z()*spz)/sp;
			plp  = (cand1->fMomentum.X()*spx + cand1->fMomentum.Y()*spy + cand1->fMomentum.Z()*spz)/sp;
		}
		if( pn == 0.0) return;
		Double_t ptm  = (1.-((pln/pn)*(pln/pn)));
		ptt= (ptm>=0.)?  pn*sqrt(ptm) :0;
		alpha = (plp-pln)/(plp+pln);
    }

};

#endif
