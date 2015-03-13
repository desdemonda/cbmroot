// $Id: TrdLinkDef.h,v 1.10 2006/06/20 09:39:59 kresan Exp $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class  CbmTrd+;
#pragma link C++ class  CbmTrdHitProducerIdeal+;
#pragma link C++ class  CbmTrdHitProducerSmearing+;
#pragma link C++ class  CbmTrdHitProducerQa+;
#pragma link C++ class  CbmTrdFindTracks+;
//#pragma link C++ class  CbmTrdTrackFinder+;
#pragma link C++ class  CbmTrdTrackFinderIdeal+;
#pragma link C++ class  CbmTrdFitTracks+;
//#pragma link C++ class  CbmTrdTrackFitter+;
#pragma link C++ class  CbmTrdTrackFitterIdeal+;
#pragma link C++ class  CbmGeoTrd+;
#pragma link C++ class  CbmGeoTrdPar;
//#pragma link C++ class  CbmTrdDigitizerMathieson;
#pragma link C++ class  CbmTrdDigitizerPRF;
#pragma link C++ class  CbmTrdDigitizerPRF_TB;
#pragma link C++ class  CbmTrdSPADIC;
#pragma link C++ class  CbmTrdHitRateQa;
#pragma link C++ class  CbmTrdHitRateFastQa;
#pragma link C++ class  CbmTrdHitDensityQa;
#pragma link C++ class  CbmTrdOccupancyQa;
#pragma link C++ class  CbmTrdDigitizerPRFQa;
#pragma link C++ class  CbmTrdClusterizerFastQa;
#pragma link C++ class  CbmTrdHitProducerClusterQa;
//#pragma link C++ class  CbmTrdCluster;
#pragma link C++ class  CbmTrdContFact;
#pragma link C++ class  CbmTrdRadiator;
//#pragma link C++ class  CbmTrdMatchTracks+;
#pragma link C++ class  CbmTrdSetTracksPidWkn+;
#pragma link C++ class  CbmTrdSetTracksPidModWkn+;
#pragma link C++ class  CbmTrdSetTracksPidLike+;
#pragma link C++ class  CbmTrdSetTracksPidANN+;
#pragma link C++ class  CbmTrdCreatePidLike+;
#pragma link C++ class  CbmTrdTracksPidQa+;
#pragma link C++ class  CbmTrdGas+;
#pragma link C++ class  CbmTrdDigitizer+;
#pragma link C++ class  CbmTrdModule+;
#pragma link C++ class  CbmTrdAsic+;
#pragma link C++ class  CbmTrdCreateDigiPar+;
#pragma link C++ class  CbmTrdDigiPar+;
#pragma link C++ class  CbmTrdHitProducerDigi+;
//#pragma link C++ class  CbmTrdDetectorId+;
#pragma link C++ class CbmTrdElectronsTrainAnn+;
#pragma link C++ class CbmTrdUtils+;
//#pragma link C++ class CbmTrdClusterFinder+;
#pragma link C++ class CbmTrdClusterFinderFast+;
#pragma link C++ class CbmTrdClusterFinderDPB+;
//#pragma link C++ class CbmTrdSimpleHitProducerCluster+;
#pragma link C++ class CbmTrdHitProducerCluster+;
#pragma link C++ class CbmTrdGeoHandler+;
//#pragma link C++ class CbmTrdDigiWriteoutBuffer+;
//#pragma link C++ class CbmTrdDigiRingSorter+;
//#pragma link C++ class CbmTrdDigiSorterTask+;
#pragma link C++ class CbmTrdQa+;
#pragma link C++ class CbmTrdRecoQa+;

//#pragma link C++ class CbmTrdRawToDigiSpadic03+;
//#pragma link C++ class TRocEvent+;
//#pragma link C++ class TSpadicEvent+;
//#pragma link C++ class TMbsCrateEvent+;
//#pragma link C++ class TBeamMonitorEvent+;
//#pragma link C++ class TCernOct12UnpackEvent+;
//#pragma link C++ class TTriglogEvent+;
#endif
