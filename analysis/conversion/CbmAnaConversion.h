/**
* \file CbmAnaConversion.h
*
* Base class for conversion analysis
*
* \author Sascha Reinecke <reinecke@uni-wuppertal.de>
* \date 2014
**/

#ifndef CBM_ANA_CONVERSION
#define CBM_ANA_CONVERSION

#include "FairTask.h"
#include "CbmMCTrack.h"
#include "../dielectron/CbmLmvmKinematicParams.h"
#include "CbmKFVertex.h"

//#include "KFParticleTopoReconstructor.h"
#include "CbmKFParticleFinder.h"
#include "CbmKFParticleFinderQA.h"

#include "CbmAnaConversionTomography.h"
#include "CbmAnaConversionRich.h"
#include "CbmAnaConversionKF.h"
#include "CbmAnaConversionReco.h"
#include "CbmAnaConversionPhotons.h"
#include "CbmAnaConversionRecoFull.h"

class TH1;
class TH2;
class TH3;
class TH1D;
class TH2D;
class TH2I;
class TH3D;
class TClonesArray;
class CbmRichRing;
class TCanvas;
class TRandom3;

#include <vector>
#include <map>

#include "TStopwatch.h"

using namespace std;



/*class KinematicParams{
public:
   Double_t momentumMag; // Absolute value of momentum
   Double_t pt; // Transverse momentum
   Double_t rapidity; // Rapidity
   Double_t minv; // Invariant mass
   Double_t angle; // Opening angle
};*/

class CbmAnaConversion : public FairTask
{

public:
   /**
    * \brief Standard constructor.
    */
	CbmAnaConversion();

   /**
    * \brief Standard destructor.
    */
   virtual ~CbmAnaConversion();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(
		   Option_t* option);
		   
   CbmLmvmKinematicParams CalculateKinematicParams(const CbmMCTrack* mctrackP, const CbmMCTrack* mctrackM);
        
	Double_t	Invmass_2gammas(const CbmMCTrack* gamma1, const CbmMCTrack* gamma2);
	Double_t	Invmass_2particles(const CbmMCTrack* mctrack1, const CbmMCTrack* mctrack2);
	Double_t	Invmass_4particles(const CbmMCTrack* mctrack1, const CbmMCTrack* mctrack2, const CbmMCTrack* mctrack3, const CbmMCTrack* mctrack4);


	void	CalculateInvMass_MC_2particles();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();
   
   void		AnalyseElectrons(CbmMCTrack* mctrack);
   void		FillMCTracklists(CbmMCTrack* mctrack, int i);
   void		FillRecoTracklist(CbmMCTrack* mtrack);
   void		FillRecoTracklistEPEM(CbmMCTrack* mctrack, TVector3 stsMomentum, TVector3 refittedMom, int i);
   void		InvariantMassTest();
   void		InvariantMassTest_4epem();
   void		InvariantMassTestReco();
   int		GetTest();
   int		GetNofEvents();
   void		SetMode(int mode);
   
   void		ReconstructGamma();
   
   
   void		SetKF(CbmKFParticleFinder* kfparticle, CbmKFParticleFinderQA* kfparticleQA);




private:
	Int_t DoTomography;
	Int_t DoRichAnalysis;
	Int_t DoKFAnalysis;
	Int_t DoReconstruction;
	Int_t DoPhotons;
	Int_t DoRecoFull;
   
   TH1D * fhNofElPrim;
   TH1D * fhNofElSec;
   TH1D * fhNofElAll;
   TH1D * fhElectronSources;
   TH1D * fhNofPi0_perEvent;		// number of pi0 per event
   TH1D * fhNofPi0_perEvent_cut;	// number of pi0 with cut on z-axis (z <= 70cm, i.e. generated before 70cm)
   TH1D * fhPi0_z;					// number of pi0 per z-bin
   TH1D * fhPi0_z_cut;				// number of pi0 per z-bin with cut on acceptance (25° via x^2 + y^2 <= r^2 with r = z*tan 25°)
   TH1D * fhElectronsFromPi0_z;		//
   
   
   TH1D * fhInvariantMass_test;
   TH1D * fhInvariantMass_test2;
   TH1D * fhInvariantMass_test3;
   TH1D * fhInvariantMassReco_test;
   TH1D * fhInvariantMassReco_test2;
   TH1D * fhInvariantMassReco_test3;
      
   TH1D * fhInvariantMassReco_pi0;
   
   TH2D * fhMomentum_MCvsReco;
   TH1D * fhMomentum_MCvsReco_diff;
   

      
   TH1D * fhSearchGammas;

	CbmVertex *fPrimVertex;
	CbmKFVertex fKFVertex;
   TClonesArray* fRichPoints;
   TClonesArray* fRichRings;
   TClonesArray* fRichRingMatches;
   TClonesArray* fMcTracks;
   TClonesArray* fStsTracks;
   TClonesArray* fStsTrackMatches;
   TClonesArray* fGlobalTracks;

   Int_t fEventNum;
   
   Int_t test;
   
   int testint;
   
   Int_t fAnalyseMode;
   
   
   CbmKFParticleFinder* fKFparticle;
   CbmKFParticleFinderQA* fKFparticleFinderQA;
   const KFParticleTopoReconstructor* fKFtopo;
   vector<int> trackindexarray;
   Int_t particlecounter;
   Int_t particlecounter_2daughters;
   Int_t particlecounter_3daughters;
   Int_t particlecounter_4daughters;
   Int_t particlecounter_all;


	// for data gained from the KFParticle package
	Int_t fNofGeneratedPi0_allEvents;	// number of generated pi0 summed up over ALL EVENTS
	Int_t fNofPi0_kfparticle_allEvents;	// number of all reconstructed pi0 summed up over ALL EVENTS
	Int_t fNofGeneratedPi0;
	Int_t fNofPi0_kfparticle;
	TH1D * fhPi0Ratio;
	TH1D * fhPi0_mass;
	TH1D * fhPi0_NDaughters;








   /**
    * \brief Initialize histograms.
    */
   void InitHistograms();
   
   vector<TH1*> fHistoList;				// list of all histograms
   vector<TH1*> fHistoList_MC;			// list of all histograms generated with MC data
   vector<TH1*> fHistoList_tomography;	// list of all histograms of tomography data (photon conversion)
   vector<TH1*> fHistoList_reco;		// list of all histograms of reconstruction data
   vector<TH1*> fHistoList_reco_mom;	// list of all histograms of reconstruction data (used momenta)
   vector<TH1*> fHistoList_kfparticle;	// list of all histograms containing results from KFParticle package
   vector<TH1*> fHistoList_richrings;	// list of all histograms related to rich rings
   
   vector<CbmMCTrack*>	fMCTracklist;
   vector<CbmMCTrack*>	fMCTracklist_all;
   vector<CbmMCTrack*>	fRecoTracklist;
   vector<CbmMCTrack*>	fRecoTracklistEPEM;
   vector<int>			fRecoTracklistEPEM_id; // ids of mctracks from fRecoTracklistEPEM
   
   vector<TVector3> fRecoMomentum;
   vector<TVector3> fRecoRefittedMomentum;
   
   
   
   // timer
   TStopwatch timer_all;
   Double_t fTime_all;
   
   TStopwatch timer_mc;
   Double_t fTime_mc;
   TStopwatch timer_rec;
   Double_t fTime_rec;
   
   
   CbmAnaConversionTomography	*fAnaTomography;
   CbmAnaConversionRich			*fAnaRich;
   CbmAnaConversionKF			*fAnaKF;
   CbmAnaConversionReco			*fAnaReco;
   CbmAnaConversionPhotons		*fAnaPhotons;
   CbmAnaConversionRecoFull		*fAnaRecoFull;
   
   
   
   /**
    * \brief Copy constructor.
    */
   CbmAnaConversion(const CbmAnaConversion&);

   /**
    * \brief Assignment operator.
    */
   CbmAnaConversion& operator=(const CbmAnaConversion&);


	ClassDef(CbmAnaConversion,1)
};

#endif

