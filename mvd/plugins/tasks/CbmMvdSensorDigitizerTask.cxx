// -------------------------------------------------------------------------
// -----                  CbmMvdSensorDigitizerTask source file              -----
// -----                  Created 02.02.2012 by M. Deveaux            -----
// -------------------------------------------------------------------------
/**
 *
 * ____________________________________________________________________________________________
 * --------------------------------------------------------------------------------------------
 * adaptation for CBM: C.Dritsa
  * Acknowlegments to:
 *	Rita de Masi (IPHC, Strasbourg), M.Deveaux (IKF, Frankfurt), V.Friese (GSI, Darmstadt)
 *   Code tuning and maintainance M.Deveaux 01/07/2010
 *   Redesign as plugin: M. Deveaux 02.02.2012   
 * ____________________________________________________________________________________________
 * --------------------------------------------------------------------------------------------
 **/


#include "CbmMvdSensorDigitizerTask.h"

#include "TClonesArray.h"
#include "TObjArray.h"

#include "CbmMvdPoint.h"
#include "CbmMvdPileupManager.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"



// Includes from C++
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

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
CbmMvdSensorDigitizerTask::CbmMvdSensorDigitizerTask()
  : CbmMvdSensorTask(),
    fcurrentFrameNumber(0),
    fEpiTh(0.0014),
    fSegmentLength(0.0001),
    fDiffusionCoefficient(0.0055),
    fElectronsPerKeV(276.),
    fWidthOfCluster(3.5),
    fPixelSizeX(0.0030),
    fPixelSizeY(0.0030),
    fCutOnDeltaRays(0.00169720),
    fChargeThreshold(100.),
    fFanoSilicium(0.115), 
    fEsum(0.),
    fSegmentDepth(0.),
    fCurrentTotalCharge(0.),
    fCurrentParticleMass(0.),
    fCurrentParticleMomentum(0.),
    fCurrentParticlePdg(0),
    fRandomGeneratorTestHisto(NULL),
  fPosXY(NULL),
  fpZ(NULL),
  fPosXinIOut(NULL),
  fAngle(NULL),
  fSegResolutionHistoX(NULL),
  fSegResolutionHistoY(NULL),
  fSegResolutionHistoZ(NULL),
  fTotalChargeHisto(NULL),
  fTotalSegmentChargeHisto(NULL),
  fLorentzY0(-6.1),
  fLorentzXc(0.), 
  fLorentzW(1.03),
  fLorentzA(477.2),
  fLorentzNorm(1.),
  fLandauMPV(877.4),
  fLandauSigma(204.93),
  fLandauGain(3.3),
  fLandauRandom(new TRandom3()),
  fPixelSize(0.0025),
  fPar0(520.),
  fPar1(0.34),
  fPar2(-1.2),
  fCompression(1.),
  fResolutionHistoX(NULL),
  fResolutionHistoY(NULL),
  fNumberOfSegments(0),
  fCurrentLayer(0),
  fEvent(0),
  fVolumeId(0),
  fNPixelsX(0),
  fNPixelsY(0),
  fPixelCharge(new TClonesArray("CbmMvdPixelCharge")),
  fDigis(NULL),
  fDigiMatch(NULL),
  fPixelChargeShort(),
  fPixelScanAccelerator(NULL),
  fChargeMap(),
  fChargeMapIt(),
  fsensorDataSheet(NULL),
  fMode(0),
  fSigmaX(0.0005),
  fSigmaY(0.0005),
  fReadoutTime(0.00005),
  fEfficiency(-1.),
  fMergeDist(-1.),
  fFakeRate(-1.),
  fNPileup(0),
  fNDeltaElect(0),
  fDeltaBufferSize(0),
  fBgBufferSize(0),
  fBranchName(""),
  fBgFileName(""),
  fDeltaFileName(""),
  fInputPoints(NULL),
  fPoints(new TRefArray()),
  fRandGen(), 
  fTimer(),
  fPileupManager(NULL),
  fDeltaManager(NULL),
  fGeoPar(NULL),
  fNEvents(0),
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
    fRandGen.SetSeed(2736);
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdSensorDigitizerTask::CbmMvdSensorDigitizerTask(const char* name, Int_t iMode,
				 Int_t iVerbose)
  : CbmMvdSensorTask(),
    fcurrentFrameNumber(0),
    fEpiTh(0.0014),
    fSegmentLength(0.0001),
    fDiffusionCoefficient(0.0055),
    fElectronsPerKeV(276.),
    fWidthOfCluster(3.5),
    fPixelSizeX(0.0030),
    fPixelSizeY(0.0030),
    fCutOnDeltaRays(0.00169720),
    fChargeThreshold(100.),
    fFanoSilicium(0.115), 
    fEsum(0.),
    fSegmentDepth(0.),
    fCurrentTotalCharge(0.),
    fCurrentParticleMass(0.),
    fCurrentParticleMomentum(0.),
    fCurrentParticlePdg(0),
    fRandomGeneratorTestHisto(NULL),
  fPosXY(NULL),
  fpZ(NULL),
  fPosXinIOut(NULL),
  fAngle(NULL),
  fSegResolutionHistoX(NULL),
  fSegResolutionHistoY(NULL),
  fSegResolutionHistoZ(NULL),
  fTotalChargeHisto(NULL),
  fTotalSegmentChargeHisto(NULL),
  fLorentzY0(-6.1),
  fLorentzXc(0.), 
  fLorentzW(1.03),
  fLorentzA(477.2),
  fLorentzNorm(1.),
  fLandauMPV(877.4),
  fLandauSigma(204.93),
  fLandauGain(3.3),
  fLandauRandom(new TRandom3()),
  fPixelSize(0.0025),
  fPar0(520.),
  fPar1(0.34),
  fPar2(-1.2),
  fCompression(1.),
  fResolutionHistoX(NULL),
  fResolutionHistoY(NULL),
  fNumberOfSegments(0),
  fCurrentLayer(0),
  fEvent(0),
  fVolumeId(0),
  fNPixelsX(0),
  fNPixelsY(0),
  fPixelCharge(new TClonesArray("CbmMvdPixelCharge",100000)),
  fDigis(NULL),
  fDigiMatch(NULL),
  fPixelChargeShort(),
  fPixelScanAccelerator(NULL),
  fChargeMap(),
  fChargeMapIt(),
  fsensorDataSheet(NULL),
  fMode(iMode),
  fSigmaX(0.0005),
  fSigmaY(0.0005),
  fReadoutTime(0.00005),
  fEfficiency(-1.),
  fMergeDist(-1.),
  fFakeRate(-1.),
  fNPileup(0),
  fNDeltaElect(0),
  fDeltaBufferSize(0),
  fBgBufferSize(0),
  fBranchName("MvdPoint"),
  fBgFileName(""),
  fDeltaFileName(""),
  fInputPoints(NULL),
  fPoints(new TRefArray()),
  fRandGen(), 
  fTimer(),
  fPileupManager(NULL),
  fDeltaManager(NULL),
  fGeoPar(NULL),
  fNEvents(0),
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
  cout << "Starting CbmMvdSensorDigitizerTask::CbmMvdSensorDigitizerTask() "<< endl;

  fRandGen.SetSeed(2736);
    fEvent       = 0;
    fTime        = 0.;
    fSigmaX      = 0.0005;
    fSigmaY      = 0.0005;
    fReadoutTime = 0.00005;
   

    fEpiTh         = 0.0014;
    fSegmentLength = 0.0001;
    fDiffusionCoefficient = 0.0055; // correspondes to the sigma of the gauss with the max drift length
    fElectronsPerKeV = 276; //3.62 eV for e-h creation
    fWidthOfCluster  = 3.5; // in sigmas
    fPixelSizeX = 0.0025; // in cm
    fPixelSizeY = 0.0025;
    fCutOnDeltaRays  = 0.00169720;  //MeV
    fChargeThreshold = 100.; //electrons change 1 to 10
    fFanoSilicium    = 0.115;
    fEsum            = 0;
    fSegmentDepth    = 0;
    fCurrentTotalCharge      = 0;
    fCurrentParticleMass     = 0;
    fCurrentParticleMomentum = 0;
    fPixelScanAccelerator    = 0;


    fPixelSize = 0.0025;
    fPar0 = 520.;
    fPar1 = 0.34;
    fPar2 = -1.2;
    fLandauMPV   = 877.4;
    fLandauSigma = 204.93;
    fLandauGain= 3.3;

    fLorentzY0=-6.1;
    fLorentzXc=0.;
    fLorentzW=1.03;
    fLorentzA=477.2;


    fCompression = 1.;  


    //fLorentzNorm=0.00013010281679422413;
    fLorentzNorm=1;

    
    fcurrentFrameNumber = 0;


 }


// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdSensorDigitizerTask::~CbmMvdSensorDigitizerTask() {

    if ( fPileupManager ) delete fPileupManager;
    if ( fDeltaManager  ) delete fDeltaManager;
    if ( fInputPoints) {fInputPoints->Delete(); delete fInputPoints;}
    if ( fOutputBuffer) {fOutputBuffer->Delete(); delete fOutputBuffer;}

}
// ------------------------------------------------------------------------
  
  
  
// -----    Virtual private method ReadSensorInformation   ----------------
InitStatus CbmMvdSensorDigitizerTask::ReadSensorInformation() {

  CbmMvdSensorDataSheet* sensorData;
  sensorData=fSensor->GetDataSheet();
  if (!sensorData){return kERROR;}
  
  fPixelSizeX = sensorData->GetPixelPitchX();
  fPixelSizeY = sensorData->GetPixelPitchY();
  fNPixelsX = sensorData->GetNPixelsX();
  fNPixelsY = sensorData->GetNPixelsY();

  fChargeThreshold = sensorData->GetChargeThreshold();
  
  fPar0=sensorData->GetLorentzPar0();
  fPar1=sensorData->GetLorentzPar1();
  fPar2=sensorData->GetLorentzPar2(); //cout<< endl << " LorentzPar2 is now set to " << fPar2 << endl;
  
  fLandauMPV  =sensorData->GetLandauMPV(); //cout << endl << " Landau MPV is now set to " << fLandauMPV << endl;
  fLandauSigma=sensorData->GetLandauSigma();//cout << endl << " Landau Sigma is now set to " << fLandauSigma << endl;
  fLandauGain =sensorData->GetLandauGain();//cout << endl << " Landau Gain is now set to " << fLandauGain << endl;
  fEpiTh=      sensorData->GetEpiThickness();//cout << endl << " Epitaxial thickness is now set to " << fEpiTh << endl;
  fCompression = fPixelSizeX / fPixelSizeY;

  if (fCompression != 1)
	//cout << endl << "working with non uniform pixels" << endl;
  if (fCompression <= 0)
	fCompression = 1;
  return kSUCCESS;
}

  

// -----------------------------------------------------------------------------

void CbmMvdSensorDigitizerTask::SetInputArray (TClonesArray* inputStream){
  
  CbmMvdPoint* point;
  Int_t i=0;
  Int_t nInputs = inputStream->GetEntriesFast();
   while (nInputs > i)   
	  {
     	      new((*fInputPoints)[fInputPoints->GetEntriesFast()]) CbmMvdPoint(*((CbmMvdPoint*)inputStream->At(i)));
             
	      //cout << endl << "New Input registered" << endl;
	  i++;
	  }
} 
// -----------------------------------------------------------------------------
void CbmMvdSensorDigitizerTask::SetInput(CbmMvdPoint* point){

new((*fInputPoints)[fInputPoints->GetEntriesFast()]) CbmMvdPoint(*((CbmMvdPoint*)point));


} 


// -------------- public method ExecChain   ------------------------------------
void CbmMvdSensorDigitizerTask::ExecChain() {
 
//   cout << endl << "start Digitizer on sensor " << fSensor->GetName() << endl;
  

  Exec();

}

// -----   Virtual public method Exec   ------------------------------------
void CbmMvdSensorDigitizerTask::Exec() {
  
if(fPreviousPlugin)
  {
   fInputPoints->Delete();
  fInputPoints->AbsorbObjects(fPreviousPlugin->GetOutputArray());
  }
fOutputBuffer->Clear("C");
fDigis->Clear("C");
fDigiMatch->Clear("C");
if(fInputPoints->GetEntriesFast() > 0)
  {

for (Int_t iPoint=0; iPoint<fInputPoints->GetEntriesFast(); iPoint++)
    {
   
     CbmMvdPoint* point=(CbmMvdPoint*)fInputPoints->At(iPoint);
       
      if (!point) 
	  {
	  cout << "-W-" << GetName() << ":: Exec:" <<endl;
	  cout << "    -received bad MC-Point. Ignored." << endl;
	  continue;
	  } 
      if (point->GetStationNr() != fSensor->GetSensorNr())
	{
	  cout << "-W-" << GetName() << ":: Exec:" <<endl;
	  cout << "    -received bad MC-Point which doesn't belong here. Ignored." << endl;
	  continue;	
	}
      fcurrentFrameNumber = point->GetFrame();
    //The digitizer acts only on particles, which crossed the station.
    //Particles generated in the sensor or being absorbed in this sensor are ignored
      if(TMath::Abs(point->GetZOut()-point->GetZ())<0.9*fEpiTh) 
	  {
		//cout << endl << "hit not on chip with thickness " << 0.9* 2 *fSensor->GetDZ() << endl;
		//cout << endl << "hit not on chip with length " << TMath::Abs(point->GetZOut()-point->GetZ()) << endl;
		continue;}
    // Reject for the time being light nuclei (no digitization modell yet)
      if ( point->GetPdgCode() > 100000) 
	  {continue;}
    // Digitize the point
     //cout << endl << " found point make digi" << endl;
     ProduceIonisationPoints(point);
     ProducePixelCharge(point);
    } //loop on MCpoints

     
 
 
for (Int_t i=0; i<fPixelCharge->GetEntriesFast(); i++)
	{ 
        CbmMvdPixelCharge* pixel = (CbmMvdPixelCharge*)fPixelCharge->At(i);
           
	    if ( pixel->GetCharge()>fChargeThreshold )
	    {
		Int_t nDigis = fDigis->GetEntriesFast();
		new ((*fDigis)[nDigis]) CbmMvdDigi (fSensor->GetSensorNr(),
						    pixel->GetX(), pixel->GetY(), pixel->GetCharge(),
						    fPixelSizeX, fPixelSizeY, pixel->GetTime(), 
						    pixel->GetFrame());

		//cout << endl << " new Digi at " <<  pixel->GetX() << " " << pixel->GetY() << endl;
		new ((*fOutputBuffer)[nDigis]) CbmMvdDigi (fSensor->GetSensorNr(),
						    pixel->GetX(), pixel->GetY(), pixel->GetCharge(),
						    fPixelSizeX, fPixelSizeY, pixel->GetTime(), 
                                                    pixel->GetFrame());

 		new ((*fDigiMatch)[nDigis]) CbmMatch();
                CbmMatch* match = (CbmMatch*)fDigiMatch->At(nDigis);
		for(Int_t iLink = 0; iLink < pixel->GetNContributors(); iLink++)
			{
	                match->AddLink((Double_t) pixel->GetPointWeight()[iLink],pixel->GetPointID()[iLink]);
			}		    
	    }
		else 
		{ 
		//cout << endl << "charge under threshold, digi rejected" << endl;
		}
	}

 }

else { //cout << endl << "No input found." << endl;
     }

 fPixelCharge->Delete();
 fChargeMap.clear();
 fInputPoints->Delete();
 fSignalPoints.clear();
 
}// end of exec

// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMvdSensorDigitizerTask::ProduceIonisationPoints(CbmMvdPoint* point) {
  /** Produces ionisation points along track segment within 
   ** the active Silicon layer.
   **/
  
 //Option_t* opt1;
 //cout << endl << "Computing Point "   << endl;
 //point->Print(opt1);
  
  Int_t pdgCode = point->GetPdgCode();

  //Transform coordinates of the point into sensor frame
  
  Double_t globalPositionIn[3] ={point->GetX(), point->GetY(), point->GetZ()};
  Double_t globalPositionOut[3]={point->GetXOut(), point->GetYOut(), point->GetZOut()};
  
  Double_t localPositionIn[3]  ={0,0,0};
  Double_t localPositionOut[3] ={0,0,0};
    
  fSensor->TopToLocal(globalPositionIn, localPositionIn);
  fSensor->TopToLocal(globalPositionOut, localPositionOut);

  //if (fShowDebugHistos)cout << endl << " partical from global " << globalPositionIn[0] << " to local " << localPositionIn[0] << endl;

  Int_t pixelX, pixelY;
  fSensor->LocalToPixel(&localPositionIn[0],pixelX, pixelY); 

  //if (fShowDebugHistos)cout << endl << "hit pixel number " << pixelX << ", " << pixelY << endl;
  // Copy results into variables used by earlier versions
  
  Double_t entryX = localPositionIn [0];
  Double_t exitX  = localPositionOut[0];
  Double_t entryY = localPositionIn [1];
  Double_t exitY  = localPositionOut[1];
  Double_t entryZ = localPositionIn [2];
  Double_t exitZ  = localPositionOut[2];

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

    
    Double_t lx        = -(entryXepi-exitXepi); //length of segment x-direction
    Double_t ly        = -(entryYepi-exitYepi);
    Double_t lz        = -(entryZepi-exitZepi);
   

    //-----------------------------------------------------------


    Double_t rawLength = sqrt( lx*lx + ly*ly + lz*lz );  //length of the track inside the epi-layer, in cm
    Double_t trackLength = 0;

    if(rawLength<1.0e+3) { trackLength = rawLength; }

    else{
	cout << "-W- "<< GetName() << " : rawlength > 1.0e+3 : "<< rawLength << endl;
	trackLength = 1.0e+3;
    }

    //Smear the energy on each track segment
     Double_t charge = fLandauRandom->Landau(fLandauGain,fLandauSigma/fLandauMPV);

 //if (fShowDebugHistos )  cout << endl << "charge " << charge << endl;
    
    if (charge>(12000/fLandauMPV)){charge=12000/fLandauMPV;} //limit Random generator behaviour
   
    if (fShowDebugHistos ){fRandomGeneratorTestHisto->Fill(charge*fLandauMPV);}
    //Translate the charge to normalized energy
    
//     cout << endl << "charge after random generator " << charge << endl;
    Double_t dEmean = charge / (fElectronsPerKeV * 1e6);
 //   cout << endl << "dEmean " << dEmean << endl;
    fNumberOfSegments = int(trackLength/fSegmentLength) + 1;

    dEmean = dEmean*((Double_t)trackLength/fEpiTh);//scale the energy to the track length

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

    Double_t xDebug=0,yDebug=0,zDebug=0;
    Float_t totalSegmentCharge=0;

    for (int i=0; i<fNumberOfSegments; ++i) {
       
	z = -fEpiTh/2 + ((double)(i)+0.5)*fSegmentDepth; //middle position of the segment; zdirection
	x = entryXepi + ((double)(i)+0.5)*( lx/( (Double_t)fNumberOfSegments) ); //middle position of the segment; xdirection
        y = entryYepi + ((double)(i)+0.5)*( ly/( (Double_t)fNumberOfSegments) ); //middle position of the segment; ydirection

	if (fShowDebugHistos ){
	    xDebug=xDebug + x;
	    yDebug=yDebug + y;
	    zDebug=zDebug + z;
	};

	SignalPoint* sPoint=&fSignalPoints[i];
	
	fEsum = fEsum + dEmean;
	sPoint->eloss = dEmean;
	sPoint->x = x; //here the coordinates x,y,z are given in the sensor reference frame.
	sPoint->y = y;
	sPoint->z = z;
	charge      = 1.0e+6*dEmean*fElectronsPerKeV;
	//cout << endl << "charge " << charge << endl;
	sPoint->sigmaX = fPixelSize;
	sPoint->sigmaY = fPixelSize;
	sPoint->charge = charge;
	totalSegmentCharge=totalSegmentCharge+charge;
    }
//if (fShowDebugHistos  )cout << endl << "totalSegmentCharge " << totalSegmentCharge << endl;
    if (fShowDebugHistos  ){
        fTotalSegmentChargeHisto->Fill(totalSegmentCharge*fLandauMPV);
       	fSegResolutionHistoX->Fill(xDebug/fNumberOfSegments - (point->GetX()+point->GetXOut())/2-fSensor->GetX());
	fSegResolutionHistoY->Fill(yDebug/fNumberOfSegments- (point->GetY()+point->GetYOut())/2-fSensor->GetY());
	fSegResolutionHistoZ->Fill(zDebug/fNumberOfSegments - (point->GetZ()+point->GetZOut())/2 - fSensor->GetZ());
    }


}



// -------------------------------------------------------------------------

void CbmMvdSensorDigitizerTask::ProducePixelCharge(CbmMvdPoint* point) {
        /** Simulation of fired pixels. Each fired pixel is considered
     * as SimTrackerHit
     */

    fCurrentTotalCharge = 0.0;
   

    // MDx - Variables needed in order to compute a "Monte Carlo Center of Gravity" of the cluster

    Float_t xCharge=0., yCharge=0., totClusterCharge=0.;
    CbmMvdPixelCharge* pixel;

    pair<Int_t, Int_t> thispoint;
    
    Double_t xCentre, yCentre, sigmaX, sigmaY, xLo, xUp, yLo, yUp;
    
    SignalPoint* sPoint;
    sPoint= &fSignalPoints[0];
    
    xCentre = sPoint->x;  //of segment
    yCentre = sPoint->y;  /// idem
    sigmaX  = sPoint->sigmaX;
    sigmaY  = sPoint->sigmaY;
        
    xLo = sPoint->x - fWidthOfCluster*sPoint->sigmaX;
    xUp = sPoint->x + fWidthOfCluster*sPoint->sigmaX;
    yLo = sPoint->y - fWidthOfCluster*sPoint->sigmaY;
    yUp = sPoint->y + fWidthOfCluster*sPoint->sigmaY;

    if (fNumberOfSegments<2){Fatal("-E- CbmMvdDigitizer: ","fNumberOfSegments < 2, this makes no sense, check parameters.");}

    Int_t* lowerXArray=new Int_t[fNumberOfSegments];
    Int_t* upperXArray=new Int_t [fNumberOfSegments];
    Int_t* lowerYArray=new Int_t [fNumberOfSegments];
    Int_t* upperYArray=new Int_t [fNumberOfSegments];
   
    Int_t ixLo, ixUp, iyLo, iyUp;

    
    Double_t minCoord[] = {xLo, yLo};
    Double_t maxCoord[] = {xUp, yUp};
    
    
    fSensor->LocalToPixel(minCoord, lowerXArray[0], lowerYArray[0]);
    fSensor->LocalToPixel(maxCoord, upperXArray[0], upperYArray[0]);

    
    if(lowerXArray[0] < 0)lowerXArray[0]=0;
    if(lowerYArray[0] < 0)lowerYArray[0]=0;
    if(upperXArray[0] > fNPixelsX)upperXArray[0]=fNPixelsX;
    if(upperYArray[0] > fNPixelsY)upperYArray[0]=fNPixelsY;
    
     ixLo=lowerXArray[0];
     iyLo=lowerYArray[0];
     ixUp=upperXArray[0];
     iyUp=upperYArray[0];


    for (Int_t i=1; i<fNumberOfSegments;i++) {
    	
	sPoint= &fSignalPoints[i];
    	sigmaX  = sPoint->sigmaX;
    	sigmaY  = sPoint->sigmaY;
	
	minCoord[0] = sPoint->x - fWidthOfCluster*sPoint->sigmaX;
	minCoord[1] = sPoint->y - fWidthOfCluster*sPoint->sigmaY;
        maxCoord[0] = sPoint->x + fWidthOfCluster*sPoint->sigmaX;
	maxCoord[1] = sPoint->y + fWidthOfCluster*sPoint->sigmaY;
  
	
	fSensor->LocalToPixel(minCoord, lowerXArray[i], lowerYArray[i]);
        fSensor->LocalToPixel(maxCoord, upperXArray[i], upperYArray[i]);
	       
	if(lowerXArray[i] < 0)lowerXArray[i]=0;
	if(lowerYArray[i] < 0)lowerYArray[i]=0;

	if(upperXArray[i] > fNPixelsX)upperXArray[i]=fNPixelsX;
	if(upperYArray[i] > fNPixelsY)upperYArray[i]=fNPixelsY;

    	if (ixLo > lowerXArray[i]){ixLo = lowerXArray[i];}
    	if (ixUp < upperXArray[i]){ixUp = upperXArray[i];}
    	if (iyLo > lowerYArray[i]){iyLo = lowerYArray[i];}
    	if (iyUp < upperYArray[i]){iyUp = upperYArray[i];}
    }


//cout << "Scanning from x= " << ixLo << " to " <<ixUp <<" and  y= "<<iyLo<< " to " << iyUp << endl;
    
    // loop over all pads of interest. 
fPixelChargeShort.clear();   
Int_t ix, iy;    

    
    for (ix = ixLo; ix < ixUp+1; ix++) {
// cout << endl <<"loop for " << ix << endl;
	    for (iy = iyLo; iy < iyUp+1; iy++) {
// cout << endl <<"loop in " << iy << endl;
		//calculate the position of the current pixel in the lab-system

		Double_t Current[3];
		fSensor->PixelToLocal(ix,iy,Current);
		pixel=0; //decouple pixel-pointer from previous pixel
		//loop over segments, check if the pad received some charge

    		for (Int_t i=0; i<fNumberOfSegments; ++i) {
// 			cout << endl << "check segment nr. " << i << " from " << fNumberOfSegments << endl;
			// ignore pads, which are out of reach for this segments
			if(ix<lowerXArray[i]){continue;}
			if(iy<lowerYArray[i]){continue;}
			if(ix>upperXArray[i]){continue;}
			if(iy>upperYArray[i]){continue;}

// cout << endl << "found vallied pad " << i << endl;
			sPoint = &fSignalPoints[i];

			xCentre = sPoint->x;  //of segment
			yCentre = sPoint->y;  // idem
			sigmaX  = sPoint->sigmaX;
			sigmaY  = sPoint->sigmaY;
        
			fCurrentTotalCharge += sPoint->charge;
 			
			//compute the charge distributed to this pixel by this segment 
Float_t totCharge = (sPoint->charge * fLorentzNorm *(0.5*fPar0*fPar1/TMath::Pi())/
	TMath::Max(1.e-10, (((Current[0]-xCentre)*(Current[0]-xCentre))+((Current[1]-yCentre)*(Current[1]-yCentre)))/fPixelSize/fPixelSize+0.25*fPar1*fPar1)+fPar2);
			
			if(totCharge<1){
			 
//    			 cout << endl << "charge is " << totCharge << " < 1 electron thus charge is negligible" << endl;
			   continue;
			} //ignore negligible charge (< 1 electron)
			if(!pixel) {
//      			  cout << endl << "charge is " << totCharge << " > 1 electron thus pixel is firred at "<< ix << " " << iy << endl;
		 		// Look for pixel in charge map if not yet linked.
     				thispoint = std::make_pair(ix,iy);
//   				cout << endl << "creat pair at "<< ix << " " << iy << endl;
				fChargeMapIt = fChargeMap.find(thispoint);
//   				cout << endl << "found pair at "<< ix << " " << iy << endl;
//   				cout << endl << "Map size is now " << fChargeMap.size() << endl;
    				// Pixel not yet in map -> Add new pixel
    				if ( fChargeMapIt == fChargeMap.end() ) {
					pixel= new ((*fPixelCharge)[fPixelCharge->GetEntriesFast()])
	    				  CbmMvdPixelCharge(totCharge, ix, iy, point->GetPointId(),point->GetTrackID(),
							    (point->GetX()+point->GetXOut())/2, 
							    (point->GetY()+point->GetXOut())/2, point->GetTime(), point->GetFrame()
							   );
  //cout << endl << "new charched pixel with charge " << totCharge << " at " << ix << " " << iy << endl;
 					  fPixelChargeShort.push_back(pixel);
// 				cout << endl << "added pixel to ChargeShort vector " << endl;   
					
					fChargeMap[thispoint] = pixel;
					
				}

    				// Pixel already in map -> Add charge
    				else {  pixel = fChargeMapIt->second;
					//if ( ! pixel ) Fatal("AddChargeToPixel", "Zero pointer in charge map!");
					pixel->AddCharge(totCharge);
//  					if(pixel->GetCharge()>150)
// 					{cout << endl << "added charge to pixel summing up to "<< pixel->GetCharge() << endl;}
				}
				
			}
			else{	//pixel already linked => add charge only
				pixel->AddCharge(totCharge);
// 				cout<<"put charge" << endl;
			}
				

			
			if(fShowDebugHistos ){
		    	xCharge=xCharge + Current[0] * totCharge;
		    	yCharge=yCharge + Current[1] * totCharge;
		    	totClusterCharge=totClusterCharge + totCharge;
			} // end if
		} // end for (track segments)
	
		  
		}//for y
    }// for x
//    cout << endl << "End of loops " << endl;
  std::vector<CbmMvdPixelCharge*>::size_type vectorSize=fPixelChargeShort.size();

    for(Int_t f=0;f<vectorSize; f++)
	{
	  CbmMvdPixelCharge* pixelCharge =  fPixelChargeShort.at(f);
	  if(pixelCharge)
	  {pixelCharge->DigestCharge( ( (float)( point->GetX()+point->GetXOut() )/2 ) , ( (float)( point->GetY()+point->GetYOut() )/2 ), point->GetPointId(), point->GetTrackID());}
	  else
	  {
	    cout << endl << "Warning working on broken pixel " << endl;
	  }
	  
	  
	}
	
  
  
if (fShowDebugHistos ) {
//cout << endl << "produced " << fPixelChargeShort.size() << " Digis with total charge of " << totClusterCharge << endl;
  TVector3 momentum, position;
if (totClusterCharge > 0)fTotalChargeHisto->Fill(totClusterCharge); 
  point->Position(position);
  point->Momentum(momentum);
   fPosXY->Fill(position.X(),position.Y());
   fpZ->Fill(momentum.Z());
  fPosXinIOut->Fill(point->GetZ()-point->GetZOut());
  fAngle -> Fill (TMath::ATan ( momentum.Pt()/momentum.Pz())*180/TMath::Pi());
}               

    delete [] lowerXArray;
    delete [] upperXArray;
    delete [] lowerYArray;
    delete [] upperYArray;
    

}//end of function



// -------------------------------------------------------------------------

//void CbmMvdSensorDigitizerTask::PoissonSmearer( SimTrackerHitImplVec & simTrkVec ) {
    /*
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
void CbmMvdSensorDigitizerTask::SetParContainers() {
    FairRunAna*    ana  = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    fGeoPar  = (CbmMvdGeoPar*)  (rtdb->getContainer("CbmMvdGeoPar"));
}
// -------------------------------------------------------------------------


			      
// -----    Virtual private method Init   ----------------------------------
void CbmMvdSensorDigitizerTask::Init(CbmMvdSensor* mySensor) {
  
  //Read information on the sensor von data base
  fSensor = mySensor;

 // cout << "-I- " << GetName() << ": Initialisation of sensor " << fSensor->GetName() << endl;
  
   FairRootManager* ioman = FairRootManager::Instance();
 
  
  
    fDigis = new TClonesArray("CbmMvdDigi", 10000);
    fDigiMatch = new TClonesArray("CbmMatch", 10000);

    fOutputBuffer= new TClonesArray("CbmMvdDigi", 10000);  
    fInputPoints = new TClonesArray("CbmMvdPoint",10000); 
    
  if (!fSensor) {
    Fatal(GetName(), "Fatal error: Init(CbmMvdSensor*) called without valid pointer, don't know how to proceed.");
  }; 
  
InitStatus initSuccess=ReadSensorInformation();
 
  // Initialize histogramms used for debugging  
   
  if (fShowDebugHistos) {
	cout << endl << "Show debug histos in this Plugin" << endl;
	fRandomGeneratorTestHisto = new TH1F("TestHisto","TestHisto",1000,0,12000);
	fResolutionHistoX=new TH1F ("DigiResolutionX","DigiResolutionX", 1000, -.005,.005);
	fResolutionHistoY=new TH1F ("DigiResolutionY","DigiResolutionY", 1000, -.005,.005);
	fPosXY= new TH2F("DigiPointXY","DigiPointXY", 100,-6,6,100,-6,6);
	fpZ= new TH1F ("DigiPointPZ","DigiPointPZ", 1000, -0.5,0.5);
	fPosXinIOut = new TH1F("DigiZInOut","Digi Z InOut", 1000, -0.04,0.04);
	fAngle = new TH1F ("DigiAngle","DigiAngle", 1000,0,90);
	fSegResolutionHistoX= new TH1F("SegmentResolutionX","SegmentResolutionX",1000, -.005,.005);
	fSegResolutionHistoY= new TH1F("SegmentResolutionY","SegmentResolutionY",1000, -.005,.005);
        fSegResolutionHistoZ= new TH1F("SegmentResolutionZ","SegmentResolutionZ",1000, -.005,.005);
	fTotalChargeHisto=new TH1F("TotalChargeHisto","TotalChargeHisto",1000,0,12000);
	fTotalSegmentChargeHisto=new TH1F("TotalSegmentChargeHisto","TotalSegmentChargeHisto",1000,0,12000);

    }

  /** Screen output
  cout << GetName() << " initialised with parameters: " << endl;
  PrintParameters();
  cout << "---------------------------------------------" << endl;**/

  fPreviousPlugin = NULL;
  initialized = kTRUE;
 
}
// -------------------------------------------------------------------------



// -----   Virtual public method Reinit   ----------------------------------
void CbmMvdSensorDigitizerTask::ReInit(CbmMvdSensor* sensor) {

  delete fOutputBuffer;
  
  Init(sensor);
    
}
// -------------------------------------------------------------------------



// -----   Virtual method Finish   -----------------------------------------
void CbmMvdSensorDigitizerTask::Finish() {


   // PrintParameters();


    if (fShowDebugHistos){
	TCanvas* c=new TCanvas("DigiCanvas","DigiCanvas", 150,10,800,600);
	c->Divide(3,3);
	c->cd(1);
        fResolutionHistoX->Draw();
        fResolutionHistoX->Write();
        c->cd(2);
	fResolutionHistoY->Draw();
	fResolutionHistoY->Write();
	c->cd(3);
	fPosXY->Draw();
	fPosXY->Write();
        c->cd(4);
	fpZ->Draw();
	fpZ->Write();
        c->cd(5);
	fPosXinIOut->Draw();
	fPosXinIOut->Write();
        c->cd(6);
	fAngle->Draw();
	fAngle->Write();
	c->cd(7);
	//fSegResolutionHistoX->Draw();
	fSegResolutionHistoX->Write();
	fTotalSegmentChargeHisto->Draw();
	fTotalSegmentChargeHisto->Write();
        c->cd(8);
	fRandomGeneratorTestHisto->Draw();
	fRandomGeneratorTestHisto->Write();

	fSegResolutionHistoY->Write();
        c->cd(9);
	fTotalChargeHisto->Draw();
	fTotalChargeHisto->Write();
	cout << "-I- CbmMvdDigitizerL::Finish - Fit of the total cluster charge"<< endl;
	fTotalChargeHisto->Fit("landau");
	cout << "=============================================================="<< endl; 	 	
	// new TCanvas();
	//fTotalChargeHisto->Draw();
    };

	
}					       
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void CbmMvdSensorDigitizerTask::Reset() {


}
// -------------------------------------------------------------------------  



// -----   Private method PrintParameters   --------------------------------
void CbmMvdSensorDigitizerTask::PrintParameters() {
    
    cout.setf(ios_base::fixed, ios_base::floatfield);
    cout << "============================================================" << endl;
    cout << "============== Parameters of the Lorentz - Digitizer =======" << endl;
    cout << "============================================================" << endl;
    
    
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
	<< fCutOnDeltaRays  << " MeV " <<  endl;    cout << "ChargeThreshold            : " << setw(8) << setprecision(2)
	<< fChargeThreshold  <<  endl;
    cout << "Pileup: " << fNPileup << endl;
    cout << "Delta - Pileup: " << fNDeltaElect << endl;
    cout << "=============== End Parameters Digitizer ===================" << endl;
 
}
// -------------------------------------------------------------------------  



ClassImp(CbmMvdSensorDigitizerTask);




ClassImp(CbmMvdSensorDigitizerTask)
