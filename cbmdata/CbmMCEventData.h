/** CbmMCEventData.h
 **@author Selim <s.seddiki@gsi.de>
 **@since 06.2013
 **@version 1.0
 **
 ** Data container class to store MC info (b, phi_RP) in output of event plane reconstruction (seperately from reconstructed data)
 **/


#ifndef CBMMCEventData_H
#define CBMMCEventData_H 1


#include "CbmDetectorList.h"
#include "TNamed.h"
#include <vector>
#include "TH2F.h" 

class CbmMCEventData : public TNamed
{

    public:

	/**   Default constructor   **/
	CbmMCEventData() ;
	CbmMCEventData(const char *name) ;

	// CbmMCEventData(Float_t rp, Float_t b) ;


	/**   Destructor   **/
	virtual ~CbmMCEventData();



	/**   Setters - Getters   **/

        //Int_t GetEvtID() const { return fevtID; }   
	//void SetEvtID(Int_t id) {fevtID = id;}

	Double_t GetRP() const { return fRP; }   
	void SetRP(Double_t rp) {fRP = rp;}

	Double_t GetB() const { return fB; }
	void SetB(Double_t b) {  fB = b; }	

	void Print();

    private:


	/**   Data members  **/

        //Int_t fevtID;         
	Double_t fRP;      // MC reaction plane
	Double_t fB;       // MC impact parameter	

	ClassDef(CbmMCEventData,2);

};


#endif
