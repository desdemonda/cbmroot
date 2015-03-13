/**
 * \file global_reco_ideal.C
 * \brief Macro runs ideal event reconstruction.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 **/

#include <iostream>
using std::cout;
using std::endl;

void global_reco_ideal(Int_t nEvents = 5000)
{
	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

	Double_t trdHitErr = 100; // if == 0 than standard errors are used
	TString dir, imageDir, mcFile, parFile, globalTracksFile;
	TList *parFileList = new TList();
	TObjString stsDigiFile, trdDigiFile;
	TString muchDigiFile;
	if (script != "yes") {
	   dir = "/data.local1/andrey/events/std_muon_5jpsi/";
		mcFile = dir + "mc.0000.root";
		parFile = dir + "param.0000.root";
		globalTracksFile = dir + "global.tracks.ideal.0000.root";
		muchDigiFile = parDir + "/much/much_v11a.digi.root";
		TObjString stsDigiFile = parDir + "/sts/sts_v11a.digi.par";
		parFileList->Add(&stsDigiFile);
		TObjString trdDigiFile = parDir + "/trd/trd_v11b.digi.par";
		parFileList->Add(&trdDigiFile);
		imageDir = "./test/";
	} else {
		mcFile = TString(gSystem->Getenv("MCFILE"));
		parFile = TString(gSystem->Getenv("PARFILE"));
		globalTracksFile = TString(gSystem->Getenv("GLOBALTRACKSIDEALFILE"));
		imageDir = TString(gSystem->Getenv("IMAGEDIR"));
		muchDigiFile = TString(gSystem->Getenv("MUCHDIGI"));
		//trdHitErr = TString(gSystem->Getenv("TRDHITERR"))->Atof();
		TObjString stsDigiFile = TString(gSystem->Getenv("STSDIGI"));
		parFileList->Add(&stsDigiFile);
		TObjString trdDigiFile = TString(gSystem->Getenv("TRDDIGI"));
		parFileList->Add(&trdDigiFile);
	}

	Int_t iVerbose = 1;
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

	FairRunAna *run = new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(globalTracksFile);

	if (IsMvd(parFile)) {
		// ----- MVD reconstruction    --------------------------------------------
		CbmMvdDigitizeL* mvdDigi = new CbmMvdDigitizeL("MVD Digitiser", 0, iVerbose);
		run->AddTask(mvdDigi);

		CbmMvdFindHits* mvdHitFinder = new CbmMvdFindHits("MVD Hit Finder", 0, iVerbose);
		run->AddTask(mvdHitFinder);
		// -------------------------------------------------------------------------
	}

	// ----- STS reconstruction   ---------------------------------------------
	FairTask* stsDigitize = new CbmStsIdealDigitize("STSDigitize", iVerbose);
	run->AddTask(stsDigitize);

    FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder", iVerbose);
    run->AddTask(stsClusterFinder);

	FairTask* stsFindHits = new CbmStsIdealFindHits("STSFindHits", iVerbose);
	run->AddTask(stsFindHits);

	FairTask* stsMatchHits = new CbmStsIdealMatchHits("STSMatchHits", iVerbose);
	run->AddTask(stsMatchHits);

	FairTask* kalman= new CbmKF();
	run->AddTask(kalman);
	FairTask* l1 = new CbmL1();
	run->AddTask(l1);
	CbmStsTrackFinder* trackFinder    = new CbmL1StsTrackFinder();
//	CbmStsTrackFinder* trackFinder    = new CbmStsTrackFinderIdeal();
	FairTask* findTracks = new CbmStsFindTracks(iVerbose, trackFinder);
	run->AddTask(findTracks);

	FairTask* stsMatchTracks = new CbmStsMatchTracks("STSMatchTracks", iVerbose);
	run->AddTask(stsMatchTracks);

	CbmStsTrackFitter* trackFitter = new CbmStsKFTrackFitter();
	FairTask* fitTracks = new CbmStsFitTracks("STS Track Fitter", trackFitter, iVerbose);
	run->AddTask(fitTracks);
	// ------------------------------------------------------------------------

	if (IsMuch(parFile)) {
	// ----- MUCH reconstruction   --------------------------------------------
		CbmMuchDigitizeSimpleGem* muchDigitize = new CbmMuchDigitizeSimpleGem("MuchDigitize", muchDigiFile.Data(), iVerbose);
		run->AddTask(muchDigitize);
		CbmMuchDigitizeStraws* strawDigitize = new CbmMuchDigitizeStraws("MuchDigitizeStraws", muchDigiFile.Data(), iVerbose);
		run->AddTask(strawDigitize);

		CbmMuchFindHitsSimpleGem* muchFindHits = new CbmMuchFindHitsSimpleGem("MuchFindHits", muchDigiFile.Data(), iVerbose);
		run->AddTask(muchFindHits);
		CbmMuchFindHitsStraws* strawFindHits = new CbmMuchFindHitsStraws("MuchFindHitsStraws", muchDigiFile.Data(), iVerbose);
		run->AddTask(strawFindHits);

		CbmMuchTrackFinder* muchTrackFinder = new CbmMuchTrackFinderIdeal();
		CbmMuchFindTracks* muchFindTracks = new CbmMuchFindTracks("Much Track Finder");
		muchFindTracks->UseFinder(muchTrackFinder);
		run->AddTask(muchFindTracks);

		CbmMuchMatchTracks* muchMatchTracks = new CbmMuchMatchTracks();
		run->AddTask(muchMatchTracks);
	// ------------------------------------------------------------------------
	}

	if (IsTrd(parFile)){
	// ----- TRD reconstruction -- --------------------------------------------
		// Update of the values for the radiator F.U. 17.08.07
		Int_t trdNFoils    = 130;      // number of polyetylene foils
		Float_t trdDFoils = 0.0013;    // thickness of 1 foil [cm]
		Float_t trdDGap   = 0.02;      // thickness of gap between foils [cm]
		Bool_t simpleTR = kTRUE;       // use fast and simple version for TR
									   // production

		CbmTrdRadiator *radiator = new CbmTrdRadiator(simpleTR , trdNFoils,	 trdDFoils, trdDGap);

                CbmTrdHitProducerSmearing* trdHitProd = new CbmTrdHitProducerSmearing(radiator);
                run->AddTask(trdHitProd);

		CbmTrdTrackFinder* trdTrackFinder    = new CbmTrdTrackFinderIdeal();
		CbmTrdFindTracks* trdFindTracks = new CbmTrdFindTracks("TRDTrackFinder");
		trdFindTracks->UseFinder(trdTrackFinder);
		run->AddTask(trdFindTracks);

		CbmTrdMatchTracks* trdMatchTracks = new CbmTrdMatchTracks(1);
		run->AddTask(trdMatchTracks);
	// ------------------------------------------------------------------------
	}

	if (IsTof(parFile)) {
	// ------ TOF reconstruction ----------------------------------------------
		CbmTofHitProducer* tofHitProd = new CbmTofHitProducer("TOF HitProducer", 1);
		run->AddTask(tofHitProd);
	// ------------------------------------------------------------------------
	}

	// ------ Ideal global tracking -------------------------------------------
	CbmLitFindGlobalTracksIdeal* findGlobal = new CbmLitFindGlobalTracksIdeal();
	run->AddTask(findGlobal);
	// ------------------------------------------------------------------------

	// -----   Primary vertex finding   ---------------------------------------
	CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
	CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
	run->AddTask(findVertex);
	// ------------------------------------------------------------------------


   if (IsRich(parFile)) {
     // ---------------------RICH Hit Producer ----------------------------------
     Double_t richPmtRad = 0.4; // PMT radius [cm]
     Double_t richPmtDist = 0.; // Distance between PMTs [cm]
     Int_t richDetType = 4; // Detector type Hamamatsu H8500-03 (no WLS)
     Int_t richNoise = 220; // Number of noise points per event
     Double_t richCollEff = 1.0; // Collection Efficiency of PMT electron optics
     Double_t richSMirror = 0.06; // Sigma for additional point smearing due to light scattering in mirror

     CbmRichHitProducer* richHitProd = new CbmRichHitProducer(richPmtRad,
         richPmtDist, richDetType, richNoise, iVerbose, richCollEff, richSMirror);
     run->AddTask(richHitProd);
     //--------------------------------------------------------------------------

     //----------------------RICH Track Extrapolation ---------------------------
     Int_t richNSts = 4; // minimum number of STS hits for extrapolation
     Double_t richZPos = 300.; // z position for extrapolation [cm]
     CbmRichTrackExtrapolation* richExtra = new CbmRichTrackExtrapolationKF(
         richNSts, iVerbose);
     CbmRichExtrapolateTracks* richExtrapolate = new CbmRichExtrapolateTracks();
     richExtrapolate->UseExtrapolation(richExtra, richZPos);
     run->AddTask(richExtrapolate);
     //--------------------------------------------------------------------------

     //--------------------- Rich Track Projection to photodetector -------------
     Int_t richZFlag = 1; // Projetion from IM plane (default)
     CbmRichProjectionProducer* richProj = new CbmRichProjectionProducer(iVerbose, richZFlag);
     run->AddTask(richProj);
     //--------------------------------------------------------------------------

     //--------------------- RICH Ring Finding ----------------------------------
     TString richGeoType = "compact";//choose between compact or large
     CbmRichRingFinderHough* richFinder = new CbmRichRingFinderHough(iVerbose,   richGeoType);
     CbmRichFindRings* richFindRings = new CbmRichFindRings();
     richFindRings->UseFinder(richFinder);
     run->AddTask(richFindRings);
     //--------------------------------------------------------------------------

     //-------------------- RICH Ring Fitting -----------------------------------
     CbmRichRingFitter* richFitter = new CbmRichRingFitterEllipseTau(iVerbose,   1, richGeoType);
     CbmRichFitRings* fitRings = new CbmRichFitRings("", "", richFitter);
     run->AddTask(fitRings);
     //--------------------------------------------------------------------------

     // ------------------- RICH Ring matching  ---------------------------------
     CbmRichMatchRings* matchRings = new CbmRichMatchRings(iVerbose);
     run->AddTask(matchRings);
     // -------------------------------------------------------------------------

     //--------------------- RICH ring-track assignment ------------------------
     Double_t richDistance = 10.; // Max. dist. ring centre to track [cm]
     Int_t richNPoints = 5; // Minmum number of hits on ring
     CbmRichRingTrackAssign* richAssign = new CbmRichRingTrackAssignClosestD(
         richDistance, richNPoints, iVerbose);
     CbmRichAssignTrack* assignTrack = new CbmRichAssignTrack();
     assignTrack->UseAssign(richAssign);
     run->AddTask(assignTrack);
     // ------------------------------------------------------------------------
   }

	// ------- Track finding QA check   ---------------------------------------
   CbmLitTrackingQa* trackingQa = new CbmLitTrackingQa();
   trackingQa->SetMinNofPointsSts(4);
   trackingQa->SetMinNofPointsTrd(8);
   trackingQa->SetMinNofPointsMuch(16);
   trackingQa->SetMinNofPointsTof(1);
   trackingQa->SetQuota(0.7);
   trackingQa->SetMinNofHitsTrd(8);
   trackingQa->SetMinNofHitsMuch(16);
   trackingQa->SetVerbose(0);
   trackingQa->SetMinNofHitsRich(7);
   trackingQa->SetQuotaRich(0.6);
   trackingQa->SetOutputDir(std::string(imageDir));
   run->AddTask(trackingQa);
	// ------------------------------------------------------------------------

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
	parIo1->open(parFile.Data());
	parIo2->open(parFileList, "in");
	rtdb->setFirstInput(parIo1);
	rtdb->setSecondInput(parIo2);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	// -----   Intialise and run   --------------------------------------------
	run->Init();
	run->Run(0, nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << globalTracksFile << endl;
	cout << "Parameter file is " << parFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
}
