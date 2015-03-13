// -------------------------------------------------------------------------
// -----                    CbmMvdDigitize source file                -----
// -------------------------------------------------------------------------


/**  Digitizer for Simulated Point in the Vertex Detector. 
 * Digitization follows the procedure adopted in the CMS software package
 * an adapted later on to the ILC vertex detector.
 * For each Simulated MVD Hit the intersection points with 
 * the inner and outer boundaries of sensitive Si layer are calculated. 
 * Track segment within a layer is approximated with the line. 
 * It is divided by n subsegments, where n can be specified by a user 
 * via external Processor parameter.  
 * The charge transfer from the middle point of track subsegment (referred 
 * hereafter to as ionisation point) 
 * to the outer collection plane is performed. 
 * It is assumed that on the collection plane the electron cloud from 
 * each ionisation point is spread according to the Gaussian distribution 
 * whose width  is proportional to the square-root of the drift distance. 
 * The charge on each fired pixel is then calculated as a sum of contributions 
 * from n Gaussians.
 *
 * Note that the integration of the gaussians
 * consumes most of the execution time of this task.
 *
 * The output of the processor is the collection of MVD digis.
 *
 * Input collection 
 * Processor requires collection of simulated mvd points. 
 * Output
 * Processor produces an output collection of Mvd digis
 *
 * @param fCutOnDeltaRays: cut on the energy of delta-electrons (in MeV) 
 * (default parameter value : 0.0016972 ) 
 *
 * @param fDiffusionCoefficient: diffusion coefficient for the nominal active layer thickness (in cm) 
 * (default parameter value : 0.0055) 
 *
 * @param fEpiThickness: thickness of the active Silicon layer (in cm) 
 * (default parameter value : 0.0014) 
 *
 * @param PixelSizeX: pixel size along X direction (in cm)
 * (default value : 0.003) 
 *
 * @param PixelSizeY: pixel size along Y direction (in cm) 
 * (default value : 0.003) 
 *
 * @param fElectronsPerKeV: number of electrons produced per KeV of deposited energy 
 * (default parameter value : 276) 
 *
 * @param fChargeThreshold: threshold on charge deposited on one pixel (in electrons) 
 * (default parameter value : 1 ) 
 *
 * @param fSegmentLength: segment length along track path which is used to subdivide track into segments (in cm).
 * The number of track subsegments is calculated as int(TrackLengthWithinActiveLayer/SegmentLength)+1 
 * (default parameter value : 0.0001) 
 *
 * @param fWidthOfCluster: defines width in Gaussian sigmas to perform charge integration for
 * a given pixel 
 * (default parameter value : 3.0) 
 * 
 * original author: A. Raspereza, MPI Munich
 *
 * ____________________________________________________________________________________________
 * --------------------------------------------------------------------------------------------
 * adaptation for CBM: C.Dritsa
 * Acknowlegments to:
 *	Rita de Masi (IPHC, Strasbourg), M.Deveaux (IKF, Frankfurt), V.Friese (GSI, Darmstadt)
 * ____________________________________________________________________________________________
 * --------------------------------------------------------------------------------------------
 */


// Includes from MVD
#include "CbmMvdDigitize.h"

#include "CbmMvdGeoPar.h"
#include "CbmMvdHit.h"
#include "CbmMvdHitMatch.h"
#include "CbmMvdPileupManager.h"
#include "CbmMvdPoint.h"
#include "CbmMvdStation.h"

// Includes from base
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmMCTrack.h"

// Includes from ROOT
#include "TArrayD.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TDatabasePDG.h"
//#include "TGeoShape.h"
#include "TGeoTube.h"
#include "TObjArray.h"
#include "TRandom3.h"
#include "TString.h"
#include "TVector3.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"

// Includes from C++
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

#include "gsl/gsl_sf_erf.h"
//#include "CLHEP/Random/RandGauss.h"
//#include "CLHEP/Random/RandPoisson.h"
//#include "CLHEP/Random/RandFlat.h"


using std::cout;
using std::endl;
using std::map;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::pair;
using std::setprecision;
using std::ios_base;
using std::vector;


// -----   Default constructor   ------------------------------------------
CbmMvdDigitize::CbmMvdDigitize()
  : FairTask("MVDDigitize"),
    fEpiTh(0.0014),
    fSegmentLength(0.0001),
    fDiffusionCoefficient(0.0055),
    fElectronsPerKeV(276.), //3.62 eV for e-h creation
    fWidthOfCluster(3.), // in sigmas
    fPixelSizeX(0.0030), // in cm
    fPixelSizeY(0.0030),
    fCutOnDeltaRays(0.00169720),  //MeV
    fChargeThreshold(1.), //electrons
    fFanoSilicium(0.115),
    fEsum(0.),
    fSegmentDepth(0.),
    fCurrentTotalCharge(0.),
    fCurrentParticleMass(0.),
    fCurrentParticleMomentum(0.),
    fCurrentParticlePdg(0),
    fNumberOfSegments(0),
    fCurrentLayer(0),
    fEvent(0),
    fVolumeId(0),
    fFluctuate(new MyG4UniversalFluctuationForSi()),
    fDigis(new TClonesArray("CbmMvdDigi")),
    fMCTracks(NULL),
    fPixelCharge(new TClonesArray("CbmMvdPixelCharge")),
    fPixelScanAccelerator(NULL),
    fChargeMap(),
    fChargeMapIt(),
    fMode(0),
    fSigmaX(0.0005),
    fSigmaY(0.0005),
    fEfficiency(0.99),
    fMergeDist(0.),
    fFakeRate(0.03),
    fNPileup(0),
    fNDeltaElect(0),
    fDeltaBufferSize(10000),
    fBgBufferSize(1000),
    fStationMap(),
    fBranchName("MvdPoint"),
    fBgFileName(""),
    fDeltaFileName(""),
    fInputPoints(),
    fPoints(new TRefArray()),
    fRandGen(2736), 
    fTimer(),
    fPileupManager(NULL),
    fDeltaManager(NULL),
    fGeoPar(NULL),
    fNEvents(0.),
    fNPoints(0.),
    fNReal(0.),
    fNBg(0.),
    fNFake(0.),
    fNLost(0.),
    fNMerged(0.),
    fTime(0.),
    fSignalPoints(),
    h_trackLength(NULL),
    h_numSegments(NULL),
    h_LengthVsAngle(NULL),
    h_LengthVsEloss(NULL),
    h_ElossVsMomIn(NULL)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdDigitize::CbmMvdDigitize(const char* name, Int_t iMode,
				 Int_t iVerbose)
  : FairTask(name, iVerbose),
    fEpiTh(0.0014),
    fSegmentLength(0.0001),
    fDiffusionCoefficient(0.0055),
    fElectronsPerKeV(276.), //3.62 eV for e-h creation
    fWidthOfCluster(3.), // in sigmas
    fPixelSizeX(0.0030), // in cm
    fPixelSizeY(0.0030),
    fCutOnDeltaRays(0.00169720),  //MeV
    fChargeThreshold(1.), //electrons
    fFanoSilicium(0.115),
    fEsum(0.),
    fSegmentDepth(0.),
    fCurrentTotalCharge(0.),
    fCurrentParticleMass(0.),
    fCurrentParticleMomentum(0.),
    fCurrentParticlePdg(0),
    fNumberOfSegments(0),
    fCurrentLayer(0),
    fEvent(0),
    fVolumeId(0),
    fFluctuate(new MyG4UniversalFluctuationForSi()),
    fDigis(new TClonesArray("CbmMvdDigi")),
    fMCTracks(NULL),
    fPixelCharge(new TClonesArray("CbmMvdPixelCharge")),
    fPixelScanAccelerator(NULL),
    fChargeMap(),
    fChargeMapIt(),
    fMode(iMode),
    fSigmaX(0.0005),
    fSigmaY(0.0005),
    fEfficiency(0.99),
    fMergeDist(0.),
    fFakeRate(0.03),
    fNPileup(0),
    fNDeltaElect(0),
    fDeltaBufferSize(10000),
    fBgBufferSize(1000),
    fStationMap(),
    fBranchName("MvdPoint"),
    fBgFileName(""),
    fDeltaFileName(""),
    fInputPoints(),
    fPoints(new TRefArray()),
    fRandGen(2736), 
    fTimer(),
    fPileupManager(NULL),
    fDeltaManager(NULL),
    fGeoPar(NULL),
    fNEvents(0.),
    fNPoints(0.),
    fNReal(0.),
    fNBg(0.),
    fNFake(0.),
    fNLost(0.),
    fNMerged(0.),
    fTime(0.),
    fSignalPoints(),
    h_trackLength(NULL),
    h_numSegments(NULL),
    h_LengthVsAngle(NULL),
    h_LengthVsEloss(NULL),
    h_ElossVsMomIn(NULL)
{
}


// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdDigitize::~CbmMvdDigitize() {

    if ( fDigis) {
	fDigis->Delete();
	delete fDigis;
    }

    if ( fPileupManager ) delete fPileupManager;
    if ( fDeltaManager ) delete fDeltaManager;

}


// -----------------------------------------------------------------------------
Int_t CbmMvdDigitize::BuildEvent() {

  // Counters
  Int_t nOrig = 0;
  Int_t nPile = 0;
  Int_t nElec = 0;


  // Some frequently used variables
  CbmMvdPoint*   point    = NULL;
  CbmMvdStation* station  = NULL;
  Int_t          iStation = 0;


  // ----- First treat standard input file
  for (Int_t i=0; i<fInputPoints->GetEntriesFast(); i++) {
    point = (CbmMvdPoint*) fInputPoints->At(i);
    iStation = point->GetStationNr();
    if ( fStationMap.find(iStation) == fStationMap.end() ) 
      Fatal("BuildEvent", "Station not found");
    fStationMap[iStation]->AddPoint(point);
    nOrig++;
  }


  // ----- Then treat event pileup
  if (fNPileup>0) {
 
    // --- Vector of available background events from pile-up. 
    // --- Each event is used only once.
    Int_t nBuffer = fPileupManager->GetNEvents();
    vector<Int_t> freeEvents(nBuffer);
    for (Int_t i=0; i<nBuffer; i++) freeEvents[i] = i;

    // --- Loop over pile-up events
    for (Int_t iBg=0; iBg<fNPileup; iBg++) {

      // Select random event from vector and remove it after usage
      Int_t index = gRandom->Integer(freeEvents.size());
      Int_t iEvent = freeEvents[index];
      TClonesArray* points = fPileupManager->GetEvent(iEvent);
      freeEvents.erase(freeEvents.begin() + index);

      // Add points from this event to the input arrays
      for (Int_t iPoint=0; iPoint<points->GetEntriesFast(); iPoint++) {
	point = (CbmMvdPoint*) points->At(iPoint);
	point->SetTrackID(-2);
	iStation = point->GetStationNr();
	if ( fStationMap.find(iStation) == fStationMap.end() ) 
	  Fatal("BuildEvent", "Station not found");
	fStationMap[iStation]->AddPoint(point);
	nPile++;
      }
	
    }   // Pileup event loop

  }    // Usage of pile-up

			   
  // ----- Finally, treat delta electrons
  if (fNDeltaElect>0) {
 
    // --- Vector of available delta events.
    // --- Each event is used only once.
    Int_t nDeltaBuffer = fDeltaManager->GetNEvents();
    vector<Int_t> freeDeltaEvents(nDeltaBuffer);
    for (Int_t i=0; i<nDeltaBuffer; i++) freeDeltaEvents[i] = i;

    // --- Loop over delta events
    for (Int_t it=0; it<fNDeltaElect; it++) {

      // Select random event from vector and remove it after usage
      Int_t indexD  = gRandom->Integer(freeDeltaEvents.size());
      Int_t iEventD = freeDeltaEvents[indexD];
      TClonesArray* pointsD = fDeltaManager->GetEvent(iEventD);
      freeDeltaEvents.erase(freeDeltaEvents.begin() + indexD);

      // Add points from this event to the input arrays
      for (Int_t iPoint=0; iPoint<pointsD->GetEntriesFast(); iPoint++) {
	point = (CbmMvdPoint*) pointsD->At(iPoint);
	point->SetTrackID(-3);
	iStation = point->GetStationNr();
	if ( fStationMap.find(iStation) == fStationMap.end() ) 
	  Fatal("BuildEvent", "Station not found");
	fStationMap[iStation]->AddPoint(point);
	nElec++;
      }

    }  // Delta electron event loop

  }    // Usage of delta


  // ----- At last: Screen output
  if ( fVerbose > 1 ) cout << "-I- " << GetName() << "::BuildEvent: original "
			   << nOrig << ", pileup " << nPile << ", delta "
			   << nElec << ", total " << nOrig+nPile+nElec
			   << " MvdPoints" << endl;

  return nOrig + nPile + nElec;

}
// -----------------------------------------------------------------------------



// -----   Virtual public method Exec   ------------------------------------
void CbmMvdDigitize::Exec(Option_t* opt) {

    // Clear output array and stations
    fDigis->Clear("C");
    map<Int_t, CbmMvdStation*>::iterator stationIt;
    for (stationIt=fStationMap.begin(); stationIt!=fStationMap.end();
	 stationIt++) (*stationIt).second->Clear();


    // Fill input arrays with signal and background points
    Int_t nPoints = BuildEvent();


    fEvent++;
    TVector3 mom;


    // Loop over MVD stations
    for (stationIt=fStationMap.begin(); stationIt!=fStationMap.end();
	 stationIt++) {
	CbmMvdStation* station = (*stationIt).second;

	cout << "-I- Digitizer: Station Nr" << station->GetStationNr() << endl;
	cout << "-I- Digitizer: Station Points" <<   station->GetNPoints() << endl;

	fPixelCharge->Clear("C");

	// Clear charge map
	fChargeMap.clear();

	// Loop over MvdPoints in station
	for (Int_t iPoint=0; iPoint<station->GetNPoints(); iPoint++) {
	    CbmMvdPoint* point = station->GetPoint(iPoint);
	    
	    
	    // Reject for the time being light nuclei.
	    // They have to be added in the data base...
	    if ( point->GetPdgCode() > 100000) continue;

	    // Produce charge in pixels
	    ProduceIonisationPoints(point, station);
	    ProduceSignalPoints();
	    ProducePixelCharge(point);

	    CbmMvdPixelCharge* pixelCharge;

	    for(Int_t f=0; f<fPixelCharge->GetEntriesFast(); f++)
	    {
		pixelCharge = (CbmMvdPixelCharge*) fPixelCharge->At(f);
		pixelCharge->DigestCharge( ( (float)( point->GetX()+point->GetXOut() )/2 ) , ( (float)( point->GetY()+point->GetYOut() )/2 ),-1, point->GetTrackID() );
	    };

	} //loop on MCpoints

	for (Int_t i=0; i<fPixelCharge->GetEntriesFast(); i++)
	{
	    CbmMvdPixelCharge* pixel = (CbmMvdPixelCharge*)fPixelCharge->At(i);
	    if ( pixel->GetCharge()>fChargeThreshold )
	    {
		Int_t nDigis = fDigis->GetEntriesFast();
		new ((*fDigis)[nDigis])
		    CbmMvdDigi(station->GetStationNr(),
			       pixel->GetX(), pixel->GetY(),
			       pixel->GetCharge(),
			       fPixelSizeX, fPixelSizeY,
			       pixel->GetPointX(), pixel->GetPointY(),
			       pixel->GetContributors(),
			       pixel->GetMaxChargeContribution(),
			       pixel->GetPointId(),
			       pixel->GetTrackId());
	    }
	}
	//------------------------------------------------------------------------------

    }//loop on mvd stations
 
       
    cout << "-I- " << GetName() << " Event " << fEvent << ", MvdPoints " 
         << nPoints << ", MvdDigis " << fDigis->GetEntriesFast() << endl;

}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
void CbmMvdDigitize::ProduceIonisationPoints(CbmMvdPoint* point,
					      CbmMvdStation* station) {
  /** Produces ionisation points along track segment within 
   ** the active Silicon layer.
   **/

  if ( (! station) || (! point) )
    Fatal("ProduceIonisationPoints", "Invalid point or station pointer!");

  Double_t layerRadius = station->GetRmax();
  Double_t layerTh     = station->GetD();

  Int_t pdgCode = point->GetPdgCode();
  Double_t mass = TDatabasePDG::Instance()->GetParticle(pdgCode)->Mass();
  TVector3 mom;
  point->Momentum(mom);
  Double_t momentum = mom.Mag();
  

 
  // entry and exit from the det layer ( detector ref frame ) :
  // -------------OK-------------------------------------------//
  Double_t entryZ = -layerTh/2;                                //
  Double_t exitZ  =  layerTh/2;                                //
                                                               //
  Double_t entryX = point->GetX()    + layerRadius;            //
  Double_t exitX  = point->GetXOut() + layerRadius;            //
  Double_t entryY = point->GetY()    + layerRadius;            //
  Double_t exitY  = point->GetYOut() + layerRadius;            //
                                                               //
  Double_t lxDet  = TMath::Abs(entryX-exitX);                  //
  Double_t lyDet  = TMath::Abs(entryY-exitY);                  //
   						               //
  //-----------------------------------------------------------//


    /**

    Create vector entryDet a (x1,y1,z1) = entry in detector
    Create vector exitDet  b (x2,y2,z2) = exit from detector

    Substract   b-a and get the vector "c" giving the direction of the particle.

    Scale the vector c (draw the 3D schema and check the similar triangles)

    Add vector a.

    The result is a vector with starting point [(x,y,z)entry in detector]
    and end point [(x,y,z)entry in the epi layer]

    same for defining exit from epi layer.
    **/


    // entry and exit from the epi layer ( det ref frame ) :
    Double_t entryZepi = -fEpiTh/2;
    Double_t exitZepi  =  fEpiTh/2;

    
    TVector3  a( entryX, entryY, entryZ ); // entry in the detector
    TVector3  b( exitX,  exitY,  exitZ  ); // exit from the detector
    TVector3  c;

    c = b-a;  // AB in schema

    TVector3 d;
    TVector3 e;

    Double_t scale1 = (entryZepi-entryZ)/(exitZ-entryZ);
    Double_t scale2 = (exitZepi-entryZ)/(exitZ-entryZ);


    d = c*scale1;
    e = c*scale2;

    TVector3 entryEpiCoord;
    TVector3 exitEpiCoord;

    entryEpiCoord = d+a;
    exitEpiCoord  = e+a;


    //Get x and y coordinates at the ENTRY of the epi layer
    Double_t entryXepi = entryEpiCoord.X();
    Double_t entryYepi = entryEpiCoord.Y();
             entryZepi = entryEpiCoord.Z();

    //Get x and y coordinates at the EXIT of the epi layer
    Double_t exitXepi = exitEpiCoord.X();
    Double_t exitYepi = exitEpiCoord.Y();
             exitZepi = exitEpiCoord.Z();

    Double_t lx        = TMath::Abs(entryXepi-exitXepi); //length of segment x-direction
    Double_t ly        = TMath::Abs(entryYepi-exitYepi);
    Double_t lz        = TMath::Abs(entryZepi-exitZepi);
    //-----------------------------------------------------------


    Double_t rawLength = sqrt( lx*lx + ly*ly + lz*lz );  //length of the track inside the epi-layer, in cm
    Double_t trackLength = 0;

    if(rawLength<1.0e+3) { trackLength = rawLength; }
    else{
	cout << "-W- "<< GetName() << " : rawlength > 1.0e+3 : "<< rawLength << endl;
	trackLength = 1.0e+3;
    }


    //Smear the energy on each track segment
    Double_t dEmean = point->GetEnergyLoss()*fEpiTh/layerTh; // dEmean: energy loss corresponds to the epi thickness
    fNumberOfSegments = int(trackLength/fSegmentLength) + 1;
    dEmean = dEmean/((Double_t)fNumberOfSegments); // From this point dEmean corresponds to the E lost per segment.
   
    fSignalPoints.resize(fNumberOfSegments);

    fEsum = 0.0;

    Double_t segmentLength_update = trackLength/((Double_t)fNumberOfSegments);

    if( lz!=0 ){
	/**
	 condition added 05/08/08 because if lz=0 then there is no segment
         projection (=fSegmentDepth)
	 **/
	fSegmentDepth = fEpiTh/((Double_t)fNumberOfSegments);
    }
    else{//condition added 05/08/08
	fSegmentDepth = 0;
        cout << "-W- " << GetName() << " Length of track in detector (z-direction) is 0!!!" << endl;
    }


    Double_t x=0,y=0,z=0;

    for (int i=0; i<fNumberOfSegments; ++i) {
       
	z = -fEpiTh/2 + ((double)(i)+0.5)*fSegmentDepth; //middle position of the segment; zdirection
	x = entryXepi + ((double)(i)+0.5)*( lx/( (Double_t)fNumberOfSegments) ); //middle position of the segment; xdirection
        y = entryYepi + ((double)(i)+0.5)*( ly/( (Double_t)fNumberOfSegments) ); //middle position of the segment; ydirection


	SignalPoint* spoint=&fSignalPoints[i];
	Double_t de;

	if( mass !=0 ){
	    de = fFluctuate->SampleFluctuations( double(1000.* momentum), double(1000.*mass), fCutOnDeltaRays, segmentLength_update*10, double(1000.*dEmean) ) /1000.;
	}
	else {  de = dEmean;  }



	fEsum = fEsum + de;
	spoint->eloss = de;
	spoint->x = x; //here the coordinates x,y,z are given in the detector reference frame.
	spoint->y = y;
	spoint->z = z;

        // --- debug 05/08/08 start -------



	x=x-layerRadius;
        y=y-layerRadius;

	if (sqrt(x*x + y*y )< 0.5) {
	    cout <<"-I- " << GetName() << "point->GetX()= " <<  point->GetX() <<  " , point->GetXOut()= " << point->GetXOut()  << " , pdg code " << pdgCode << endl;
	    cout <<"-I- " << GetName() << "point->GetY()= " <<  point->GetY() <<  " , point->GetYOut()= " << point->GetYOut() << endl;
	    cout <<"-I- " << GetName() << "point->GetZ()= " <<setprecision(8)<<  point->GetZ() <<  " , point->GetZOut()= " <<setprecision(8)<< point->GetZOut() << endl;
	}

	// --- debug 05/08/08 end -------
      

    }




}


// -------------------------------------------------------------------------
void CbmMvdDigitize::ProduceSignalPoints() {
    /** Produces signal points on the collection plane.
     */

    

    // loop over ionisation points
    for (Int_t i=0; i<fNumberOfSegments; ++i) {

	SignalPoint* spoint = &fSignalPoints[i];
        Double_t de = spoint->eloss;
	Double_t DriftLength = fEpiTh/2 - spoint->z;
        Double_t SigmaDiff = sqrt(DriftLength/fEpiTh)*fDiffusionCoefficient;

        Double_t charge      = 1.0e+6*de*fElectronsPerKeV;

	//Double_t chargeFluct = gRandom->Gaus( 0, TMath::Sqrt(charge*fFanoSilicium) );
	//charge = charge + chargeFluct;

	spoint->sigmaX = SigmaDiff;
	spoint->sigmaY = SigmaDiff;
	spoint->charge = charge;

    }


}
// -------------------------------------------------------------------------

void CbmMvdDigitize::ProducePixelCharge(CbmMvdPoint* point) {
    /** Simulation of fired pixels. Each fired pixel is considered
     * as SimTrackerHit
     */

    fCurrentTotalCharge = 0.0;

    for (int i=0; i<fNumberOfSegments; ++i) {
	SignalPoint* spoint = &fSignalPoints[i];

	Double_t xCentre = spoint->x;  //of segment
	Double_t yCentre = spoint->y;  /// idem
	Double_t sigmaX  = spoint->sigmaX;
	Double_t sigmaY  = spoint->sigmaY;
        
	Double_t xLo = spoint->x - fWidthOfCluster*spoint->sigmaX;
	Double_t xUp = spoint->x + fWidthOfCluster*spoint->sigmaX;

	Double_t yLo = spoint->y - fWidthOfCluster*spoint->sigmaY;
	Double_t yUp = spoint->y + fWidthOfCluster*spoint->sigmaY;

	fCurrentTotalCharge += spoint->charge;
	Int_t ixLo, ixUp, iyLo, iyUp;

	TransformXYtoPixelIndex(xLo,yLo,ixLo,iyLo);
	TransformXYtoPixelIndex(xUp,yUp,ixUp,iyUp);
        
	// Loop over all fired pads
	// and calculate deposited charges
	for (int ix = ixLo; ix<ixUp+1; ix++) {

	    for (int iy = iyLo; iy<iyUp+1; iy++) {
	    
			Double_t xCurrent,yCurrent;
			TransformPixelIndexToXY(ix,iy,xCurrent,yCurrent);
			gsl_sf_result result;
			Int_t status = gsl_sf_erf_Q_e(float((xCurrent - 0.5*fPixelSizeX - xCentre)/sigmaX), &result);
			Float_t LowerBound = 1 - result.val;  // substract from 1 because result.val is a probability
			status = gsl_sf_erf_Q_e(float((xCurrent + 0.5*fPixelSizeX - xCentre)/sigmaX), &result);
			Float_t UpperBound = 1 - result.val;
			Float_t integralX = UpperBound - LowerBound;
			status = gsl_sf_erf_Q_e(float((yCurrent - 0.5*fPixelSizeY - yCentre)/sigmaY), &result);
			LowerBound = 1 - result.val;
			status = gsl_sf_erf_Q_e(float((yCurrent + 0.5*fPixelSizeY - yCentre)/sigmaY), &result);
			UpperBound = 1 - result.val;
			Float_t integralY = UpperBound - LowerBound;
			Int_t totCharge = int((spoint->charge)*integralX*integralY);

  			CbmMvdDigi * digi = 0;

			if(totCharge>0){AddChargeToPixel(ix,iy,totCharge,point);};

	    }//for y
	}// for x
    }// for number of segments

}//end of function

// -------------------------------------------------------------------------
void CbmMvdDigitize::TransformXYtoPixelIndex(Double_t x, Double_t y,Int_t & ix, Int_t & iy) {
    /**
     * Function calculates position in pixel matrix based on the
     * local coordinates of point in the ladder.
     */
   
    iy = int(y/fPixelSizeY);
    ix = int(x/fPixelSizeX);
}

// ---------------------------------------------------------------------------

void CbmMvdDigitize:: AddChargeToPixel(Int_t channelX, Int_t channelY, Int_t charge, CbmMvdPoint* point){
    // Adds the charge of a hit to the pixels. Checks if the pixel was hit before.

    CbmMvdPixelCharge* pixel;


    // Look for pixel in charge map
    pair<Int_t, Int_t> a(channelX, channelY);
    fChargeMapIt = fChargeMap.find(a);

    // Pixel not yet in map -> Add new pixel
    if ( fChargeMapIt == fChargeMap.end() ) {
        pixel= new ((*fPixelCharge)[fPixelCharge->GetEntriesFast()])
	    CbmMvdPixelCharge(charge, channelX, channelY, -1, point->GetTrackID());
	fChargeMap[a] = pixel;
    }

    // Pixel already in map -> Add charge
    else {
	pixel = fChargeMapIt->second;
	if ( ! pixel ) Fatal("AddChargeToPixel", "Zero pointer in charge map!");
	pixel->AddCharge(charge);
    }

}


// -------------------------------------------------------------------------

void CbmMvdDigitize::TransformPixelIndexToXY(Int_t ix, Int_t iy, Double_t & x, Double_t & y) {

    /**
     Function calculates position in the local frame
     based on the index of pixel in the ladder.
     */

    //x,y are calculated at the center of the pixel:
    y = ( 0.5+double(iy) )*fPixelSizeY;
    x = ( 0.5+double(ix) )*fPixelSizeX;
}
// -------------------------------------------------------------------------



// -----  Private method GetMvdGeometry  ---------------------------------------

/**  The method assumes the following convention:
 **  --- The MVD stations are of the shape TGeoTube.
 **  --- The name of a station is mvdstationxx, where xx is the number
 **      of the station.
 **  --- The stations are numbered from 1 on consecutively.
 **  V. Friese, 4 December 2008
 **/

Int_t CbmMvdDigitize::GetMvdGeometry() {
   
  cout << "-I- " << GetName() << " : Reading MVD geometry..." << endl;
  Int_t iStation =  1;
  Int_t volId    = -1;
  fStationMap.clear();
  
  do {

    // Volume name according to convention
    TString volName  = Form("mvdstation%02i", iStation);
    volId = gGeoManager->GetUID(volName);
    if ( volId > -1 ) {

      // Get shape parameters
      TGeoVolume* volume = gGeoManager->GetVolume(volName.Data());
      TGeoTube* tube = (TGeoTube*) volume->GetShape();
      Double_t rmin = tube->GetRmin();
      Double_t rmax = tube->GetRmax();
      Double_t d    = 2. * tube->GetDz();

      // Full path to node 
      // Since there are now different pipe and mvd geomteries  it is more 
      // difficult to get the full path
      //TString nodeName = "/cave_1/pipevac1_0/" + volName + "_0";
      TString nodeName;
      TGeoNode* node1 = gGeoManager->GetTopVolume()->FindNode("pipevac1_0");
      if (node1) { // old ascii geometry for pipe
	TObjArray* nodes = node1->GetNodes();
	for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
	  TGeoNode* node = (TGeoNode*) nodes->At(iNode);
          TString tmpnodeName = node->GetName();
	  if (tmpnodeName.Contains(volName)) {
            nodeName = "/cave_1/pipevac1_0/" + volName + "_0";
            break;
	  } else if (tmpnodeName.Contains("mvd_")) {
            nodeName = "/cave_1/pipevac1_0/" + tmpnodeName 
	      + "/" + volName + "_0";
            break;
	  }
	}
      } else {
	// Find Pipe top node
	nodeName = "/cave_1/";
	TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
	for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
	  TGeoNode* node = (TGeoNode*) nodes->At(iNode);
	  TString nodeName1 = node->GetName();
	  cout << "Node: "<< nodeName1 <<endl;
	  nodeName1.ToLower();
	  if (nodeName1.Contains("pipe")) {
            nodeName +=  node->GetName();
            nodeName += "/";	  
	    // find pipevac1
	    TObjArray* nodes2 = node->GetNodes();
	    for (Int_t iiNode = 0; iiNode < nodes2->GetEntriesFast(); iiNode++) {
	      TGeoNode* node2 = (TGeoNode*) nodes2->At(iiNode);
	      TString nodeName2 = node2->GetName();
	      cout << "Node: "<< nodeName2 <<endl;
	      nodeName2.ToLower();
	      if (nodeName2.Contains("pipevac1")) {
		nodeName +=  node2->GetName();
		nodeName += "/";	  
		cout << "I am here " <<endl;
		// check if there is a volume with mvd in name in the pipevac
		TObjArray* nodes3 = node2->GetNodes();
		for (Int_t iiiNode = 0; iiiNode < nodes3->GetEntriesFast(); iiiNode++) {
		  TGeoNode* node3 = (TGeoNode*) nodes3->At(iiiNode);
		  TString nodeName3 = node3->GetName();
		  cout << "Node: "<< nodeName3 <<endl;
		  if (nodeName3.Contains(volName)) {
		    nodeName += volName;
		    nodeName +=  "_0";	  
                    goto Node_found;
		  } else if ( nodeName3.Contains("mvd") ) {
		    nodeName +=  node3->GetName();
		    nodeName += "/";	  
		    nodeName += volName;
		    nodeName +=  "_0";	  
                    goto Node_found;
		  }
		}
	      }
	    }
	  }
	}
      }
      
      Node_found:

      // Get z position of node
      Bool_t nodeFound = gGeoManager->cd(nodeName.Data());
      if ( ! nodeFound ) {
	cout << "-E- " << GetName() << "::SetMvdGeometry: Node " << nodeName
	     << " not found in geometry!" << endl;
	Fatal("SetMvdGeometry", "Node not found");
      }
      Double_t local[3] = {0., 0., 0.};  // Local centre of volume
      Double_t global[3];                // Global centre of volume
      gGeoManager->LocalToMaster(local, global);
      Double_t z = global[2];
      
      // Check for already existing station with the same ID
      // (Just in case, one never knows...)
      if ( fStationMap.find(iStation) != fStationMap.end() ) {
        cout << "-E- " << GetName() << "::GetMvdGeometry: " 
             << "Station " << iStation << " already in map!" << endl;
        Fatal("GetMvdGeometry", "Double station number in TGeoManager!");
      }

      // Create new CbmMvdStation and add it to the map
      fStationMap[iStation] = new CbmMvdStation(volName.Data(), iStation, volId,
				       	        z, d, rmin, rmax);
      fStationMap[iStation]->Print();
      
      iStation++;

    }     // Volume found

  } while ( volId > -1 );

   
  return iStation - 1;
}
// -----------------------------------------------------------------------------




// -------------------------------------------------------------------------

//void CbmMvdDigitize::PoissonSmearer( SimTrackerHitImplVec & simTrkVec ) {
    /**
     * Function that fluctuates charge (in units of electrons)
     * deposited on the fired pixels according to the Poisson
     * distribution...
     */
/*
    for (int ihit=0; ihit<int(simTrkVec.size()); ++ihit) {
	SimTrackerHitImpl * hit = simTrkVec[ihit];
	double charge = hit->getdEdx();
	double rng;
	if (charge > 1000.) { // assume Gaussian
	    double sigma = sqrt(charge);
	    rng = double(RandGauss::shoot(charge,sigma));
	    hit->setdEdx(rng);
	}
	else { // assume Poisson
	    rng = double(RandPoisson::shoot(charge));
	}
	hit->setdEdx(float(rng));
    }
} */
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------



// -----   Virtual private method SetParContainers   -----------------------
void CbmMvdDigitize::SetParContainers() {
    FairRunAna*    ana  = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    fGeoPar  = (CbmMvdGeoPar*)  (rtdb->getContainer("CbmMvdGeoPar"));
}
// -------------------------------------------------------------------------


			      
// -----    Virtual private method Init   ----------------------------------
InitStatus CbmMvdDigitize::Init() {

  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << "-I- Initialising " << GetName() << " ...." << endl; 

  
  // **********  RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- " << GetName() << "::Init: No FairRootManager!" << endl;
    return kFATAL;
  }

  // **********  Get input arrays
  fInputPoints = (TClonesArray*) ioman->GetObject(fBranchName);
  fMCTracks    = (TClonesArray*) ioman->GetObject("MCTrack");
  
  
  // **********  Register output array
  fDigis = new TClonesArray("CbmMvdDigi", 10000);
  ioman->Register("MvdDigi", "MVDRawData", fDigis, kTRUE);
  
  
  // **********  Get MVD geometry
  Int_t nStations = GetMvdGeometry();
  if ( ! nStations ) {
    cout << "-W- " << GetName() << "::Init: No MVD stations in geometry!"
	 << endl << "   Task will be inactive!" << endl;
    return kERROR;
  }


  // ********** Check for too many pileup / delta events
  if (fNPileup > 200) {
      cout << "-E- " << GetName() << "::Init:  Pileup of " << fNPileup
	  << " too large; maximum buffer size is 1000 events." << endl;
      return kERROR;
  }
  if (fNDeltaElect > 20000) {
      cout << "-E- " << GetName() << "::Init:  Delta Pileup of " << fNDeltaElect
	  << " too large; maximum buffer size is 100000 events." << endl;
      return kERROR;
  }


  // **********  Create pileup manager if necessary
  if (fNPileup >= 1 && !(fPileupManager) && fMode == 0 ) {
      if ( fBgFileName == "" ) {
	  cout << "-E- " << GetName() << "::Init: Pileup events needed, but no "
	      << " background file name given! " << endl;
	  return kERROR;
      }
      fPileupManager = new CbmMvdPileupManager(fBgFileName,
					       fBranchName, fBgBufferSize);
  }

  // **********   Create delta electron manager if required
  if (fNDeltaElect >= 1 && !(fDeltaManager) && fMode == 0 ) {
      if ( fDeltaFileName == "" ) {
	  cout << "-E- " << GetName() << "::Init: Pileup events needed, but no "
	      << " background file name given! " << endl;
	  return kERROR;
      }
      fDeltaManager = new CbmMvdPileupManager(fDeltaFileName,
					      fBranchName, fDeltaBufferSize);
  }

  // Screen output
  cout << GetName() << " initialised with parameters: " << endl;
  PrintParameters();
  cout << "---------------------------------------------" << endl;
  
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Virtual public method Reinit   ----------------------------------
InitStatus CbmMvdDigitize::ReInit() {

  // **********  Get MVD geometry
  Int_t nStations = GetMvdGeometry();
  if ( ! nStations ) {
    cout << "-W- " << GetName() << "::ReInit: No MVD stations in geometry!"
	 << endl << "   Task will be inactive!" << endl;
    return kERROR;
  }
   return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Virtual method Finish   -----------------------------------------
void CbmMvdDigitize::Finish() {
    cout.setf(ios_base::fixed, ios_base::floatfield);
    cout << endl << "---------------------------------------------" << endl;
    cout << "CbmMvdDigitize:: Parameters used for digitisation: " << endl;
    cout << "Pixel Size X               : " << setw(8) << setprecision(2)
	<< fPixelSizeX * 10000. << " mum" << endl;
    cout << "Pixel Size Y               : " << setw(8) << setprecision(2)
	<< fPixelSizeY * 10000. << " mum" << endl;
    cout << "Epitaxial layer thickness  : " << setw(8) << setprecision(2)
	<< fEpiTh * 10000. << " mum" << endl;
    cout << "Segment Length             : " << setw(8) << setprecision(2)
	<< fSegmentLength * 10000. << " mum" << endl;
    cout << "Diffusion Coefficient      : " << setw(8) << setprecision(2)
	<< fDiffusionCoefficient * 10000. << " mum" << endl;
    cout << "Width of Cluster           : " << setw(8) << setprecision(2)
	<< fWidthOfCluster << " * sigma " << endl;
    cout << "ElectronsPerKeV 3.62 eV/eh : " << setw(8) << setprecision(2)
	<< fElectronsPerKeV  <<  endl;
    cout << "CutOnDeltaRays             : " << setw(8) << setprecision(8)
	<< fCutOnDeltaRays  << " MeV " <<  endl;
    cout << "ChargeThreshold            : " << setw(8) << setprecision(2)
	<< fChargeThreshold  <<  endl;
    cout << "YOU USED THE GAUSS DIGITIZER!!! " <<  endl;
    cout << "---------------------------------------------" << endl;

	
}					       
// -------------------------------------------------------------------------


// -----   Private method Reset   ------------------------------------------
void CbmMvdDigitize::Reset() {
    fDigis->Clear("C");
}
// -------------------------------------------------------------------------  



// -----   Private method PrintParameters   --------------------------------
void CbmMvdDigitize::PrintParameters() {
    cout.setf(ios_base::fixed, ios_base::floatfield);
    cout << "Pixel Size X               : " << setw(8) << setprecision(2)
	<< fPixelSizeX * 10000. << " mum" << endl;
    cout << "Pixel Size Y               : " << setw(8) << setprecision(2)
	<< fPixelSizeY * 10000. << " mum" << endl;
    cout << "Epitaxial layer thickness  : " << setw(8) << setprecision(2)
	<< fEpiTh * 10000. << " mum" << endl;
    cout << "Segment Length             : " << setw(8) << setprecision(2)
	<< fSegmentLength * 10000. << " mum" << endl;
    cout << "Diffusion Coefficient      : " << setw(8) << setprecision(2)
	<< fDiffusionCoefficient * 10000. << " mum" << endl;
    cout << "Width of Cluster           : " << setw(8) << setprecision(2)
	<< fWidthOfCluster << " * sigma "<< endl;
    cout << "ElectronsPerKeV 3.62 eV/eh : " << setw(8) << setprecision(2)
	<< fElectronsPerKeV  <<  endl;
    cout << "CutOnDeltaRays             : " << setw(8) << setprecision(8)
	<< fCutOnDeltaRays  << " MeV " <<  endl;
    cout << "ChargeThreshold            : " << setw(8) << setprecision(2)
	<< fChargeThreshold  <<  endl;}
// -------------------------------------------------------------------------  



ClassImp(CbmMvdDigitize);
