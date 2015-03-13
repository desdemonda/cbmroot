/** CbmStsEventData.h
 **@author Selim SEDDIKI <s.seddiki at gsi.de>
 **@since 23.10.2012
 **@version 1.0
 **
 ** Data container class to store reconstructed data in output of event plane reconstruction
 **/


#ifndef CBMSTSEventData_H
#define CBMSTSEventData_H 1


#include "CbmDetectorList.h"
#include "TNamed.h"
#include <vector>
#include "TH2F.h"

class CbmStsEventData : public TNamed
{

    public:

	/**   Default constructor   **/
	CbmStsEventData() ;
	CbmStsEventData(const char *name) ;

	// CbmStsEventData(Float_t rp, Float_t b) ;

	/**   Destructor   **/
	virtual ~CbmStsEventData();

	/**   Setters   **/
           
	void SetEvtID(Int_t id) {fevtID = id;}		

	void SetQvect(Double_t Qx1_1, Double_t Qy1_1, Double_t Qx1_2, Double_t Qy1_2, Double_t Qx1_full, Double_t Qy1_full, Double_t Qx2_1, Double_t Qy2_1, Double_t Qx2_2, Double_t Qy2_2, Double_t Qx2_full, Double_t Qy2_full)  
	{
	    fQx_harmo1_sub1 = Qx1_1;
	    fQy_harmo1_sub1 = Qy1_1;

            fQx_harmo1_sub2 = Qx1_2;
	    fQy_harmo1_sub2 = Qy1_2;

	    fQx_harmo1_full = Qx1_full;
	    fQy_harmo1_full = Qy1_full;
	    
	    fQx_harmo2_sub1 = Qx2_1;
	    fQy_harmo2_sub1 = Qy2_1;

            fQx_harmo2_sub2 = Qx2_2;
	    fQy_harmo2_sub2 = Qy2_2;

	    fQx_harmo2_full = Qx2_full;
	    fQy_harmo2_full = Qy2_full;
	}
        
	void SetEP(Double_t ep1_1, Double_t ep1_2, Double_t ep1_full, Double_t ep2_1, Double_t ep2_2, Double_t ep2_full)
	{
	    fEP_RECO_harmo1_sub1 = ep1_1;
	    fEP_RECO_harmo1_sub2 = ep1_2;
            fEP_RECO_harmo1_full = ep1_full;
	    
	    fEP_RECO_harmo2_sub1 = ep2_1;
	    fEP_RECO_harmo2_sub2 = ep2_2;
            fEP_RECO_harmo2_full = ep2_full;
	}	

	void SetdeltaEP(Double_t deltaEP1_1, Double_t deltaEP1_2, Double_t deltaEP1_full, Double_t deltaEP2_1, Double_t deltaEP2_2, Double_t deltaEP2_full)
	{
	    fdeltaEP_harmo1_sub1 = deltaEP1_1;
            fdeltaEP_harmo1_sub2 = deltaEP1_2;
	    fdeltaEP_harmo1_full = deltaEP1_full;
	    
	    fdeltaEP_harmo2_sub1 = deltaEP2_1;
            fdeltaEP_harmo2_sub2 = deltaEP2_2;
	    fdeltaEP_harmo2_full = deltaEP2_full;
	} 
	
	void SetMult(Int_t mult)   
	{
	    fmult = mult;;
	}
	
	void Setv1v2(Double_t v1, Double_t v2)
	{
	    fv1 = v1;
	    fv2 = v2;
	}

	/**   Getters   **/

	Int_t GetEvtID() const { return fevtID; }
	void GetEP(Double_t ep1_full, Double_t ep2_full)
	{
	    ep1_full = fEP_RECO_harmo1_full;
	    ep2_full = fEP_RECO_harmo2_full; 
	}

        Int_t GetMult() const { return fmult; }

	void Print();

    private:


	/**   Data members  **/

        Int_t fevtID;	

	// === first order event plane
	
	// sub-event 1
	Double_t fEP_RECO_harmo1_sub1;    // SELIM: reconstructed event plane angle phi
	Double_t fdeltaEP_harmo1_sub1;    // SELIM: diff MC phi - reco phi in proper range [-pi, pi] (for draw purpose)
	Double_t fQx_harmo1_sub1;         // SELIM: Q-vector components
	Double_t fQy_harmo1_sub1;         

	// sub-event 2
        Double_t fEP_RECO_harmo1_sub2;    // SELIM: reconstructed event plane phi
	Double_t fdeltaEP_harmo1_sub2;    // SELIM: diff MC phi - reco phi in proper range [-pi, pi] (for draw purpose)
	Double_t fQx_harmo1_sub2;         // SELIM: Q-vector components
	Double_t fQy_harmo1_sub2;         
 
	// full event
        Double_t fEP_RECO_harmo1_full;    // SELIM: reconstructed event plane phi
	Double_t fdeltaEP_harmo1_full;    // SELIM: diff MC phi - reco phi in proper range [-pi, pi] (for draw purpose)
	Double_t fQx_harmo1_full;         // SELIM: Q-vector components
	Double_t fQy_harmo1_full;         

	// === second order event plane
	
	// sub-event 1
	Double_t fEP_RECO_harmo2_sub1;    // SELIM: reconstructed event plane phi
	Double_t fdeltaEP_harmo2_sub1;    // SELIM: diff MC phi - reco phi in proper range [-pi, pi] (for draw purpose)
	Double_t fQx_harmo2_sub1;         // SELIM: Q-vector components
	Double_t fQy_harmo2_sub1;         

	// sub-event 2
        Double_t fEP_RECO_harmo2_sub2;    // SELIM: reconstructed event plane phi
	Double_t fdeltaEP_harmo2_sub2;    // SELIM: diff MC phi - reco phi in proper range [-pi, pi] (for draw purpose)
	Double_t fQx_harmo2_sub2;         // SELIM: Q-vector components
	Double_t fQy_harmo2_sub2;         

	// full event
        Double_t fEP_RECO_harmo2_full;    // SELIM: reconstructed event plane phi
	Double_t fdeltaEP_harmo2_full;    // SELIM: diff MC phi - reco phi in proper range [-pi, pi] (for draw purpose)
	Double_t fQx_harmo2_full;         // SELIM: Q-vector components
	Double_t fQy_harmo2_full;         

	Int_t fmult;        // SELIM: number of tracks used in event plane analysis
	
	Double_t fv1, fv2;  // SELIM: flow of particles (w.r.t. true reaction plane) used in event plane analysis

	ClassDef(CbmStsEventData,2);

};


#endif
