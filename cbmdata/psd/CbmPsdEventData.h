/** CbmPsdHit.h
 **@author Alla Maevskaya <alla@inr.ru>
 **@author Selim SEDDIKI <s.seddiki at gsi.de>
 **@since 23.10.2012
 **@version 1.0
 **
 ** Data class for PSD reconstruction
 ** Energy deposition per module
 **
 ** Modified to implement event plane method (S. Seddiki)
 **/


#ifndef CBMPSDEventData_H
#define CBMPSDEventData_H 1


#include "CbmDetectorList.h"
#include "TNamed.h"
#include <vector>
#include "TH2F.h"

// Data container class to store reconstructed data in output of event plane reconstruction

class CbmPsdEventData : public TNamed
{

    public:

	/**   Default constructor   **/
	CbmPsdEventData() ;
	CbmPsdEventData(const char *name) ;

	// CbmPsdEventData(Float_t rp, Float_t b) ;

	/**   Destructor   **/
	virtual ~CbmPsdEventData();

	/**   Setters  **/
           
	void SetEvtID(Int_t id) {fevtID = id;}	

	void SetQvect(Double_t Qx, Double_t Qy, Double_t Qx_sub1, Double_t Qy_sub1, Double_t Qx_sub2, Double_t Qy_sub2, Double_t Qx_sub3, Double_t Qy_sub3)
	{
	    fQx = Qx;
	    fQy = Qy;

	    // Sub-event method

            fQx_sub1 = Qx_sub1;
	    fQy_sub1 = Qy_sub1;
            fQx_sub2 = Qx_sub2;
	    fQy_sub2 = Qy_sub2;
	    fQx_sub3 = Qx_sub3;
	    fQy_sub3 = Qy_sub3;
	}

	void SetEP(Double_t ep, Double_t ep_sub1, Double_t ep_sub2, Double_t ep_sub3)
	{
	    fEP_RECO = ep;

	    // Sub-event method

	    fEP_sub1 = ep_sub1;
	    fEP_sub2 = ep_sub2;
            fEP_sub3 = ep_sub3;
	}

	void SetdeltaEP(Double_t deltaEP, Double_t deltaEP_sub1, Double_t deltaEP_sub2, Double_t deltaEP_sub3)
	{
	    fdeltaEP = deltaEP;

	    // Sub-event method

	    fdeltaEP_sub1 = deltaEP_sub1;
	    fdeltaEP_sub2 = deltaEP_sub2;
            fdeltaEP_sub3 = deltaEP_sub3;
	}

	void SetEdep_full(Double_t edep_centralmod_nohole, Double_t edep_centralmod_hole, Double_t edep_1stCorona, Double_t edep_2ndCorona)
	{
	    fedep_centralmod_nohole = edep_centralmod_nohole;    
	    fedep_centralmod_hole = edep_centralmod_hole;
	    fedep_1stCorona = edep_1stCorona; 
	    fedep_2ndCorona = edep_2ndCorona;
	}	

        /** Getters **/

	Int_t GetEvtID() const { return fevtID; }
	Double_t GetEP() const { return fEP_RECO; }
        Double_t GetEP_sub1() const { return fEP_sub1; }
        Double_t GetEP_sub2() const { return fEP_sub2; }
        Double_t GetEP_sub3() const { return fEP_sub3; }

	Float_t GetE1() const { return fedep_centralmod_hole; }
	Float_t GetE2() const { return fedep_1stCorona; }
        Float_t GetE3() const { return fedep_2ndCorona; }

	void Print();

    private:


	/**   Data members  **/

        Int_t fevtID;         //SELIM
	
	Double_t fEP_RECO;    // SELIM: reconstructed event plane angle phi (full event)
	Double_t fdeltaEP;    // SELIM: diff MC phi - reco phi in proper range [-pi, pi] (for draw purpose)

	Double_t fQx;         // SELIM: Q-vector components
	Double_t fQy;         

	Float_t fedep_centralmod_nohole;    // SELIM: total energy deposition /event
        Float_t fedep_centralmod_hole;
	Float_t fedep_1stCorona;            
	Float_t fedep_2ndCorona;

	// Sub-event method

	Double_t fEP_sub1;
	Double_t fdeltaEP_sub1;
        Double_t fQx_sub1;
	Double_t fQy_sub1;

        Double_t fEP_sub2;
	Double_t fdeltaEP_sub2;
        Double_t fQx_sub2;
	Double_t fQy_sub2;

        Double_t fEP_sub3;
	Double_t fdeltaEP_sub3;
        Double_t fQx_sub3;
	Double_t fQy_sub3;

	ClassDef(CbmPsdEventData,2);

};


#endif
