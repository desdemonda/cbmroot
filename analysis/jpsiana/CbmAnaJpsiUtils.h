
#ifndef CBM_ANA_JPSI_UTILS_H
#define CBM_ANA_JPSI_UTILS_H

#include "CbmAnaJpsiCandidate.h"
#include "CbmStsTrack.h"
#include "CbmKFVertex.h"
#include "CbmL1PFFitter.h"
#include "L1Field.h"
#include <vector>
#include "TDatabasePDG.h"
using namespace std;

class CbmAnaJpsiUtils{
public:

	/*
	 * Calculates and set track parameters to CbmAnaJpsiCandidate.
	 * The following parameters are set: fChi2sts, fChi2Prim, fPosition, fMomentum, fMass, fCharge, fEnergy, fRapidity
	 */
	static void CalculateAndSetTrackParamsToCandidate(
	    CbmAnaJpsiCandidate* cand,
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
};

#endif
