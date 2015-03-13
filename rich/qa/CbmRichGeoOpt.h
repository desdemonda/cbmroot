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
   Int_t fEventNum2;
   Int_t fMinNofHits; // Min number of hits in ring for detector acceptance calculation.
 
   //TVector3 NormToXYPlane;
   //TVector3 XYPlane_P0;TVector3 XYPlane_P1;TVector3 XYPlane_P2;
   //double RotAngleX;
   //double RotAngleY;
   
   vector<TVector3> PlanePoints;
   TVector3 PMTPlaneCenter;
   TVector3 MirrorCenter;
   /**
    * \brief get MC Histos (P & Pt).
    */
   void FillMcHist();

   /**
    * \brief get point coordinates.
    */
   void FillPointsAtPMT();
   
   
   /**
    * \brief Initialize histograms.
    */
   void InitHistograms();
   
   /**
    * \brief write histograms to a root-file.
    */
   void WriteHistograms();
   
   /**
    * \brief Calculate residuals between hits and MC points and fill histograms.
    */
   void HitsAndPoints();
   /**
    * \brief Loop over all rings in array and fill ring parameters histograms.
    */
   void RingParameters();
   /**
    * \brief calculate distance between mirror center and pmt-point.
    */
   float GetDistanceMirrorCenterToPMTPoint(TVector3 PMTpoint);
   /**
    * \brief calculate intersection sphere-line.
    */
   float  GetIntersectionPointsLS( TVector3 MirrCenter,  TVector3 G_P1,  TVector3 G_P2, float R);
   
   /**
    * \brief Copy constructor.
    */
   CbmRichGeoOpt(const CbmRichGeoOpt&);
   
   
   /**
    * \brief histograms.
    */
   TH1D* H_MomPrim;
   TH1D* H_PtPrim;

   TH2D* H_Hits_XY; // distribution of X and Y position of hits
   TH2D* H_PointsIn_XY; // distribution of X and Y position of points
   TH2D* H_PointsOut_XY; // distribution of X and Y position of points (tilting pmt plane)
   TH1D* H_NofPhotonsPerHit; // Number of photons per hit
   TH1D* H_DiffXhit;
   TH1D* H_DiffYhit;
   
   /* TH1D* H_AlphaMomNormOfXY; */
   /* TH1D* H_AlphaMomPointsAtXY; */
   /* TH1D* H_AlphaMomPointsAtPMT; */
   /* TH1D* H_AlphaRefPointsNormOfXY; */
   TH1D* H_dFocalPoint_I;
   TH1D* H_dFocalPoint_II;
   TH1D* H_Alpha;
   TH1D* H_Alpha_UpLeft;
   TH3D* H_Alpha_XYposAtDet;

   TH1D* H_acc_mom_el;
   TH2D* H_acc_pty_el;

   //////////////////////////////////
   TH1D* H_NofHitsAll;

   //////////////////////////////////
   TH1D *H_RingCenterX;TH1D *H_RingCenterY;TH2D *H_RingCenter;
   TH1D *H_Radius; TH1D *H_aAxis; TH1D *H_bAxis; TH1D *H_boa; TH1D *H_dR;
   TH3D *H_RingCenter_Aaxis;  TH3D *H_RingCenter_Baxis;  TH3D *H_RingCenter_boa;

   /**
    * \brief Assignment operator.
    */
   CbmRichGeoOpt& operator=(const CbmRichGeoOpt&);
   
   
   ClassDef(CbmRichGeoOpt,1)
     };

#endif

