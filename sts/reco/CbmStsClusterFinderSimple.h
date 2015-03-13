/** @file CbmClusterFinderSimple.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 16.06.2014
 **/


#ifndef CBMSTSCLUSTERFINDERSIMPLE_H
#define CBMSTSCLUSTERFINDERSIMPLE_H 1

#include <map>
#include "TObject.h"

class TClonesArray;
class CbmStsDigi;
class CbmStsModule;

using std::map;

/** @class CbmStsClusterFinderSimple
 ** @brief Engine for finding STS clusters
 ** @author Volker Friese <v.friese@gsi.de>
 ** @since 16.06.2014
 ** @version 1.0
 **
 ** The simple cluster finder connects neighbouring active channels
 ** (containing one digi) of a module into a cluster. It disregards the
 ** charge information in the digis for cluster finding. Similarly,
 ** inactive channels (strips) are not taken into account; they will lead
 ** to splitting one cluster into two.
 ** A created cluster is assigned the sum of charges of all contributing
 ** digis, the charge-weighted mean of the channel number (equivalent
 ** to the x coordinate on the read-out edge), and the charge-weighted
 ** mean square of the channel number, equivalent to the width of
 ** the cluster. The cluster time is the average of the times of the
 ** contributing digis.
 **/
class CbmStsClusterFinderSimple : public TObject {

	public:

		/** Constructor **/
		CbmStsClusterFinderSimple();

		/** Destructor **/
		virtual ~CbmStsClusterFinderSimple();

		/** Cluster finding engine **/
		Int_t FindClusters(CbmStsModule* module);


		/** Set the cluster array **/
		void SetOutputArray(TClonesArray* clusters) { fClusters = clusters;		}


	private:

		Int_t fIndex[2048];  ///< Array of digi indices in channels

		TClonesArray* fClusters;   ///< Output array of clusters

                CbmStsClusterFinderSimple(const CbmStsClusterFinderSimple&);
                CbmStsClusterFinderSimple operator=(const CbmStsClusterFinderSimple&);

		ClassDef(CbmStsClusterFinderSimple,1);
};

#endif /* CBMSTSCLUSTERFINDERSIMPLE_H */
