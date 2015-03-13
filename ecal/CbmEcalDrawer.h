/* $Id: CbmEcalDrawer.h,v 1.3 2006/07/14 15:11:51 prokudin Exp $ */
/** CbmEcalDrawer 
 ** ECAL drawer. You can draw structure or energy deposition
 ** in cells. 
 **/
/*
 * $Log: CbmEcalDrawer.h,v $
 * Revision 1.3  2006/07/14 15:11:51  prokudin
 * Posibility to draw ECAL according to current palette added
 *
 * Revision 1.2  2006/07/13 16:41:00  prokudin
 * Cluster draw
 * */
#ifndef CBMECALDRAWER_H
#define CBMECALDRAWER_H

#include "CbmEcalStructure.h"

#include "TCanvas.h"
#include "TColor.h"
#include "TAttLine.h"
#include "TObjArray.h"

#include <list>

class CbmEcalDrawer: public TAttLine
{
public:
	/** Default constructor **/
 CbmEcalDrawer(CbmEcalStructure* ecalStructure=NULL)
   : TAttLine(), fEcalStructure(ecalStructure), fC(NULL), fCX(0), fCY(0),
    fCCX(0.), fCCY(0.), fMaxEnergy(0.), fFirstColor(), fSecondColor(),
    fEnergyType(), fLines(), fBoxes()
    {Init();};

	CbmEcalStructure* GetEcalStructure() const;
	void SetEcalStructure(CbmEcalStructure* ecalStructure);
	/** Returns canvas with picture **/
	TCanvas* GetCanvas() const;
        /** Draw only structure of the ECAL **/
	void DrawStructure();
	/** Draw structure and energy deposition in ECAL **/
	void DrawEcal(Int_t type=-2);

	/** Color used for 0 energy**/
	void SetFirstColor(TColor color);
	/** Color used for maximum energy**/
	void SetSecondColor(TColor color);

	TColor GetFirstColor() const;
	TColor GetSecondColor() const;

	/** Sets maximum energy. To init it from 
	 ** ECAL structure use
	 ** CbmEcalDrawer::InitMaximumEnergy() **/
	void SetMaximumEnergy(Double_t energy);
	Double_t GetMaximumEnergy() const;

	/** Inits MaxEnergy from ECAL structure. **/
	void InitMaximumEnergy();

	/** 0 --- for total energy (ECAL+PS)
	 ** 1 --- only ECAL energy
	 ** 2 --- only PS energy **/
	void SetEnergyType(Int_t energytype);
	Int_t GetEnergyType() const;
	virtual ~CbmEcalDrawer();
	/** Delete all lines and boxes after use**/
	void CleanUp();
	/** Draws module with all its cells
	 ** color==-1 to draw ECAL structure
	 ** color==-2 to draw energy deposition
	 ** color==-3 to draw everything with 0 energy
	 ** color==-4 to draw according to current palette **/
	void DrawModule(CbmEcalModule* module, Int_t color);

	/** Sets canvas size according to size of
	 ** cluster in arr **/
	void InitCanvas(TObjArray* arr);
	/** Draws cells from TObjArray
	 ** color==-2 to draw energy deposition
	 ** color==-3 to draw everything with 0 energy 
	 ** color==-4 to draw according to current palette **/
	void DrawCells(TObjArray* arr, Int_t color);
	/** Draws cell
	 ** color==-2 to draw energy deposition
	 ** color==-3 to draw everything with 0 energy 
	 ** color==-4 to draw according to current palette **/
	void DrawCell(CbmEcalCell* cell,Int_t color);
private:
	/** Calculates size of cavnas **/
	void Prepare();
	/** Creates white canvas **/
	void InitCanvas(Int_t x, Int_t y);
	/** Set default color, energy type and
	 ** maximum energy**/
	void Init();
	/** Gets energy from cell corresponding to
	 ** fEnergyType **/
	Double_t GetEnergy(CbmEcalCell* cell) const;

	CbmEcalStructure* fEcalStructure;
	/** Canvas to draw **/
	TCanvas* fC;
	/** Size of canvas **/
	Int_t fCX;
	Int_t fCY;
	/** Position of center of cluster **/
	Float_t fCCX;
	Float_t fCCY;
	Double_t fMaxEnergy;
	TColor fFirstColor;
	TColor fSecondColor;
	Int_t fEnergyType;

	std::list<void*> fLines;
	std::list<void*> fBoxes;

	CbmEcalDrawer(const CbmEcalDrawer&);
	CbmEcalDrawer& operator=(const CbmEcalDrawer&);

	ClassDef(CbmEcalDrawer,1);
};

inline void CbmEcalDrawer::SetEnergyType(Int_t energytype)
{
	if (energytype<0||energytype>2) return;
	fEnergyType=energytype;
}

inline Int_t CbmEcalDrawer::GetEnergyType() const
{
	return fEnergyType;
}

inline Double_t CbmEcalDrawer::GetEnergy(CbmEcalCell* cell) const
{
	if (fEnergyType==0) return cell->GetTotalEnergy();
	if (fEnergyType==1) return cell->GetEnergy();
	return 0;
}

inline TColor CbmEcalDrawer::GetFirstColor() const
{
	return fFirstColor;
}

inline TColor CbmEcalDrawer::GetSecondColor() const
{
	return fSecondColor;
}

/** First color used for 0 energy**/
inline void CbmEcalDrawer::SetFirstColor(TColor color)
{
	fFirstColor=color;
}

/** First color used for maximum energy**/
inline void CbmEcalDrawer::SetSecondColor(TColor color)
{
	fSecondColor=color;
}

inline Double_t CbmEcalDrawer::GetMaximumEnergy() const
{
	return fMaxEnergy;
}

inline void CbmEcalDrawer::SetMaximumEnergy(Double_t energy)
{
	fMaxEnergy=energy;
}

inline CbmEcalStructure* CbmEcalDrawer::GetEcalStructure() const
{
	return fEcalStructure;
}

inline void CbmEcalDrawer::SetEcalStructure(CbmEcalStructure* ecalStructure)
{
	fEcalStructure=ecalStructure;
}

inline TCanvas* CbmEcalDrawer::GetCanvas() const
{
	return fC;
}

#endif
