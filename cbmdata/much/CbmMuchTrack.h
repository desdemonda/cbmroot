/**  CbmMuchTrack.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2007
 **
 ** Base class for local tracks in the MUCH detector.
 ** Derives from CbmTrack.
 **/

#ifndef CBMMUCHTRACK_H_
#define CBMMUCHTRACK_H_ 1

#include "CbmTrack.h"

class CbmMuchTrack : public CbmTrack
{
public:
	/** Default constructor **/
	CbmMuchTrack();

	/** Destructor **/
	virtual ~CbmMuchTrack();

	ClassDef(CbmMuchTrack, 3);
};

#endif
