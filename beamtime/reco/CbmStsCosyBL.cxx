#include "CbmStsCosyBL.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "CbmTbEvent.h"

#include "CbmStsDigi.h"
#include "CbmStsAddress.h"
#include "CbmFiberHodoAddress.h"
#include "CbmFiberHodoDigi.h"
#include "CbmAuxDigi.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include <iostream>

using namespace std;


// ---- Default constructor -------------------------------------------
CbmStsCosyBL::CbmStsCosyBL()
  :FairTask("CbmStsCosyBL",1),fDigis(NULL),fTriggeredMode(kFALSE),
   fTriggeredStation(2)
   { 
     fChain = new TChain("cbmsim");
     outFile=NULL;
     fLogger->Debug(MESSAGE_ORIGIN,"Defaul Constructor of CbmStsCosyBL");
     cDigis = new TClonesArray("CbmStsDigi");
}
   
// ---- Destructor ----------------------------------------------------
CbmStsCosyBL::~CbmStsCosyBL()
{
  if(fDigis){
    fDigis->Delete();
    fBLDigis->Delete();
    hBLDigis->Delete();
    hDigis->Delete();
    auxDigis->Delete();
    delete fDigis;
    delete fBLDigis;
    delete hBLDigis;
    delete hDigis;
    delete auxDigis;
  }
  fLogger->Debug(MESSAGE_ORIGIN,"Destructor of CbmStsCosyBL");
}

// ----  Initialisation  ----------------------------------------------
void CbmStsCosyBL::SetParContainers()
{
  fLogger->Debug(MESSAGE_ORIGIN,"SetParContainers of CbmStsCosyBL");
  // Load all necessary parameter containers from the runtime data base
  
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  /*
  <CbmStsCosyBLDataMember> = (<ClassPointer>*)
    (rtdb->getContainer("<ContainerName>"));
  */
}

// ---- Init ----------------------------------------------------------
InitStatus CbmStsCosyBL::Init()
{
  fLogger->Debug(MESSAGE_ORIGIN,"Initilization of CbmStsCosyBL");
  
  // Get a handle from the IO manager
  FairRootManager* ioman = FairRootManager::Instance();
  
  // Get a pointer to the previous already existing data level
  
  fDigis = (TClonesArray*) ioman->GetObject("StsDigi");
  fBLDigis = (TClonesArray*) ioman->GetObject("StsBaselineDigi");
  hBLDigis = (TClonesArray*) ioman->GetObject("HodoBaselineDigi");
  hDigis = (TClonesArray*) ioman->GetObject("HodoDigi");
  auxDigis= (TClonesArray*) ioman->GetObject("AuxDigi");   

  if ( ! fDigis ) 
    {
      fLogger->Error(MESSAGE_ORIGIN,"No InputDataLevelName array!\n CbmStsCosyBL will be inactive");
      return kERROR;
    }
  
  calib = kFALSE;
  hodo_calib = kFALSE;
  
  Int_t NStations = 3; // TODO: from where do I get the number of stations?
  Int_t NSides = 2; // TODO: from where do I get the number of sides per station?
  Int_t NStrips = 1024; // TODO: from where do I get the number of strips per station side?
  
  base_line_array.resize(NStations );
  for( Int_t iStation = 0; iStation <  base_line_array.size(); iStation++ ) 
    {
      base_line_array.at( iStation ).resize( NSides );
      for( Int_t iSide = 0; iSide <  base_line_array.at( iStation ).size(); iSide++ ) 
	{
	  base_line_array.at( iStation ).at( iSide ).resize( NStrips );
	  for( Int_t iStrip = 0; iStrip <  base_line_array.at( iStation ).at( iSide ).size(); iStrip++ ) 
	    {
	      base_line_array.at( iStation ).at( iSide ).at(iStrip)=0;
	    }
	}
      }

  Int_t hStations = 2; // TODO: from where do I get the number of stations?
  Int_t hSides = 2; // TODO: from where do I get the number of sides per station?
  Int_t hStrips = 64; // TODO: from where do I get the number of strips per station side?
  
  hodo_BL_array.resize(hStations );
  for( Int_t iStation = 0; iStation <  hodo_BL_array.size(); iStation++ ) 
    {
      hodo_BL_array.at( iStation ).resize(hSides );
      for( Int_t iSide = 0; iSide <  hodo_BL_array.at( iStation ).size(); iSide++ ) 
	{
	  hodo_BL_array.at( iStation ).at( iSide ).resize( hStrips );
	  for( Int_t iStrip = 0; iStrip <  hodo_BL_array.at( iStation ).at( iSide ).size(); iStrip++ ) 
	    {
	      hodo_BL_array.at( iStation ).at( iSide ).at(iStrip)=0;
	    }
	}
    }
  
  
  for(int i =0; i<3;i++)
    {
      for(int s=0; s<2; s++)
	{
	  Char_t inName[200];
	  sprintf(inName,"base_line_vs_ch_STS%i_side%i",i,s);
	  TString f1(inName);
	  baseline_ch[i][s] = new TH2F(f1, f1, 256, 0,256, 2000, 1000, 3000);

	  sprintf(inName,"fiber_distr_system%i_side%i",i,s);
	  TString f2(inName);
	  hits_layer[i][s] = new TH1F(f2, f2, 256, 0,256);
	}
    }
  hodo_layer = new TH1F("layers", "layers", 100, 0,100);
  hodo_hits = new TH1F("fibers", "fibers", 1000, 0,1000);
  hodo_system = new TH1F("system", "system", 1000, 0,1000);

  aux_digi = new TH1F("aux_digi", "aux_digi", 100, 0,100);
  aux_time_diff = new TH1F("aux_time_diff", "aux_time_diff", 100, 0,100);
  aux_time = new TH1F("aux_time", "aux_time_diff", 10000, 0,10000);
 
  cDigis = new TClonesArray("CbmStsDigi", 100);
  ioman->Register("StsCalibDigi", "Calibrated", cDigis, kTRUE);
  chDigis = new TClonesArray("CbmFiberHodoDigi", 100);
  ioman->Register("HodoCalibDigi", "HodoCalibrated", chDigis, kTRUE);
  return kSUCCESS;
  
}

// ---- ReInit  -------------------------------------------------------
InitStatus CbmStsCosyBL::ReInit()
{
  fLogger->Debug(MESSAGE_ORIGIN,"Initilization of CbmStsCosyBL");
  return kSUCCESS;
}

// ---- Exec ----------------------------------------------------------
void CbmStsCosyBL::Exec(Option_t* option)
{

  cDigis->Clear();
  chDigis->Clear();

  double hit_layer[2][2];
  
  for(int k=0;k<2;k++)
    {  
      for(int m=0;m<2;m++)
	hit_layer[k][m]=0;
    }

  int fNDigis =0;
  int hNDigis =0;
  if (0 < fBLDigis->GetEntries() && 0 < hBLDigis->GetEntries() ) 
    {
      BaseLine(fBLDigis, base_line_array);      
      HodoBaseLine(hBLDigis,hodo_BL_array);
      for(int i =0; i<3; i++)
	{
	  for(int s=0;s<2;s++)
	    {
	      for(int ch=0; ch<256; ch++)
		baseline_ch[i][s] ->Fill(ch, base_line_array.at(i).at(s).at(ch));
	    }
	}
    }
  
  else
    {
      if(calib==kTRUE && hodo_calib ==kTRUE)
	{    
	  Int_t nofSTS = fDigis->GetEntries();
	  Int_t nofHodo = hDigis->GetEntries();
	  
	  CbmStsDigi* StsDigi = NULL;
	  CbmFiberHodoDigi* HodoDigi = NULL;
	  for (Int_t iDigi=0; iDigi < nofHodo; iDigi++ ) 
	    {
	      HodoDigi = (CbmFiberHodoDigi*) hDigis->At(iDigi);
	      
	      Int_t system = CbmFiberHodoAddress::GetStationId(HodoDigi->GetAddress());
	      Int_t layer = CbmFiberHodoAddress::GetSideId(HodoDigi->GetAddress());
	      Int_t fiberNr = CbmFiberHodoAddress::GetStripId( HodoDigi->GetAddress());
	      hodo_system->Fill(system);
	      hodo_layer->Fill(layer);
	      hodo_hits->Fill(fiberNr);
	      hits_layer[system][layer]->Fill(fiberNr);

	      double adc = -HodoDigi->GetCharge() + hodo_BL_array.at(system).at(layer).at(fiberNr);
	      if(adc>0)
		{
		  new ( (*chDigis)[hNDigis] )CbmFiberHodoDigi(HodoDigi->GetAddress(), adc,HodoDigi->GetTime());
		  hNDigis++;
		  hit_layer[system][layer]++;
		}
	      else return;
	      
	    }
	  
	  int sts[3]={0,0,0};
	  int side_sts[3][2]={0,0,0,0,0,0};
	  
	  for (Int_t iDigi=0; iDigi < nofSTS; iDigi++ ) 
	    {
	      StsDigi = (CbmStsDigi*) fDigis->At(iDigi);
	      
	      int station = CbmStsAddress::GetElementId(StsDigi->GetAddress(),kStsStation);
	      int side = CbmStsAddress::GetElementId(StsDigi->GetAddress(),kStsSide);
	      int ch = CbmStsAddress::GetElementId(StsDigi->GetAddress(),kStsChannel);
	      
	      Double_t limit = 0;
	      if(fTriggeredMode && station==fTriggeredStation)limit = -130;
	      
	      double adc = -StsDigi->GetCharge() + base_line_array.at(station).at(side).at(ch) - limit;
	      if(adc>0)
		{
		  new ( (*cDigis)[fNDigis] ) CbmStsDigi(StsDigi->GetAddress(), StsDigi->GetTime(), (UShort_t)adc);
		  fNDigis++;
		}
	      else return;
	    }
	  
	}
      
    }
  

  
    
  fLogger->Debug(MESSAGE_ORIGIN,"Exec of CbmStsCosyBL");
//  Reset();
}

 void CbmStsCosyBL::FinishEvent()
 {
   //Reset();
   // cout << "---I ------------- FinishEvent" << endl;
}




// ---- Finish --------------------------------------------------------
void CbmStsCosyBL::Finish()
{
  fLogger->Debug(MESSAGE_ORIGIN,"Finish of CbmStsCosyBL");

  // Write standard file
  FairRun* ana = FairRunAna::Instance();
  if ( ! ana ) {
    ana = FairRunOnline::Instance();
  }
  TFile* fout = ana->GetOutputFile();
  hodo_layer->Write();
  hodo_hits->Write();
  hodo_system->Write();
  aux_digi->Write();
  aux_time_diff->Write();
  aux_time->Write();
 


 for(int i =0; i<3;i++)
    {
      for(int s=0; s<2; s++)	
	{
	  baseline_ch[i][s]->Write();
	  hits_layer[i][s]->Write();
	}
    }

  if (fout) fout->Write();
    
}

/*
Int_t CbmStsCosyBL::AddFile( const char* name ){
  if (name) {
    fChain->AddFile(name);
  }

}
*/

void CbmStsCosyBL::BaseLine(TClonesArray* fBaselineDigis, vector< vector < vector < double> > > base_line)
{

  //BLInit();
  Int_t kNStations = 3; // TODO: from where do I get the number of stations?
  Int_t kNSides = 2; // TODO: from where do I get the number of sides per station?
  Int_t kNStrips = 1024; // TODO: from where do I get the number of strips per station side?

  vector< vector < vector < TH1F * > > > fBaselines;
  fBaselines.resize( kNStations );
  
  for( Int_t iStation = 0; iStation <  fBaselines.size(); iStation++ ) 
    {
      fBaselines.at( iStation ).resize( kNSides );
      for( Int_t iSide = 0; iSide < fBaselines.at( iStation ).size(); iSide++ ) 
	{
	  fBaselines.at( iStation ).at( iSide ).resize( kNStrips );
	  for( Int_t iStrip = 0; iStrip < fBaselines.at( iStation ).at( iSide ).size(); iStrip++ ) 
	    {
	      const char * nametitle = Form( "blhist_sta%d_side%d_str%d", iStation, iSide, iStrip );
	      fBaselines.at( iStation ).at( iSide ).at( iStrip ) = new TH1F( nametitle, nametitle, kBaselineNBins, kBaselineMinAdc, kBaselineMaxAdc );
	    }
	}
    }
    
  cout << "---------CALIBRATION ---------ON ------------------- " << endl;
  calib=kTRUE;
  Int_t nBaselineEntries = fBaselineDigis->GetEntriesFast();
  if( nBaselineEntries ) 
    { // TODO: Check here in a proper way if the event is a baseline event
      
      /* Baseline data should be taken from only one iteration of the baseline measurement
       * (i.e. data from several different iterations of baseline calibration should not be mixed up)
       * Therefore the baseline histograms should be zeroed before filling them with the data of the
       * next calibration iteration */
      for( Int_t iStation = 0; iStation <  fBaselines.size(); iStation++ ) 
	{
	  for( Int_t iSide = 0; iSide < fBaselines.at( iStation ).size(); iSide++ ) 
	    {
	      for( Int_t iStrip = 0; iStrip < fBaselines.at( iStation ).at( iSide ).size(); iStrip++ ) 
		{
		  fBaselines.at( iStation ).at( iSide ).at( iStrip )->Reset();
		}
	    }
	}
      
      for( Int_t iDigi = 0; iDigi < nBaselineEntries; ++iDigi ) 
	{
	  CbmStsDigi * digi = static_cast< CbmStsDigi * >( fBaselineDigis->At( iDigi ) );
	  Int_t station = CbmStsAddress::GetElementId( digi->GetAddress(), kStsStation );
	  Int_t side = CbmStsAddress::GetElementId( digi->GetAddress(), kStsSide );
	  Int_t strip = CbmStsAddress::GetElementId( digi->GetAddress(), kStsChannel );
	  Double_t adc = digi->GetCharge();
	  fBaselines.at( station ).at( side ).at( strip )->Fill( adc );

	}
      
      for( Int_t iStation = 0; iStation <  fBaselines.size(); iStation++ ) 
	{
	  for( Int_t iSide = 0; iSide < fBaselines.at( iStation ).size(); iSide++ ) 
	    {
	      for( Int_t iStrip = 0; iStrip < fBaselines.at( iStation ).at( iSide ).size(); iStrip++ ) 
		{
		  TH1F * blHist = fBaselines.at( iStation ).at( iSide ).at( iStrip );
		  if( blHist->GetEntries() ) 
		    {
		      Double_t bl = GetBlPos( blHist );
		      base_line_array.at(iStation).at(iSide).at(iStrip)=bl;
		      //	    LOG(INFO) << "Baseline of station " << iStation << " side " << iSide << " strip " << iStrip << " is " << base_line_array.at(iStation).at(iSide).at(iStrip)
		      // << FairLogger::endl;
		    }
		  if(blHist)
		    {
		      blHist->Delete();
		    }
		}
	    }
	}
    }
  // BLClean();
 cout << "---------CALIBRATION ---------OFF ------------------- " << endl;  
 
}

void CbmStsCosyBL::HodoBaseLine(TClonesArray* fBaselineDigis, vector< vector < vector < double> > > base_line)
{

  Int_t hStations = 2; // TODO: from where do I get the number of stations?
  Int_t hSides = 2; // TODO: from where do I get the number of sides per station?
  Int_t hStrips = 64; // TODO: from where do I get the number of strips per station side?
  vector< vector < vector < TH1F * > > > fBaselines;
  
  fBaselines.resize(hStations );
  for( Int_t iStation = 0; iStation <  fBaselines.size(); iStation++ ) 
    {
      fBaselines.at( iStation ).resize(hSides );
      for( Int_t iSide = 0; iSide <  fBaselines.at( iStation ).size(); iSide++ ) 
	{
	  fBaselines.at( iStation ).at( iSide ).resize( hStrips );
	  for( Int_t iStrip = 0; iStrip <  fBaselines.at( iStation ).at( iSide ).size(); iStrip++ ) 
	    {
	      const char * nametitle = Form( "blhist_sta%d_side%d_str%d", iStation, iSide, iStrip );
	      fBaselines.at( iStation ).at( iSide ).at(iStrip)=new TH1F( nametitle, nametitle, kBaselineNBins, kBaselineMinAdc, kBaselineMaxAdc );
	    }
	}
    }

  cout << "---------HODO------------CALIBRATION ---------ON ------------------- " << endl;
  hodo_calib=kTRUE;
  Int_t nBaselineEntries = fBaselineDigis->GetEntriesFast();
  if( nBaselineEntries ) 
    { // TODO: Check here in a proper way if the event is a baseline event
      
      /* Baseline data should be taken from only one iteration of the baseline measurement
       * (i.e. data from several different iterations of baseline calibration should not be mixed up)
       * Therefore the baseline histograms should be zeroed before filling them with the data of the
       * next calibration iteration */
      for( Int_t iStation = 0; iStation <  fBaselines.size(); iStation++ ) 
	{
	  for( Int_t iSide = 0; iSide < fBaselines.at(iStation).size(); iSide++ ) 
	    {
	      for( Int_t iStrip = 0; iStrip < fBaselines.at(iStation).at( iSide ).size(); iStrip++ ) 
		{
		  fBaselines.at(iStation).at( iSide ).at( iStrip )->Reset();
		}
	    }
	}
      
      for( Int_t iDigi = 0; iDigi < nBaselineEntries; ++iDigi ) 
	{
	  CbmFiberHodoDigi * digi = static_cast< CbmFiberHodoDigi * >( fBaselineDigis->At( iDigi ) );
	  Int_t station = CbmFiberHodoAddress::GetStationId(digi->GetAddress());
	  Int_t side = CbmFiberHodoAddress::GetSideId(digi->GetAddress());
	  Int_t strip = CbmFiberHodoAddress::GetStripId(digi->GetAddress());

	  Double_t adc = digi->GetCharge();
	  fBaselines.at(station).at( side ).at( strip )->Fill( adc );
	  
	}
      for( Int_t iStation = 0; iStation <  fBaselines.size(); iStation++ ) 
	{
	  for( Int_t iSide = 0; iSide < fBaselines.at(iStation).size(); iSide++ ) 
	    {
	      for( Int_t iStrip = 0; iStrip < fBaselines.at(iStation).at( iSide ).size(); iStrip++ ) 
		{
		  TH1F * blHist = fBaselines.at(iStation).at( iSide ).at( iStrip );
		  if( blHist->GetEntries() ) 
		    {
		      Double_t bl = GetBlPos( blHist );
		      hodo_BL_array.at(iStation).at(iSide).at(iStrip)=bl;
		      //		  if(iSide ==0 || iSide ==1 || iSide==16 ||iSide ==17)
		      //		    LOG(INFO) << "Baseline of hodo " <<  " layer " << iSide << " strip " << iStrip << " is " << hodo_BL_array.at(iSide).at(iStrip) << FairLogger::endl;
		    }
		  if(blHist)
		    {
		      blHist->Delete();
		    }
		}
	    }
	}
    }
  // BLClean();
  cout << "---------HODO-------------CALIBRATION ---------OFF ------------------- " << endl;  
  
}

Double_t CbmStsCosyBL::GetBlPos( TH1F * hist )
{
  Int_t medianBin = 1;
  
  Double_t * integral = hist->GetIntegral();
  while( integral[ medianBin + 1 ] <= 0.5 ) {
    medianBin++;
  }
  
  return hist->GetXaxis()->GetBinCenter( medianBin );
}

void  CbmStsCosyBL::Reset()
{
  if (cDigis) {
    cDigis->Clear();
  }
  if (chDigis) {
    chDigis->Clear();
  }
}

ClassImp(CbmStsCosyBL)
