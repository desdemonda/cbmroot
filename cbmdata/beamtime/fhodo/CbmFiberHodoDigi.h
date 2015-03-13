/** CbmFiberHodoDigi.h
**@author F.Uhlig <f.uhlig@gsi.de>
**@since 11.12.13
**@version 1.0
**
** Data class for digital fibre hodoskop information
** This hodoskope is used during test beamtimes for
** generating a trigger and for beam position control 
** Data level: RAW
**
** The information is encoded into 8 bytes (1 Long64_t).
** Address:                       25 bits
**   - Station Nr.        4 bits
**   - Sector Nr.        10 bits
**   - Side (front/back)  1 bit
**   - Channel Nr.       10 bits
** Charge (ADC):                  12 bits
** Time (ns):                     14 bits
**
** The time is relative to the event time.   
**/
                    
#ifndef CBMFIBERHODODIGI_H
#define CBMFIBERHODODIGI_H 1

#include "CbmDigi.h"
#include "CbmFiberHodoAddress.h"

#include <string>
using std::string;

class CbmFiberHodoDigi : public CbmDigi
{
public:
   /**
    * \brief Default constructor.
    */
   CbmFiberHodoDigi();

   /**
    * \brief Constructor with assignment.
    * \param[in] address Unique channel address.
    * \param[in] charge Charge.
    * \param[in] time Absolute time [ns].
    */
   CbmFiberHodoDigi(Int_t address, Double_t charge, ULong_t time);

   /**
    * \brief Destructor.
    */
   ~CbmFiberHodoDigi();

   /**
    * \brief Inherited from CbmDigi.
    */
   Int_t GetAddress() const { return fAddress; };

   /**
    * \brief Inherited from CbmDigi.
    */
   Double_t GetCharge() const { return fCharge; }

   /**
    * \brief Inherited from CbmDigi.
    */
   Int_t GetSystemId() const { return CbmFiberHodoAddress::GetSystemId(fAddress); }

   /**
    * \brief Inherited from CbmDigi.
    */
   Double_t GetTime() const { return Double_t(fTime); }

   /** Accessors **/
   void SetAddress(Int_t address) { fAddress = address; }
   void SetCharge(Double_t charge) { fCharge = charge; }
   void SetTime(ULong_t time) { fTime = time; }

   /** Modifiers **/
   void AddCharge(Double_t charge) { fCharge += charge; }

   string ToString() const;
  
private:
   Int_t fAddress; ///< Unique channel address
   Double_t fCharge; ///< Charge
   ULong_t fTime; ///< Absolute time [ns]

   ClassDef(CbmFiberHodoDigi, 3);
};

#endif
