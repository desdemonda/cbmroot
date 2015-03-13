/** CbmMuchTrackFinder.h
 *@author A.Lebedev <Andrey.Lebedev@gsi.de>
 *@since 2007
 **
 ** Abstract base class for concrete MUCH track finding algorithm.
 **/

#ifndef CBMMUCHTRACKFINDER_H_
#define CBMMUCHTRACKFINDER_H_ 1

#include "TObject.h"

class TClonesArray;

class CbmMuchTrackFinder : public TObject
{
public:
	/** Default constructor **/
	CbmMuchTrackFinder() {;}


	/** Destructor **/
	virtual ~CbmMuchTrackFinder() {;}


	/** Virtual method Init. If needed, to be implemented in the
	** concrete class. Else no action.
	**/
	virtual void Init() {;};


	/** Abstract method DoFind. To be implemented in the concrete class.
	** Task: Fill the track array, which is given as argument
	**/
	virtual Int_t DoFind(
		  TClonesArray* trackArray) = 0;

	ClassDef(CbmMuchTrackFinder, 1);
};

#endif
