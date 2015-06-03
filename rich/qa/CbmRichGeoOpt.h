/**
* \file CbmRichGeoOpt.h
*
* \brief Optimization of the RICH geometry.
*
* \author Tariq Mahmoud<t.mahmoud@gsi.de>
* \date 2014
**/

#ifndef CBM_RICH_GEO_OPT
#define CBM_RICH_GEO_OPT

#include "FairTask.h"
#include "CbmRichRecGeoPar.h"
#include "CbmRichPoint.h"
#include <sstream>
#include <fstream>     

class TVector3;
class TH1;
class TH2;
class TH1D;
class TH2D;
class TH3D;
class TClonesArray;
class CbmRichRing;
class CbmRichRingLight;

class TCanvas;

#include <vector>
#include <map>
#include "TVector3.h"

using namespace std;

/**
* \class CbmRichGeoOpt
*
* \brief Optimization of the RICH geometry.
*
* \author Tariq Mahmoud<t.mahmoud@gsi.de>
* \date 2014
**/
class CbmRichGeoOpt : public FairTask
{

public:
   /**
    * \brief Standard constructor.
    */
	CbmRichGeoOpt();

   /**
    * \brief Standard destructor.
    */
   virtual ~CbmRichGeoOpt();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(
		   Option_t* option);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();



private:

   TClonesArray* fRichPoints;
   TClonesArray* fMcTracks;
   TClonesArray* fRefPoints;
   TClonesArray* fRichHits;
   CbmRichRecGeoPar fGP; // RICH geometry parameters
   TClonesArray* fRichRings;
   TClonesArray* fRichRingMatches; 

   Int_t fEventNum;
   Int_t PointsFilled;
   Int_t fMinNofHits; // Min number of hits in ring for detector acceptance calculation.

   Int_t nPhotonsNotOnPlane;
   Int_t nPhotonsNotOnSphere;
   Int_t nTotalPhorons; 
   //TVector3 NormToXYPlane;
   //TVector3 XYPlane_P0;TVector3 XYPlane_P1;TVector3 XYPlane_P2;
   //double RotAngleX;
   //double RotAngleY;
   
   //Parameters to be read from a parameters file
   vector<TVector3> PlanePoints;
   TVector3 PMTPlaneCenter; 
   TVector3 ReadPMTPlaneCenter;
   TVector3 ReadPMTPlaneCenterOrig;
   TVector3 MirrorCenter; 
   //TVector3 ReadMirrorCenter;
   double RotX; 
   double RotY;
   TVector3 r1; 
   TVector3 r2; 
   TVector3 n;
   double PMTPlaneX; double PMTPlaneY;
   TVector3 MirrPosition;   //double MirrPosX; double MirrPosY; double MirrPosZ;
   //double PMTPlaneXatThird; double PMTPlaneYatThird;

   /**
    * \brief get MC Histos (P & Pt).
    */
   void FillMcHist();
      
   /**
    * \brief Initialize histograms.
    */
   void InitHistograms();
   
   /**
    * \brief write histograms to a root-file.
    */
   void WriteHistograms();
   
   /**
    * \brief get a point coressponding to point at sens-plane.
    */
   CbmRichPoint* GetPMTPoint(int TrackIdOfSensPlane);


   /**
    * \brief Calculate residuals between hits and MC points and fill histograms.
    */
   void HitsAndPoints();
   /**
    * \brief Loop over all rings in array and fill ring parameters histograms.
    */
   void RingParameters();
   
   /**
    * \brief get point coordinates.
    */
   void FillPointsAtPMT();
   
   /**
    * \brief calculate distance between mirror center and pmt-point.
    */
   float GetDistanceMirrorCenterToPMTPoint(TVector3 PMTpoint);
   /**
    * \ calculate intersection sphere-line.
    */
   float  GetIntersectionPointsLS( TVector3 MirrCenter,  TVector3 G_P1,  TVector3 G_P2, float R);
   
  /**
    * \ Check if a given point lies on a given sphare.
    */
   bool  CheckPointLiesOnSphere(TVector3 Point);
   
  /**
    * \brief Check if a given point lies on agiven plane.
    */
   bool  CheckPointLiesOnPlane(TVector3 Point,TVector3 p0,TVector3 n );
 /**
    * \brief Check if a given line intersects a given sphere.
    */
   bool  CheckLineIntersectsSphere(TVector3 Point);
   
/**
    * \brief Check if a given line intersects a given plane.
    */
   bool  CheckLineIntersectsPlane(TVector3 Point);

   /**
    * \get the x-y coordinate of the center of illuminated area of PMT plane
    */

   void GetPlaneCenter(float RotMir, float RotX, float RotY);

   /**
    * \get rotation angles of pmt plane around x- and y-axis
    */

   void GetPMTRotAngels();

   /**
   * \brief Copy constructor.
    */
   CbmRichGeoOpt(const CbmRichGeoOpt&);
   
   
  /**
    * \brief histograms.
    */
   
   TH1D* H_Theta_TwoVectors;

   TH1D* H_DistancePMTtoMirrCenter;
   TH1D* H_DistancePMTtoMirr;
   TH1D* H_MomPrim;
   TH1D* H_PtPrim;
   TH2D* H_MomPt;

   TH2D* H_Hits_XY; // distribution of X and Y position of hits
   TH2D* H_PointsIn_XY; // distribution of X and Y position of points
   TH2D* H_PointsOut_XY; // distribution of X and Y position of points (tilting pmt plane)
   TH1D* H_NofPhotonsPerEv; // Number of photons per hit
   TH1D* H_NofPhotonsPerHit; // Number of photons per hit
   TH1D* H_NofPhotonsSmallerThan30; // Number of photons per hit
   TH1D* H_DiffXhit;
   TH1D* H_DiffYhit;
   
   TH1D* H_dFocalPoint_Delta;
   TH1D* H_dFocalPoint_Rho;
   TH1D* H_Alpha;

   TH1D* H_Alpha_UpLeft;
   TH1D* H_Alpha_UpLeft_II;
   TH1D* H_Alpha_UpLeft_RegularTheta;
   TH1D* H_Alpha_UpLeft_LeftHalf;
   TH1D* H_Alpha_UpLeft_RightHalf;
   TH1D* H_Alpha_UpLeft_UpperHalf;
   TH1D* H_Alpha_UpLeft_LowerHalf;
   //H_Alpha_UpLeft_RighttHalf
   TH3D* H_Alpha_XYposAtDet;
   TH3D* H_Alpha_XYposAtDet_RegularTheta;
   TH3D* H_Alpha_XYposAtDet_LeftHalf;
   TH3D* H_Alpha_XYposAtDet_RightHalf;
   TH3D* H_Alpha_XYposAtDet_UpperHalf;
   TH3D* H_Alpha_XYposAtDet_LowerHalf;

   TH1D* H_acc_mom_el;
   TH2D* H_acc_pty_el;

   //////////////////////////////////
   TH1D* H_NofHitsAll;
   TH1D* H_NofRings;
   TH2D* H_NofRings_NofHits;

   //////////////////////////////////
   TH1D *H_RingCenterX;TH1D *H_RingCenterY;TH2D *H_RingCenter;
   TH1D *H_Radius; TH1D *H_aAxis; TH1D *H_bAxis;
   
   TH1D *H_boa; 
   TH1D *H_boa_RegularTheta; 
   TH1D *H_boa_LeftHalf;
   TH1D *H_boa_RightHalf;
   TH1D *H_boa_UpperHalf;
   TH1D *H_boa_LowerHalf;


   TH1D *H_dR;
   TH1D *H_dR_RegularTheta;
   TH1D *H_dR_LeftHalf;
   TH1D *H_dR_RightHalf;   
   TH1D *H_dR_UpperHalf;
   TH1D *H_dR_LowerHalf;

   TH3D *H_RingCenter_Aaxis;  TH3D *H_RingCenter_Baxis;

   TH3D *H_RingCenter_boa;
   TH3D *H_RingCenter_boa_RegularTheta;
   TH3D *H_RingCenter_boa_LeftHalf;
   TH3D *H_RingCenter_boa_RightHalf;
   TH3D *H_RingCenter_boa_UpperHalf;
   TH3D *H_RingCenter_boa_LowerHalf;

   TH3D *H_RingCenter_dR;
   TH3D *H_RingCenter_dR_RegularTheta;
   TH3D *H_RingCenter_dR_LeftHalf;
   TH3D *H_RingCenter_dR_RightHalf;
   TH3D *H_RingCenter_dR_UpperHalf;
   TH3D *H_RingCenter_dR_LowerHalf;

   /**
    * \brief Assignment operator.
    */
   CbmRichGeoOpt& operator=(const CbmRichGeoOpt&);
   
   
   ClassDef(CbmRichGeoOpt,1)
     };

#endif

