/** CbmMuchFindTracks.h
 *@author A.Lebedev <Andrey.Lebedev@gsi.de>
 *@since 2007
 **
 ** Task class for track finding in the MUCH.
 ** Input: TClonesArray of CbmMuchPixelHit
 ** Output: TClonesArray of CbmMuchTrack
 **
 ** Uses as track finding algorithm classes derived from CbmMuchTrackFinder.
 **/

#ifndef CBMMUCHFINDTRACKS_H_
#define CBMMUCHFINDTRACKS_H_ 1

#include "FairTask.h"

class CbmMuchTrackFinder;
class TClonesArray;

class CbmMuchFindTracks : public FairTask
{
public:
	/** Default constructor **/
	CbmMuchFindTracks();

	/** Standard constructor
	*@param name   Name of class
	*@param title  Task title
	*@param finder Pointer to STS track finder concrete class
	**/
	CbmMuchFindTracks(
			const char* name,
			const char* title = "FairTask",
			CbmMuchTrackFinder* finder = NULL);

	/** Destructor **/
	virtual ~CbmMuchFindTracks();

	/** Initialisation **/
	virtual InitStatus Init();

	/** Task execution **/
	virtual void Exec(
			Option_t* opt);

	/** Finish at the end **/
	virtual void Finish();

	/** SetParContainers **/
	virtual void SetParContainers();

	/** Accessors **/
	CbmMuchTrackFinder* GetFinder() { return fFinder; };
	Int_t GetNofTracks()           { return fNofTracks; };

	/** Set concrete track finder **/
	void UseFinder(CbmMuchTrackFinder* finder) { fFinder = finder; };

private:
	CbmMuchTrackFinder* fFinder;       // Pointer to TrackFinder concrete class
	TClonesArray* fTrackArray;         // Output array of CbmTrdTracks
	Int_t fNofTracks;                  // Number of tracks created

	CbmMuchFindTracks(const CbmMuchFindTracks&);
	CbmMuchFindTracks& operator=(const CbmMuchFindTracks&);

	ClassDef(CbmMuchFindTracks,1);
};

#endif
