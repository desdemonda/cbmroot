/**
 * \file CbmHit.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * Base class for hits used for tracking in CBM.
 * Derives from TObject.
 * Each hit has its unique identifier of type HitType,
 * which can be later use for safe type casting.
 *
 * Former name: CbmBaseHit (renamed 11 May 2015)
 **/
#ifndef CBMHIT_H_
#define CBMHIT_H_

enum HitType {
	kHIT,
	kPIXELHIT,
	kSTRIPHIT,
	kSTSHIT,
	kMVDHIT,
	kRICHHIT,
	kMUCHPIXELHIT,
	kMUCHSTRAWHIT,
	kTRDHIT,
	kTOFHIT,
	kECALHIT
};

#include "TObject.h"
#include <string>
using std::string;
class CbmMatch;

class CbmHit : public TObject
{
public:
	/**
	 * \brief Default constructor.
	 */
	CbmHit();

	/**
	 * \brief Destructor.
	 */
	virtual ~CbmHit();

	/* Accessors */
	HitType GetType() const { return fType; }
	Double_t GetZ() const { return fZ; }
	Double_t GetDz() const { return fDz; }
	Int_t GetRefId() const { return fRefId; }
	Int_t GetAddress() const { return fAddress; }
	CbmMatch* GetMatch() const { return fMatch; }

	/* Setters */
	void SetZ(Double_t z) { fZ = z; }
	void SetDz(Double_t dz) { fDz = dz; }
	void SetRefId(Int_t refId) { fRefId = refId; }
	void SetAddress(Int_t address) { fAddress = address; }
	 void SetMatch(CbmMatch* match) { fMatch = match; }

	/**
	 * Virtual function. Must be implemented in derived class.
	 * Should return plane identifier of the hit. Usually this is station or layer
	 * number of the detector. Can be calculated using unique detector identifier
	 * or can use additional class member from the derived class to store the plane identifier.
	 **/
	virtual Int_t GetPlaneId() const { return -1; }

	/**
	 * \brief Virtual function. Must be implemented in derived class.
	 * Has to return string representation of the object.
	 **/
	virtual string ToString() const { return "Has to be implemented in derrived class"; }

protected:
	/**
     * \brief Sets hit type.
     * \param type hit type
     **/
	void SetType(HitType type) { fType = type; }

        CbmHit(const CbmHit&);
        CbmHit& operator=(const CbmHit&);

private:
	HitType fType; ///< hit type
	Double_t fZ; ///< Z position of hit [cm]
	Double_t fDz; ///< Z position error [cm]
	Int_t fRefId; ///< some reference id (usually to cluster, digi or MC point)
	Int_t fAddress; ///< detector unique identifier
	CbmMatch* fMatch; ///< Monte-Carlo information

	ClassDef(CbmHit, 2);
};

#endif /* CBMHIT_H_ */
