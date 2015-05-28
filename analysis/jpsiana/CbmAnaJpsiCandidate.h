
#ifndef CBM_ANA_JPSI_CANDIDATE_H
#define CBM_ANA_JPSI_CANDIDATE_H

#include "TVector3.h"

class CbmAnaJpsiCandidate{
public:
	CbmAnaJpsiCandidate()
	  : fPosition(),
		fMomentum(),
		fMass(0.),
		fEnergy(0.),
		fRapidity(0.),
		fCharge(0),
		fChi2Prim(0.),
		fChi2sts(0.),
		fStsInd(-1),
		fRichInd(-1),
		fTrdInd(-1),
		fTofInd(-1),
		fIsElectron(false),
		fStsMcTrackId(-1),
		fRichMcTrackId(-1),
		fTrdMcTrackId(-1),
		fTofMcTrackId(-1),
		fIsMcSignalElectron(false),
		fIsMcPi0Electron(false),
		fIsMcGammaElectron(false),
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
		fStsMcTrackId = -1;
	}

	TVector3 fPosition; // reconstructed position of the track
	TVector3 fMomentum; // reconstructed momenta of the track
	Double_t fMass; // mass
	Double_t fEnergy; // energy
	Double_t fRapidity; // rapidity
	Int_t fCharge; // charge
	Double_t fChi2Prim; // reconstructed chi2 to primary vertex
	Double_t fChi2sts; // reconstructed chi2 of the STS track fit

	Int_t fStsInd; // index of StsTrack
	Int_t fRichInd; // index of RichRing
	Int_t fTrdInd; // index of TrdTrack
	Int_t fTofInd; // index of TofHit

	Bool_t fIsElectron; // TRUE if candidate was identified as electron

	Int_t fStsMcTrackId; // MCTrack Id of the STS track
	Int_t fRichMcTrackId; // McTrack Id of the RICH ring
	Int_t fTrdMcTrackId; // MCTrack Id of the TRD track
	Int_t fTofMcTrackId; // MCTrack Id of the TOF hit

	Bool_t fIsMcSignalElectron; // Is signal electron (MC info)
	Bool_t fIsMcPi0Electron; // Is pi0 electron (MC info)
	Bool_t fIsMcGammaElectron; // Is gamma conversion electron (MC info)

	Double_t fRichAnn;
	Double_t fTrdAnn;
	Double_t fMass2;
};

#endif
