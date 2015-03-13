/** @file CbmLmdDebug.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17.12.2014
 **/

#ifndef CBMLMDDEBUG_H
#define CBMLMDDEBUG_H 1

#include <map>
#include <utility>
#include "TObject.h"
#include "TString.h"
#include "FairLogger.h"
#include "roc/Iterator.h"
#include "roc/Message.h"


using std::map;
using std::pair;



/** @class CbmLmdDebug
 ** @brief Read and analyse message stream from lmd file
 ** @author Volker Friese <v.friese@gsi.de>
 ** @since 17.12.2014
 ** @version 1.0
 **
 ** This lightweight class reads messages from a lmd file and does some
 ** basic statistics and log output for debugging purposes.
 **/
class CbmLmdDebug: public TObject {

	public:

		CbmLmdDebug(const char* fileName = "");

		virtual ~CbmLmdDebug();

    /** Initialisation **/
    virtual Bool_t Init();

    /** Execution
     ** @param nMessages  Messages to be read. -1 means all in the file.
     **/
		virtual Int_t Run(Int_t nMessages = -1);


	private:

		TString fLmdFileName;           ///< Name of input lmd file
    roc::Iterator* fRocIter;        ///< ROC iterator
    roc::Message*  fCurrentMessage; //! Current ROC message

    /** Current epoch marker for each ROC **/
    std::map<Int_t, uint32_t> fCurrentEpoch;

    // Counters
    UInt_t fNofMsgType[8];   ///< Messages per type
    map<pair<Int_t, Int_t>, UInt_t> fNofHitNxy;  ///<  Hit messages per NXYTER
    map<Int_t, UInt_t> fNofSync;  ///< Sync messages per ROC

    // Last message info
    Int_t   fLastMsgType;
    Int_t   fLastMsgRocId;
    Int_t   fLastMsgNxyId;
    ULong_t fLastMsgTime;

    // Run start and end time
    ULong_t fTimeStart;
    ULong_t fTimeStop;

    Long64_t fMaxTimeDisorder; ///< Maximal time disordering of messages


    // Print statistics
    void PrintStats();

    // Store message info
    void SetLastMessage();

};

#endif /* CBMLMDDEBUG_H */
