#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;


// Setup
//#pragma link C++ class CbmStsAddress;
#pragma link C++ class CbmStsElement;
#pragma link C++ class CbmStsModule;
#pragma link C++ class CbmStsSensor;
#pragma link C++ class CbmStsSensorConditions;
#pragma link C++ class CbmStsSensorPoint;
#pragma link C++ class CbmStsSensorType;
#pragma link C++ class CbmStsSetup;
#pragma link C++ class CbmStsStation;

// Monte-Carlo
#pragma link C++ class CbmStsMC;
#pragma link C++ class CbmStsSensorFactory;
#pragma link C++ class CbmStsTrackStatus;

// Digitization
#pragma link C++ class CbmStsDigitize;
#pragma link C++ class CbmStsSensorTypeDssd;
#pragma link C++ class CbmStsSensorTypeDssdIdeal;
#pragma link C++ class CbmStsSensorTypeDssdReal;
#pragma link C++ class CbmStsSignal;

// Reconstruction
#pragma link C++ class CbmStsClusterFinderSimple;
#pragma link C++ class CbmStsFindClusters;
#pragma link C++ class CbmStsFindHits;

// Legacy
#pragma link C++ class CbmStsClusterFinder_old;
#pragma link C++ class CbmStsDigitize_old;
#pragma link C++ class CbmStsFindHits_old;
#pragma link C++ class CbmStsSensor_old;
#pragma link C++ class CbmStsStation_old;

// Enumerators
#pragma link C++ enum EStsElementLevel;

// Others
#pragma link C++ class CbmGeoSts+;
#pragma link C++ class CbmGeoStsPar;
#pragma link C++ class CbmStsContFact;
#pragma link C++ class CbmStsDigiPar;
#pragma link C++ class CbmStsDigiScheme+;
#pragma link C++ class CbmStsDigitizeTb;
#pragma link C++ class CbmStsFindTracks+;
#pragma link C++ class CbmStsFindTracksQa+;
#pragma link C++ class CbmStsFitTracks+;
#pragma link C++ class CbmStsHitProducerIdeal+;
#pragma link C++ class CbmStsIdealMatchHits+;
#pragma link C++ class CbmStsMatchHits+;
#pragma link C++ class CbmStsMatchTracks+;
#pragma link C++ class CbmStsParAsciiFileIo;
#pragma link C++ class CbmStsParRootFileIo;
#pragma link C++ class CbmStsRadTool;
#pragma link C++ class CbmStsSector+;
#pragma link C++ class CbmStsSensorDigiPar+;
#pragma link C++ class CbmStsSectorDigiPar+;
#pragma link C++ class CbmStsStationDigiPar+;
#pragma link C++ class CbmStsTrackFinderIdeal+;
#pragma link C++ class CbmStsTrackFitterIdeal+;
#pragma link C++ class CbmStsSimulationQa+;
#pragma link C++ class CbmStsFindHitsQa+;
#pragma link C++ class CbmStsReconstructionQa+;
#pragma link C++ class CbmStsDigitizeQa+;

/* Obsolete
#pragma link C++ class CbmStsIdealDigitize;
#pragma link C++ class CbmStsIdealFindHits+;
#pragma link C++ class CbmStsRealClusterFinder;
#pragma link C++ class CbmStsRealDigitize;
#pragma link C++ class CbmStsRealFindHits+;
*/


#endif /* __CINT__ */

