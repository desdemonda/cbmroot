// -------------------------------------------------------------------------
// -----              CbmMvdQa  source file                            -----
// -----              Created 12/01/15  by P. Sitzmann                 -----
// ------------------------------------------------------------------------

//-- Include from Cbm --//
#include "CbmMvdQa.h"
#include "CbmStsTrack.h"
#include "CbmMvdHit.h"
#include "CbmMvdPoint.h"

#include "CbmVertex.h"
#include "CbmMatch.h"
#include "CbmLink.h"
#include "CbmTrackMatchNew.h"
#include "CbmMCTrack.h"

#include "base/CbmLitToolFactory.h"
#include "data/CbmLitTrackParam.h"
#include "utils/CbmLitConverter.h"


//-- Include from Fair --//
#include "FairLogger.h"
#include "FairTrackParam.h"


//-- Include from Root --//
#include "TCanvas.h"
#include "TMath.h"

//-- Include from C++ --//
#include <iostream>


using std::cout;
using std::endl;
using std::flush;

// -----   Default constructor   -------------------------------------------
CbmMvdQa::CbmMvdQa() 
  : FairTask("MvdQa"),
  fNHitsOfLongTracks(),
  fEventNumber(),
  fStsTrackArray(),
  fStsTrackMatches(),
  fListMCTracks(),
  fMvdHitMatchArray(),
  fMvdResHisto(),
  fMvd4ResHisto(),
  fVtxResHisto(),
  fMvdAngleResHisto(),
  fMvdAngleResHisto1GeV(),
  fMvdAngleResHistolarger1GeV(),
  fMvdAngleResHisto2GeV(),
  fMvdAngleResHistolarger2GeV(),
  fMvdResHistoTrasMom(),
  fMatchingEffiHisto(),
  fInfoArray(),
  fExtrapolator(),
  fPrimVtx(),
  fSecVtx(),
  fminHitReq(0),
  fMvdRecoRatio(0),
  fBadTrack(0),
  fUsedTracks(0),
  fnrOfMergedHits(0.)
{
;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdQa::CbmMvdQa(const char* name, Int_t iMode, Int_t iVerbose) 
  : FairTask(name, iVerbose),
  fNHitsOfLongTracks(),
  fEventNumber(),
  fStsTrackArray(),
  fStsTrackMatches(),
  fListMCTracks(),
  fMvdHitMatchArray(),
  fMvdResHisto(),
  fMvd4ResHisto(),
  fVtxResHisto(),
  fMvdAngleResHisto(),
  fMvdAngleResHisto1GeV(),
  fMvdAngleResHistolarger1GeV(),
  fMvdAngleResHisto2GeV(),
  fMvdAngleResHistolarger2GeV(),
  fMvdResHistoTrasMom(),
  fMatchingEffiHisto(),
  fInfoArray(),
  fExtrapolator(),
  fPrimVtx(),
  fSecVtx(),
  fminHitReq(0),
  fMvdRecoRatio(0),
  fBadTrack(0),
  fUsedTracks(0),
  fnrOfMergedHits(0.)
{
 ;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdQa::~CbmMvdQa() 
{
;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
InitStatus CbmMvdQa::Init()
{
cout << "-------------------------------------------------------------------------" << endl
     << "-I- " << GetName() << "::Init: " 
     << " Start Initilisation " << endl
     << "-------------------------------------------------------------------------" << endl;

    FairRootManager* ioman = FairRootManager::Instance();
      if (! ioman) {
    cout << "-E- " << GetName() << "::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
    }
    fBadTracks = new TClonesArray("CbmStsTrack", 5000);
    ioman->Register("BadTracks", "sts", fBadTracks, kTRUE);

    fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
    fStsTrackArray   = (TClonesArray*) ioman->GetObject("StsTrack");
    if(! fStsTrackArray) {Fatal("CbmMvdQa: StsTrackArray not found (!)"," That's bad. ");}
    fMvdHits         = (TClonesArray*) ioman->GetObject("MvdHit");
    fMvdHitMatchArray= (TClonesArray*) ioman->GetObject("MvdHitMatch");
    fMcPoints       = (TClonesArray*) ioman->GetObject("MvdPoint");
    if( fMvdHits->GetEntriesFast() != fMvdHitMatchArray->GetEntriesFast())
	cout << endl << "MvdHit and MvdHitMatch Arrays do not have the same size" << endl;
    fPrimVtx         = (CbmVertex*) ioman->GetObject("PrimaryVertex");
    fListMCTracks    = (TClonesArray*) ioman->GetObject("MCTrack");
    fExtrapolator = CbmLitToolFactory::Instance()->CreateTrackExtrapolator("rk4");

    

    // --- all the histogramms --- //
    fMvdResHisto=new TH2F("MvdResMom","Mvd Resolution Momentum",70,0,10,70,0,0.05);
    fMvdResHisto->GetYaxis()->SetTitle("Impact Radius [cm]");
    fMvdResHisto->GetXaxis()->SetTitle("Momentum [GeV/c]");

    fMvdResHisto->GetYaxis()->SetTitleOffset(1.5);
    fMvdResHisto->GetXaxis()->SetTitleOffset(1.5);

   fMvd4ResHisto=new TH2F("Mvd4ResMom","Mvd Resolution Momentum [>2 Mvd Hits]",70,0,10,70,0,0.05);
   fMvd4ResHisto->GetYaxis()->SetTitle("Impact Radius [cm]");
   fMvd4ResHisto->GetXaxis()->SetTitle("Momentum [GeV/c]");

    fMvd4ResHisto->GetYaxis()->SetTitleOffset(1.5);
    fMvd4ResHisto->GetXaxis()->SetTitleOffset(1.5);

   fMvdAngleResHisto=new TH2F("MvdResAngle","Mvd Resolution Angle",70,0,45,70,0,0.01);
   fMvdAngleResHisto->GetYaxis()->SetTitle("Impact Radius [cm]");
   fMvdAngleResHisto->GetXaxis()->SetTitle("Angle");

    fMvdAngleResHisto->GetYaxis()->SetTitleOffset(1.5);
    fMvdAngleResHisto->GetXaxis()->SetTitleOffset(1.5);

   fMvdAngleResHisto1GeV=new TH2F("MvdResAngle1GeV","Mvd Resolution Angle [p<1GeV]",70,0,45,70,0,0.01);
   fMvdAngleResHisto1GeV->GetYaxis()->SetTitle("Impact Radius [cm]");
   fMvdAngleResHisto1GeV->GetXaxis()->SetTitle("Angle");

     fMvdAngleResHisto1GeV->GetYaxis()->SetTitleOffset(1.5);
     fMvdAngleResHisto1GeV->GetXaxis()->SetTitleOffset(1.5);

   fMvdAngleResHistolarger1GeV=new TH2F("MvdResAnglelarger1GeV","Mvd Resolution Angle [p>1GeV]",70,0,45,70,0,0.01);
   fMvdAngleResHistolarger1GeV->GetYaxis()->SetTitle("Impact Radius [cm]");
   fMvdAngleResHistolarger1GeV->GetXaxis()->SetTitle("Angle");

    fMvdAngleResHistolarger1GeV->GetYaxis()->SetTitleOffset(1.5);
    fMvdAngleResHistolarger1GeV->GetXaxis()->SetTitleOffset(1.5);

   fMvdAngleResHisto2GeV=new TH2F("MvdResAngle2GeV","Mvd Resolution Angle [p<2GeV]",70,0,45,70,0,0.01);
   fMvdAngleResHisto2GeV->GetYaxis()->SetTitle("Impact Radius [cm]");
   fMvdAngleResHisto2GeV->GetXaxis()->SetTitle("Angle");

    fMvdAngleResHisto2GeV->GetYaxis()->SetTitleOffset(1.5);
    fMvdAngleResHisto2GeV->GetXaxis()->SetTitleOffset(1.5);

   fMvdAngleResHistolarger2GeV=new TH2F("MvdResAnglelarger2GeV","Mvd Resolution Angle [p>2GeV]",70,0,45,70,0,0.01);
   fMvdAngleResHistolarger2GeV->GetYaxis()->SetTitle("Impact Radius [cm]");
   fMvdAngleResHistolarger2GeV->GetXaxis()->SetTitle("Angle");

    fMvdAngleResHistolarger2GeV->GetYaxis()->SetTitleOffset(1.5);
    fMvdAngleResHistolarger2GeV->GetXaxis()->SetTitleOffset(1.5);

   fVtxResHisto=new TH2F("VtxRes","Vertex Resolution",70,0,0.01,70,0,0.01);
   fVtxResHisto->GetXaxis()->SetTitle("Impact Radius x [cm]");
   fVtxResHisto->GetYaxis()->SetTitle("Impact Radius y [cm]");

    fVtxResHisto->GetYaxis()->SetTitleOffset(1.5);
    fVtxResHisto->GetXaxis()->SetTitleOffset(1.5);

   fMatchingEffiHisto=new TH1F("MvdMatchingEffi","Matching Efficiency",70,0,1.1);
   fMatchingEffiHisto->GetXaxis()->SetTitle("Matching Efficiency");
   fMatchingEffiHisto->GetXaxis()->SetTitleOffset(1.5);
 fMatchingEffiHisto->GetYaxis()->SetTitle("Entrys");
   fMatchingEffiHisto->GetYaxis()->SetTitleOffset(1.5);

cout << "-------------------------------------------------------------------------" << endl
     << "-I- " << GetName() << "::Init: " 
     << " Finished Initilisation " << endl
     << "-------------------------------------------------------------------------" << endl;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMvdQa::Exec(Option_t* opt) 
{
fEventNumber++;

LOG(INFO) << "CbmMvdQa is running" << FairLogger::endl;

Int_t usedTracks=0;
Double_t zOut;
CbmStsTrack* stsTrack;
Int_t nTracks = fStsTrackArray->GetEntriesFast();
Int_t nTrackMatches = fStsTrackMatches->GetEntriesFast();
Int_t nMcTracks = fListMCTracks->GetEntriesFast();
Int_t motherID = 0;
Int_t trackIndex = 0;
CbmTrackMatchNew* trackMatch;
CbmMatch* Match;
CbmMCTrack* mcTrack;


    if (fVerbose>0) printf(" CbmMvdQa: Entries:  %i StsTracks from %i McTracks\n",nTracks,nMcTracks);

    if(nTracks==0){
	cout << endl << " -W- CbmMvdQa:: No CbmStsTracks found, ignoring this event." << endl;
	return;
    }
    if( nTracks != nTrackMatches )
	{
	cout << endl << " -W- CbmMvdQa:: CbmSletsTrackMatches has not the same size as CbmStsTracks" << endl;
	return;  
	} 
    // --- Loop over reconstructed tracks ---
for ( Int_t itr=0; itr<nTracks; itr++ )
	{
	stsTrack = (CbmStsTrack*) fStsTrackArray->At(itr);
        trackMatch = (CbmTrackMatchNew*)fStsTrackMatches->At(itr);
        if(trackMatch)
		{ 
		if ( ! trackMatch->GetNofLinks() ) continue;
		trackIndex = trackMatch->GetMatchedLink().GetIndex();
		if ( 0 <= trackIndex <= nMcTracks)
			{ 
			mcTrack = (CbmMCTrack*) fListMCTracks->At(trackIndex);
			motherID = mcTrack->GetMotherId();
			}

		else
			cout << endl << "index of match to hight" << endl;
		}
	else
		{
		cout << endl << "Ups there was no Match here" << endl;
		continue;
		}
	Int_t mvdHitsInTrack = stsTrack->GetNofMvdHits();
        Int_t mvdStsRatio;
       if(mvdHitsInTrack > 0) 
		mvdStsRatio = stsTrack->GetNofHits() / mvdHitsInTrack;
       else mvdStsRatio = 10;

        if(mvdHitsInTrack >= fminHitReq && motherID == -1)
		{
		fUsedTracks++;
		//cout << endl << "found good track" << endl;
                SetMatches(trackIndex, stsTrack);
			if(fMvdRecoRatio != 1) 
				{
				new((*fBadTracks)[fBadTracks->GetEntriesFast()]) CbmStsTrack(*((CbmStsTrack*)stsTrack));
				fBadTrack++;
				continue;
				}
    		const FairTrackParam *paramIn = stsTrack->GetParamFirst();
   	 	CbmLitTrackParam litParamIn; // parameter at last attached Hit
   	 	CbmLitConverter::FairTrackParamToCbmLitTrackParam(paramIn, &litParamIn);
    		CbmLitTrackParam litParamOut; // parameter at vertex
	
   	 	fExtrapolator->Extrapolate(&litParamIn, &litParamOut,0, NULL);
		       fMvdResHisto->Fill(GetMomentum(litParamOut), GetImpactParameterRadius(litParamOut));
                       fVtxResHisto->Fill(GetImpactParameterX(litParamOut),GetImpactParameterY(litParamOut));
		  fMvdAngleResHisto->Fill(GetAngle(litParamOut),GetImpactParameterRadius(litParamOut));	
		
	   if(mvdHitsInTrack > 2)
			{
	        	fMvd4ResHisto->Fill(GetMomentum(litParamOut),GetImpactParameterRadius(litParamOut) );
                	
			}
           if (GetMomentum(litParamOut) < 1)
			{
			fMvdAngleResHisto1GeV->Fill(GetAngle(litParamOut),GetImpactParameterRadius(litParamOut));	
			}
	   if (GetMomentum(litParamOut) > 1)
			{
			fMvdAngleResHistolarger1GeV->Fill(GetAngle(litParamOut),GetImpactParameterRadius(litParamOut));	
          		}
           if (GetMomentum(litParamOut) < 2)
			{
			fMvdAngleResHisto2GeV->Fill(GetAngle(litParamOut),GetImpactParameterRadius(litParamOut));	
			}
	   if (GetMomentum(litParamOut) > 4)
			{
			fMvdAngleResHistolarger2GeV->Fill(GetAngle(litParamOut),GetImpactParameterRadius(litParamOut));	
	  		}
		}

        }
fStsTrackArray->Clear();
fStsTrackMatches->Clear();

}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void  CbmMvdQa::SetMatches(Int_t trackID, CbmStsTrack* stsTrack){

Int_t nrOfMvdHits = stsTrack->GetNofMvdHits();
Int_t mcTrackId = 0;
Int_t trueCounter = 0;
Float_t falseCounter = 0;
Bool_t hasTrack;
Int_t nrOfLinks = 0;
Int_t mcPointId = 0;
fMvdRecoRatio = 0.;
const CbmMvdPoint* point = NULL;
for(Int_t iHit = 0; iHit < nrOfMvdHits; iHit++)	
	{
	hasTrack = kFALSE;
        CbmMatch* mvdMatch = (CbmMatch*)fMvdHitMatchArray->At(stsTrack->GetMvdHitIndex(iHit));
	if(mvdMatch)
		{
		nrOfLinks = mvdMatch->GetNofLinks();
		if(nrOfLinks > 1)
			fnrOfMergedHits++;
		}	
	else 
		{
		continue; // any kind of error in the matching
		}
	for(Int_t iLink = 0; iLink < nrOfLinks; iLink++)
		{
		Int_t pointIndex = mvdMatch->GetLink(iLink).GetIndex();
		if(pointIndex < fMcPoints->GetEntriesFast())	
 		point = (CbmMvdPoint*)fMcPoints->At(pointIndex);
		if (NULL == point)
			{
			continue; //delta or background event
			}
		else
			mcTrackId = point->GetTrackID();
		if(mcTrackId == trackID)
			{
			hasTrack = kTRUE;
			}
		}
	if(!hasTrack)
	falseCounter++;
	}
Float_t counter = (Float_t)falseCounter / (Float_t)nrOfMvdHits;
fMvdRecoRatio = (Float_t)1.0 - (Float_t)counter;

	
fMatchingEffiHisto->Fill(fMvdRecoRatio);
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t  CbmMvdQa::GetImpactParameterRadius( CbmLitTrackParam t ){

    Double_t imx  = t.GetX();
    Double_t imy  = t.GetY();
    Double_t imr  = sqrt(imx*imx + imy*imy);

   return  imr;
}
// -------------------------------------------------------------------------


//--------------------------------------------------------------------------
Double_t  CbmMvdQa::GetImpactParameterX( CbmLitTrackParam t ){

Double_t imx  = t.GetX();

   return  imx;
}
// -------------------------------------------------------------------------


//--------------------------------------------------------------------------
Double_t  CbmMvdQa::GetImpactParameterY( CbmLitTrackParam t ){

    Double_t imy  = t.GetY();

   return  imy;
}
// -------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------
Double_t  CbmMvdQa::GetTransverseMomentum( CbmLitTrackParam t ){

const std::vector<double> state= t.GetStateVector();

Float_t pz   = GetMomentumZ(t);
Float_t px = state[2] * pz;
Float_t py = state[3] * pz;

Double_t pt = sqrt(px * px + py * py);

    return  pt;
}
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
Double_t  CbmMvdQa::GetMomentum( CbmLitTrackParam t ){

    Float_t p = fabs(1.F / t.GetQp());

    return  p;
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
Double_t  CbmMvdQa::GetMomentumZ( CbmLitTrackParam t ){

   const std::vector<double> state= t.GetStateVector();

Float_t pz   = sqrt(GetMomentum(t) * GetMomentum(t) / (state[2] * state[2] + state[3] * state[3] + 1));

    return  pz;
}
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
Double_t  CbmMvdQa::GetAngle( CbmLitTrackParam t ){

    Float_t angle_rad = std::atan(GetTransverseMomentum(t) / GetMomentumZ(t));
    Float_t rad_to_grad = ( 180 / TMath::Pi() );
    Float_t angle_grad = angle_rad * rad_to_grad;
//cout << endl << "calc angle of " << angle_grad << " from rad angle " << angle_rad << endl;
    return  angle_grad;
}
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
void CbmMvdQa::Finish()
{
Float_t mvdTrackingEffi = (fBadTrack)/(fUsedTracks/100);
cout << endl << "-I- " << GetName() << " Finished" << endl
     << "Total of " << fBadTrack << " StsTracks where removed due bad matched hits in the MVD" << endl
     << "Removed: " << mvdTrackingEffi << " % of all Tracks from Plots" << endl
     << fnrOfMergedHits << " Hit where merged" << endl;
TCanvas* c=new TCanvas();
c->Divide(2,2);
c->cd(1);
fMvdResHisto->Draw("COL");
fMvdResHisto->Write();
c->cd(2);
fMvd4ResHisto->Draw("COL");
fMvd4ResHisto->Write();
c->cd(3);
fMatchingEffiHisto->Draw("COL");
fMatchingEffiHisto->Write();
c->cd(4);
fMvdAngleResHisto->Draw("COL");
fMvdAngleResHisto->Write();


TCanvas* c2 = new TCanvas();
c2->Divide(2,2);
c2->cd(1);
fMvdAngleResHisto1GeV->Draw("COL");
fMvdAngleResHisto1GeV->Write();
c2->cd(2);
fMvdAngleResHistolarger1GeV->Draw("COL");
fMvdAngleResHistolarger1GeV->Write();
c2->cd(3);
fMvdAngleResHisto2GeV->Draw("COL");
fMvdAngleResHisto2GeV->Write();
c2->cd(4);
fMvdAngleResHistolarger2GeV->Draw("COL");
fMvdAngleResHistolarger2GeV->Write();
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
ClassImp(CbmMvdQa);


