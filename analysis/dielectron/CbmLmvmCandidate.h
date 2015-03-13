/**
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2015
 * @version .0
 **/
#ifndef CBM_LMVM_CANDIDATE_H
#define CBM_LMVM_CANDIDATE_H

#include "TVector3.h"

class CbmLmvmCandidate{
public:
	CbmLmvmCandidate()
	  : fPosition(),
	    fMomentum(),
	    fMass(0.),
	    fEnergy(0.),
	    fRapidity(0.),
	    fCharge(0),
	    fChi2Prim(0.),
	    fChi2sts(0.),
	    fMcMotherId(-1),
	    fStsMcTrackId(-1),
	    fRichMcTrackId(-1),
	    fTrdMcTrackId(-1),
	    fTofMcTrackId(-1),
	    fStsInd(-1),
	    fRichInd(-1),
	    fTrdInd(-1),
	    fTofInd(-1),
	    fIsElectron(kFALSE),
	    fIsMcSignalElectron(kFALSE),
	    fIsMcPi0Electron(kFALSE),
	    fIsMcGammaElectron(kFALSE),
	    fIsMcEtaElectron(kFALSE),
	    fMcPdg(-1),
	    fIsGamma(kFALSE),
	    fDSts(0.),
	    fIsTtCutElectron(kFALSE),
	    fIsStCutElectron(kFALSE),
	    fIsRtCutElectron(kFALSE),
	    fIsMvd1CutElectron(kFALSE),
	    fIsMvd2CutElectron(kFALSE),
	    fRichAnn(0.),
	    fTrdAnn(0.),
	    fMass2(0.)
	{
	}

	void ResetMcParams()
	{
		fIsMcSignalElectron = false;
		fIsMcPi0Electron = false;
		fIsMcGammaElectron = false;
		fIsMcEtaElectron = false;
		fMcMotherId = -1;
		fStsMcTrackId = -1;
		fRichMcTrackId = -1;
		fTrdMcTrackId = -1;
		fTofMcTrackId = -1;
	}

	// track parameters
	TVector3 fPosition;
	TVector3 fMomentum;
	Double_t fMass;
	Double_t fEnergy;
	Double_t fRapidity;
	Int_t fCharge;
	Double_t fChi2Prim;
	Double_t fChi2sts;

	Int_t fMcMotherId;
	Int_t fStsMcTrackId;
	Int_t fRichMcTrackId;
	Int_t fTrdMcTrackId;
	Int_t fTofMcTrackId;
	Int_t fStsInd;
	Int_t fRichInd;
	Int_t fTrdInd;
	Int_t fTofInd;
	Bool_t fIsElectron;
	Bool_t fIsMcSignalElectron;
	Bool_t fIsMcPi0Electron;
	Bool_t fIsMcGammaElectron;
	Bool_t fIsMcEtaElectron;

	Int_t fMcPdg;
	Bool_t fIsGamma;
	Double_t fDSts;
	Bool_t fIsTtCutElectron;
	Bool_t fIsStCutElectron;
	Bool_t fIsRtCutElectron;
	Bool_t fIsMvd1CutElectron;
	Bool_t fIsMvd2CutElectron;
	Double_t fRichAnn;
	Double_t fTrdAnn;
	Double_t fMass2;
};

#endif
