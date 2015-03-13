// $Id: TofLinkDef.h,v 1.3 2006/03/07 11:51:55 friese Exp $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
 
#pragma link C++ class CbmGeoTof;
#pragma link C++ class CbmGeoTofPar;
#pragma link C++ class CbmTof+;
#pragma link C++ class CbmTofContFact;
#pragma link C++ class CbmTofHitProducer+;
#pragma link C++ class CbmTofHitProducerNew+;
#pragma link C++ class CbmTofHitProducerIdeal+;
#pragma link C++ class CbmTofGeoHandler+;
#pragma link C++ class CbmTofDigiPar;
#pragma link C++ class CbmTofCell;
#pragma link C++ class CbmTofCreateDigiPar+;

#pragma link C++ class CbmTofDigiBdfPar+;
#pragma link C++ class CbmTofDigitizerBDF+;

#pragma link C++ class CbmTofSimpClusterizer+;
//#pragma link C++ class CbmTofTestBeamClusterizer+;

#pragma link C++ class CbmTofTests+;
//#pragma link C++ class CbmTofAnaTestbeam+;

#endif

