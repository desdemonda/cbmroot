/**
* \file CbmRichPrototypeQa.h
*
* \brief RICH geometry prototype geometry testing and checking.
*
* \author Semen Lebedev <s.lebedev@gsi.de>
* \date 2013
**/

#ifndef CBM_RICH_PROTOTYPE_QA
#define CBM_RICH_PROTOTYPE_QA

#include "FairTask.h"
class TH1;
class TH2;
class TH1D;
class TH2D;
class TH3D;
class TClonesArray;
class CbmRichRingFitterCOP;
class CbmRichRingFitterEllipseTau;
class CbmRichRing;
class CbmRichRingLight;
class CbmGeoRichPar;
class TCanvas;

#include <vector>

using namespace std;

/**
 * \class CbmRichPrototypeQa
 *
 * \brief RICH geometry prototype testing and checking.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2013
 **/
class CbmRichPrototypeQa : public FairTask
{

public:
   /**
    * \brief Standard constructor.
    */
   CbmRichPrototypeQa();

   /**
    * \brief Standard destructor.
    */
   virtual ~CbmRichPrototypeQa();

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

   /**
    * \brief Initialize histograms.
    */
   void InitHistograms();


   /**
    * \brief Loop over all rings in array and fill ring parameters histograms.
    */
	void RingParameters();
	
   /**
    * \brief Fit ring using ellipse fitter and fill histograms.
    * \param[in] ring Pointer to CbmRichRing to be fitted and filled in histograms.
    */
	void FitAndFillHistEllipse(
	      CbmRichRingLight* ring,
	      int hIndex);

   /**
    * \brief Fit ring using circle fitter and fill histograms.
    * \param[in] ring Pointer to CbmRichRingLight to be fitted and filled in histograms.
    */
   void FitAndFillHistCircle(
         CbmRichRingLight* ring,
         int hIndex);

   /**
    * \brief Calculate residuals between hits and MC points and fill histograms.
    */
   void HitsAndPoints(
         int hIndex);

   void CreatePionSuppression();

   /**
    *  \brief Draw histograms.
    */
	void DrawHist();

	/**
	 * \brief Draw ring in separate TCanvas.
	 * \param[in] ring Ring with RICH hits.
	 * \param[in] ringPoint Ring with MC RICH points.
	 */
	void DrawRing(
	      CbmRichRingLight* ringHit,
	      CbmRichRingLight* ringPoint);

	void DrawH1andFit(
	      TH1D* hist);

	/**
	 * \brief Calculate efficiency.
	 * \param[in] histRec
	 * \param[in] histAcc
	 */
	string CalcEfficiency(
	   TH1* histRec,
	   TH1* histAcc);

	/**
	 * \brief Divide two histograms and create third one.
	 */
	TH1D* DivideH1(
	   TH1D* h1,
	   TH1D* h2,
	   const string& name,
	   const string& title,
	   const string& axisX,
	   const string& axisY);


	TCanvas* CreateCanvas(
	      const string& name,
	      const string& title,
	      int width,
	      int height);

	void SaveCanvasToImage();

   /**
    * \brief Copy constructor.
    */
   CbmRichPrototypeQa(const CbmRichPrototypeQa&);

   /**
    * \brief Assignment operator.
    */
   CbmRichPrototypeQa& operator=(const CbmRichPrototypeQa&);

   string fOutputDir; // output dir for results

	TClonesArray* fRichHits;
	TClonesArray* fRichRings;
	TClonesArray* fRichPoints; 
	TClonesArray* fMCTracks;
	TClonesArray* fRichRingMatches; 

   // rings will be fitted on a fly
	CbmRichRingFitterCOP* fCopFit;
	CbmRichRingFitterEllipseTau* fTauFit;

	vector<TCanvas*> fCanvas;

	Int_t fEventNum;
	Int_t fMinNofHits; // Min number of hits in ring.

   //[0]-electrons, [1]-pions
	vector<TH2D*> fhHitsXY; // distribution of X and Y position of hits
	vector<TH2D*> fhPointsXY; // distribution of X and Y position of MC points
	vector<TH1D*> fhNofPhotonsPerHit; // Number of photons per hit
	vector<TH1D*> fhNofPointsEv; // number of points per event

   // fitting parameters
   vector<TH1D*> fhNofHitsEv; // number of hits per ring
   vector<TH1D*> fhNofHitsRing; // number of hits per ring
	// for ellipse
   vector<TH1D*> fhAaxis; // major axis (A)
   vector<TH1D*> fhBaxis; // minor axis (B)
   vector<TH1D*> fhBoverA; // B/A distribution
   // for circle
   vector<TH2D*> fhXcYcCircle; // (Xc, Yc) of circle center
   vector<TH1D*> fhRadius; // circle radius
   vector<TH1D*> fhDR; // dR


   // fitting efficiency
   Double_t fMinAaxis;
   Double_t fMaxAaxis;
   Double_t fMinBaxis;
   Double_t fMaxBaxis;
   Double_t fMinRadius;
   Double_t fMaxRadius;
   TH1D* fhNofHitsAll; // distribution of the number of hits in ring for all
   TH1D* fhNofHitsCircleFit; // distribution of the number of hits in ring for good fitted rings using circle  fitting
   TH1D* fhNofHitsEllipseFit; // distribution of the number of hits in ring for good fitted rings using ellipse  fitting
   TH1D* fhNofHitsCircleFitEff;
   TH1D* fhNofHitsEllipseFitEff;

	vector<TH1*> fHists; // store all TH1 pointers of the histogram

	Int_t fNofDrawnRings; // store number of drawn rings

	ClassDef(CbmRichPrototypeQa,1)
};

#endif

