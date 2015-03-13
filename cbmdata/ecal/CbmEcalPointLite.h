// CbmEcalPointLite --- stripped MCPoint class for ECAL 

#ifndef CBMECALPOINTLITE_H
#define CBMECALPOINTLITE_H

#include "FairMCPoint.h"

class CbmEcalPointLite : public FairMCPoint
{
public:
	//Default constuctor
	CbmEcalPointLite()
	  : FairMCPoint(-1111, -1111, TVector3(-1111, -1111, -1111), TVector3(-1111, -1111, -1111), -1111, -1111, -1111)
        {} ;

	CbmEcalPointLite(Int_t trackID, Int_t detID, Double32_t tof, Double32_t eloss, UInt_t EventId=0)
	  : FairMCPoint(trackID, detID, TVector3(-1111, -1111, -1111), TVector3(-1111, -1111, -1111), tof, -1111, eloss, EventId) 
	{} ;
//	  : fTrackID(trackID), fDetectorID(detID), fTime(tof), fELoss(eloss) {};
/*
	//Getters
	Int_t GetTrackID() const	{return fTrackID;}
	Int_t GetDetectorID() const	{return fDetectorID;}
	Double32_t GetTime() const	{return fTime;}
	Double32_t GetEnergyLoss() const{return fELoss;}

	//Setters
	void SetTrackID(Int_t trackID)		{fTrackID=trackID;}
	void SetDetectorID(Int_t detID)		{fDetectorID=detID;}
	void SetTime(Double32_t time)		{fTime=time;}
	void SetEnergyLoss(Double32_t eloss)	{fELoss=eloss;}

	//Need implementation
*/
	virtual void Print(const Option_t* opt=NULL) const;

	virtual ~CbmEcalPointLite() {};
private:
//	Int_t fTrackID;		//Index of track
//	Int_t fDetectorID;	//Number of volume in calorimeter system
//	Double32_t fTime;	//Time since interaction moment 
//	Double32_t fELoss;	//Energy deposition in cell
	ClassDef(CbmEcalPointLite,2)
};

#endif

