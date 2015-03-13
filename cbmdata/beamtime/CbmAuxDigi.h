/** @file CbmAuxDigi.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 12.02.2014
 **/

#ifndef CBMAUXDIGI_H
#define CBMAUXDIGI_H

#include "CbmDigi.h"

/** @class CbmAuxDigi
 ** @brief Data class for data from AUX channels in ROCs
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **/
class CbmAuxDigi : public CbmDigi {

	public:

		/** Default constructor **/
		CbmAuxDigi();


		/** Standard constructor **/
		CbmAuxDigi(UShort_t rocId, UShort_t channel, ULong64_t time);


		/** Destructor **/
		virtual ~CbmAuxDigi() { };


		/** Get Address (dummy)
		 ** Abstract method from CbmDigi. Must be implemented in order to
		 ** treat AUX like digis, but does not have any meaning here.
		 **/
		virtual Int_t GetAddress() const { return -1; }


		/** Channel number **/
		UShort_t GetChannel() const { return fChannel; }


		/** ROC number **/
		UShort_t GetRocId() const { return fRocId; }


		/** System ID
		 ** Hardcoded to 999. Did not want to introduce a new item
		 ** in enum DetectorId.
		 **/
		virtual Int_t GetSystemId() const { return 999;}


		/** Time [ns]  **/
		virtual Double_t GetTime() const { return Double_t(fTime); }


	private:

		UShort_t  fRocId;     ///< ROC number
		UShort_t  fChannel;   ///< AUX channel number (0 - 3)
		ULong64_t fTime;      ///< Time [ns]

		ClassDef(CbmAuxDigi, 1);
};


#endif /* CBMAUXDIGI_H */
