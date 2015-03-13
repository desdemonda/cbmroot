#include "CbmFiberHodoBLCalibrator.h"

#include "CbmFiberHodoDigi.h"
#include "CbmFiberHodoAddress.h"

#include "FairLogger.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"

CbmFiberHodoBLCalibrator::CbmFiberHodoBLCalibrator()
:FairTask("CbmFiberHodoBLCalibrator"),
 fBaselineDigis(NULL),
 fBaselines()
{
  LOG(DEBUG) << "Default Constructor of CbmFiberHodoBLCalibrator" << FairLogger::endl;
}


CbmFiberHodoBLCalibrator::~CbmFiberHodoBLCalibrator()
{
  LOG(DEBUG) << "Destructor of CbmFiberHodoBLCalibrator" << FairLogger::endl;
  for( Int_t iStation = 0; iStation <  fBaselines.size(); iStation++ ) {
    for( Int_t iSide = 0; iSide < fBaselines.at( iStation ).size(); iSide++ ) {
      for( Int_t iStrip = 0; iStrip < fBaselines.at( iStation ).at( iSide ).size(); iStrip++ ) {
	TH1F * baselineHist = fBaselines.at( iStation ).at( iSide ).at( iStrip );
	if( baselineHist ) {
	  delete baselineHist;
	}
	baselineHist = NULL;
      }
    }
  }
  fBaselines.clear();
}



void CbmFiberHodoBLCalibrator::SetParContainers()
{

  //Load all necessary parameter containers from the runtime data base
  FairRun* run = FairRunAna::Instance();
  if (NULL == run) {
    run = FairRunOnline::Instance();
  }
  if (NULL == run) {
    LOG(FATAL) << "No RunManager found." << FairLogger::endl;
  }

  FairRuntimeDb* rtdb=run->GetRuntimeDb();
  if (NULL == rtdb) LOG(FATAL) << "No instance of the runtime database found." 
			       << FairLogger::endl;

//  fFirstParameters = (CbmBeamTimeFirstPar*)
  //     (rtdb->getContainer("CbmBeamTimeFirstPar"));
  //    fFirstParameters->Print();  
}



InitStatus CbmFiberHodoBLCalibrator::Init()
{
  // Get a handle from the IO manager
  FairRootManager* ioman = FairRootManager::Instance();

  // Get a pointer to the previous already existing data level

  fBaselineDigis = (TClonesArray*) ioman->GetObject("HodoBaselineDigi");
  if ( ! fBaselineDigis ) {
    LOG(ERROR) << "No input data object HodoBaselineDigi found!" << FairLogger::endl;
    LOG(ERROR) << " Task CbmFiberHodoBLCalibrator will be inactive" << FairLogger::endl;
    return kERROR;
  }

  Int_t kBaselineMinAdc = 0;
  Int_t kBaselineMaxAdc = 4096;
  Int_t kBaselineNBins = 4096;

  Int_t kNStations = 2; // TODO: from where do I get the number of stations?
  Int_t kNSides = 2; // TODO: from where do I get the number of sides per station?
  Int_t kNStrips = 64; // TODO: from where do I get the number of strips per station side?

  fBaselines.resize( kNStations );

  for( Int_t iStation = 0; iStation <  fBaselines.size(); iStation++ ) {
    fBaselines.at( iStation ).resize( kNSides );
    for( Int_t iSide = 0; iSide < fBaselines.at( iStation ).size(); iSide++ ) {
      fBaselines.at( iStation ).at( iSide ).resize( kNStrips );
      for( Int_t iStrip = 0; iStrip < fBaselines.at( iStation ).at( iSide ).size(); iStrip++ ) {
        const char * nametitle = Form( "fh_blhist_sta%d_side%d_str%d", iStation, iSide, iStrip );
        fBaselines.at( iStation ).at( iSide ).at( iStrip ) = new TH1F( nametitle, nametitle, kBaselineNBins, kBaselineMinAdc, kBaselineMaxAdc );
      }
    }
  }

  return kSUCCESS;
}



InitStatus CbmFiberHodoBLCalibrator::ReInit()
{
	LOG(DEBUG) << "Initilization of CbmFiberHodoBLCalibrator" << FairLogger::endl;
	return kSUCCESS;
}



void CbmFiberHodoBLCalibrator::Exec(Option_t* option)
{


  Int_t nBaselineEntries = fBaselineDigis->GetEntriesFast();
  if( nBaselineEntries ) { // TODO: Check here in a proper way if the event is a baseline event


    /* Baseline data should be taken from only one iteration of the baseline measurement
     * (i.e. data from several different iterations of baseline calibration should not be mixed up)
     * Therefore the baseline histograms should be zeroed before filling them with the data of the
     * next calibration iteration */
    for( Int_t iStation = 0; iStation <  fBaselines.size(); iStation++ ) {
      for( Int_t iSide = 0; iSide < fBaselines.at( iStation ).size(); iSide++ ) {
	for( Int_t iStrip = 0; iStrip < fBaselines.at( iStation ).at( iSide ).size(); iStrip++ ) {
	  fBaselines.at( iStation ).at( iSide ).at( iStrip )->Reset();
	}
      }
    }
    
    for( Int_t iDigi = 0; iDigi < nBaselineEntries; ++iDigi ) {
      CbmFiberHodoDigi* digi = static_cast< CbmFiberHodoDigi * >( fBaselineDigis->At( iDigi ) );
   
      Int_t address = digi->GetAddress();
      Int_t station = CbmFiberHodoAddress::GetStationId(address);
      Int_t side = CbmFiberHodoAddress::GetSideId(address);
      Int_t strip = CbmFiberHodoAddress::GetStripId(address);
      Double_t adc = digi->GetCharge();

      fBaselines.at( station ).at( side ).at( strip )->Fill( adc );
    }
    
    for( Int_t iStation = 0; iStation <  fBaselines.size(); iStation++ ) {
      for( Int_t iSide = 0; iSide < fBaselines.at( iStation ).size(); iSide++ ) {
	for( Int_t iStrip = 0; iStrip < fBaselines.at( iStation ).at( iSide ).size(); iStrip++ ) {
	  TH1F * blHist = fBaselines.at( iStation ).at( iSide ).at( iStrip );
	  if( blHist->GetEntries() ) {
	    Double_t bl = GetBlPos( blHist );
	    LOG(INFO) << "Baseline of station " << iStation << " side " << iSide << " strip " << iStrip << " is " << bl << FairLogger::endl;
	  }
	}
      }
    }
  }

}



void CbmFiberHodoBLCalibrator::Finish()
{
}



Double_t CbmFiberHodoBLCalibrator::GetBlPos( TH1F * hist )
{
  Int_t medianBin = 1;
  
  Double_t * integral = hist->GetIntegral();
  while( integral[ medianBin + 1 ] <= 0.5 ) {
    medianBin++;
  }
  
  return hist->GetXaxis()->GetBinCenter( medianBin );
}


ClassImp(CbmFiberHodoBLCalibrator)
