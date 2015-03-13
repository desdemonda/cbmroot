#include "CbmStsBLCalibrator.h"
#include "FairLogger.h"
#include "TClonesArray.h"
#include "CbmStsDigi.h"
#include "CbmStsAddress.h"



CbmStsBLCalibrator::CbmStsBLCalibrator()
:FairTask("CbmStsBLCalibrator"),
 fBaselineDigis(NULL),
 fBaselines()
{
  LOG(DEBUG) << "Default Constructor of CbmStsBLCalibrator" << FairLogger::endl;
}



CbmStsBLCalibrator::~CbmStsBLCalibrator()
{
  LOG(DEBUG) << "Destructor of CbmStsBLCalibrator" << FairLogger::endl;
}



void CbmStsBLCalibrator::SetParContainers()
{
  // Load all necessary parameter containers from the runtime data base

  /*
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  <CbmStsBLCalibratorDataMember> = (<ClassPointer>*)
    (rtdb->getContainer("<ContainerName>"));
  */
}



InitStatus CbmStsBLCalibrator::Init()
{
  // Get a handle from the IO manager
  FairRootManager* ioman = FairRootManager::Instance();

  // Get a pointer to the previous already existing data level

  fBaselineDigis = (TClonesArray*) ioman->GetObject("StsBaselineDigi");
  if ( ! fBaselineDigis ) {
    LOG(ERROR) << "No input data object StsBaselineDigi found!" << FairLogger::endl;
    LOG(ERROR) << " Task CbmStsBLCalibrator will be inactive" << FairLogger::endl;
    return kERROR;
  }

  Int_t kNStations = 3; // TODO: from where do I get the number of stations?
  Int_t kNSides = 2; // TODO: from where do I get the number of sides per station?
  Int_t kNStrips = 1024; // TODO: from where do I get the number of strips per station side?

  fBaselines.resize( kNStations );

  for( Int_t iStation = 0; iStation <  fBaselines.size(); iStation++ ) {
    fBaselines.at( iStation ).resize( kNSides );
    for( Int_t iSide = 0; iSide < fBaselines.at( iStation ).size(); iSide++ ) {
      fBaselines.at( iStation ).at( iSide ).resize( kNStrips );
      for( Int_t iStrip = 0; iStrip < fBaselines.at( iStation ).at( iSide ).size(); iStrip++ ) {
        const char * nametitle = Form( "blhist_sta%d_side%d_str%d", iStation, iSide, iStrip );
        fBaselines.at( iStation ).at( iSide ).at( iStrip ) = new TH1F( nametitle, nametitle, kBaselineNBins, kBaselineMinAdc, kBaselineMaxAdc );
      }
    }
  }

  return kSUCCESS;
}



InitStatus CbmStsBLCalibrator::ReInit()
{
	fLogger->Debug(MESSAGE_ORIGIN,"Initilization of CbmStsBLCalibrator");
	return kSUCCESS;
}



void CbmStsBLCalibrator::Exec(Option_t* option)
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
      CbmStsDigi * digi = static_cast< CbmStsDigi * >( fBaselineDigis->At( iDigi ) );
      Int_t station = CbmStsAddress::GetElementId( digi->GetAddress(), kStsStation );
      Int_t side = CbmStsAddress::GetElementId( digi->GetAddress(), kStsSide );
      Int_t strip = CbmStsAddress::GetElementId( digi->GetAddress(), kStsChannel );
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



void CbmStsBLCalibrator::Finish()
{
   
  for( Int_t iStation = 0; iStation <  fBaselines.size(); iStation++ ) {
    for( Int_t iSide = 0; iSide < fBaselines.at( iStation ).size(); iSide++ ) {
      for( Int_t iStrip = 0; iStrip < fBaselines.at( iStation ).at( iSide ).size(); iStrip++ ) {
	TH1F * baselineHist = fBaselines.at( iStation ).at( iSide ).at( iStrip );
	if( baselineHist ) {
	  delete baselineHist;
	}
	baselineHist = 0;
      }
    }
  }
  fBaselines.clear();
}



Double_t CbmStsBLCalibrator::GetBlPos( TH1F * hist )
{
  Int_t medianBin = 1;
  
  Double_t * integral = hist->GetIntegral();
  while( integral[ medianBin + 1 ] <= 0.5 ) {
    medianBin++;
  }
  
  return hist->GetXaxis()->GetBinCenter( medianBin );
}


ClassImp(CbmStsBLCalibrator)
