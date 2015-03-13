/* $Id: CbmEcalDrawer.cxx,v 1.3 2006/07/14 15:11:51 prokudin Exp $ */
/* 
 * $Log: CbmEcalDrawer.cxx,v $
 * Revision 1.3  2006/07/14 15:11:51  prokudin
 * Posibility to draw ECAL according to current palette added
 *
 * Revision 1.2  2006/07/13 16:41:00  prokudin
 * Cluster draw
 */
#include "CbmEcalDrawer.h"

#include "TBox.h"
#include "TLine.h"
#include "TStyle.h"
#include "TColor.h"

#include <iostream>

using namespace std;

void CbmEcalDrawer::Init()
{
	fEnergyType=0;
	fMaxEnergy=10;
	TColor c;
	c.SetRGB(1,1,1);
	fFirstColor=c;
	c.SetRGB(1,0,0);
	fSecondColor=c;
	fLines.clear();
	fBoxes.clear();
	SetLineColor(1);
}

void CbmEcalDrawer::InitMaximumEnergy()
{
	vector<CbmEcalModule*> stru;
	vector<CbmEcalModule*>::const_iterator p;
	vector<CbmEcalCell*> cellstr;
	vector<CbmEcalCell*>::const_iterator cp;
	Double_t energy;

	fMaxEnergy=0;

	fEcalStructure->GetStructure(stru);
	for(p=stru.begin();p!=stru.end();++p)
	if ((*p)!=NULL)
	{
		cellstr=(*p)->GetCells();
		for(cp=cellstr.begin();cp!=cellstr.end();++cp)
		{
			energy=GetEnergy(*cp);
			if (energy>fMaxEnergy) fMaxEnergy=energy;
		}

	}
}

CbmEcalDrawer::~CbmEcalDrawer()
{
	fC->Clear();
	delete fC;
	CleanUp();
}

void CbmEcalDrawer::CleanUp()
{
	TLine* line;
	TBox* box;

	list<void*>::const_iterator p;
	for(p=fLines.begin();p!=fLines.end();++p)
	{
		line=(TLine*)(*p);
		delete line;
	}
	for(p=fBoxes.begin();p!=fBoxes.end();++p)
	{
		box=(TBox*)(*p);
		delete box;
	}
	fBoxes.clear();
	fLines.clear();
}

/** Sets canvas size according to size of
 ** cluster in arr **/
void CbmEcalDrawer::InitCanvas(TObjArray* arr)
{
	Float_t x1=9999;
	Float_t x2=-9999;
	Float_t y1=9999;
	Float_t y2=-9999;
	CbmEcalCell* cell;

	if (arr->GetSize()<=0) return;
	for(Int_t i=0;i<arr->GetSize();i++)
	{
		cell=(CbmEcalCell*)arr->At(i);
		if (cell==NULL) continue;
		if (cell->GetX1()<x1) x1=cell->GetX1();
		if (cell->GetX2()>x2) x2=cell->GetX2();
		if (cell->GetY1()<y1) y1=cell->GetY1();
		if (cell->GetY2()>y2) y2=cell->GetY2();
	}
	InitCanvas((Int_t)(x2-x1)*2,(Int_t)(y2-y1)*2);
	fCX=(Int_t)(x2-x1);
	fCY=(Int_t)(y2-y1);
	fCCX=-((Float_t)(x1))/((Float_t)(x2-x1));
	fCCY=-((Float_t)(y1))/((Float_t)(y2-y1));
}

void CbmEcalDrawer::InitCanvas(Int_t x, Int_t y)
{
	if (fC==NULL)
		fC=new TCanvas("ECALCanv","ECAL canvas",x,y);
	else
	{
		fC->Clear();
		delete fC;
		fC=new TCanvas("ECALCanv","ECAL canvas",x,y);
	}
	fC->SetFillColor(0);
	fC->Clear();
}

void CbmEcalDrawer::DrawEcal(Int_t type)
{
	vector<CbmEcalModule*> stru;
	vector<CbmEcalModule*>::const_iterator p;

	Prepare();
	CleanUp();
	fCCX=0.5;
	fCCY=0.5;
	if (fEcalStructure==NULL) return;
	fEcalStructure->GetStructure(stru);
	for(p=stru.begin();p!=stru.end();++p)
		DrawModule(*p, type);
}

void CbmEcalDrawer::DrawStructure()
{
	vector<CbmEcalModule*> stru;
	vector<CbmEcalModule*>::const_iterator p;

	Prepare();
	CleanUp();
	fCCX=0.5;
	fCCY=0.5;
	if (fEcalStructure==NULL) return;
	fEcalStructure->GetStructure(stru);
	for(p=stru.begin();p!=stru.end();++p)
		DrawModule(*p, -1);
}

void CbmEcalDrawer::Prepare()
{
	fCX=(Int_t)fEcalStructure->GetEcalInf()->GetEcalSize(0);
	fCY=(Int_t)fEcalStructure->GetEcalInf()->GetEcalSize(1);
	InitCanvas(fCX*2,fCY*2);
}

/** Draws cells from TObjArray
 ** color==-2 to draw energy deposition
 ** color==-3 to draw everything with 0 energy **/
void CbmEcalDrawer::DrawCells(TObjArray* arr, Int_t color)
{
	for(Int_t i=0;i<arr->GetSize();i++)
		DrawCell((CbmEcalCell*)arr->At(i), color);
}

void CbmEcalDrawer::DrawModule(CbmEcalModule* module, Int_t color)
{
	Int_t lcolor;

	if (module==NULL) return;
	if (color==-1)
	{
		char type=module->GetType();
		if (type==1) lcolor=3;
		if (type==2) lcolor=4;
		if (type==4) lcolor=5;
	} else lcolor=color;

	vector<CbmEcalCell*> cells=module->GetCells();
	vector<CbmEcalCell*>::const_iterator p=cells.begin();
	for(;p!=cells.end();++p)
		DrawCell((*p), lcolor);

}
void CbmEcalDrawer::DrawCell(CbmEcalCell* cell, Int_t color)
{
	Int_t lcolor;
	TLine* line;
        TBox* rect;

	if (cell==NULL) return;

	Double_t x1=((Double_t)cell->GetX1()-fEcalStructure->GetEcalInf()->GetXPos())/fCX+fCCX;
	Double_t x2=((Double_t)cell->GetX2()-fEcalStructure->GetEcalInf()->GetXPos())/fCX+fCCX;
	Double_t y1=((Double_t)cell->GetY1()-fEcalStructure->GetEcalInf()->GetYPos())/fCY+fCCY;
	Double_t y2=((Double_t)cell->GetY2()-fEcalStructure->GetEcalInf()->GetYPos())/fCY+fCCY;

	if (color==-2)
	{
		TColor c;
		Double_t e=GetEnergy(cell);
/*		Float_t r=fFirstColor.GetRed()  *(1.0-e/fMaxEnergy)+fSecondColor.GetRed()  *e/fMaxEnergy;
		Float_t g=fFirstColor.GetGreen()*(1.0-e/fMaxEnergy)+fSecondColor.GetGreen()*e/fMaxEnergy;
		Float_t b=fFirstColor.GetBlue() *(1.0-e/fMaxEnergy)+fSecondColor.GetBlue() *e/fMaxEnergy;
		c.SetRGB(r,g,b);
		lcolor=c.GetColor(r,g,b);
*/
		Int_t t=(Int_t)(e/fMaxEnergy*9);
		if (t==9) t=1; else if (t!=0) t=20-t;
		lcolor=t;
//		if (e>fMaxEnergy*0.01) lcolor=1;
	} else
	if (color==-3)
		lcolor=fFirstColor.GetColor(fFirstColor.GetRed(), fFirstColor.GetGreen(), fFirstColor.GetBlue());
	else
	if (color==-4)
	{
		Double_t e=GetEnergy(cell);
		if (e==0)
			lcolor=fFirstColor.GetColor(fFirstColor.GetRed(), fFirstColor.GetGreen(), fFirstColor.GetBlue());
		else
		{
			Int_t ncolors=gStyle->GetNumberOfColors();
			lcolor=(Int_t)(((Float_t)ncolors*e/fMaxEnergy)+0.99);
			lcolor=gStyle->GetColorPalette(lcolor);
		}
	}
	else
		lcolor=color;
	if (lcolor>0)
	{
		rect=new TBox(x1,y1,x2,y2);
		rect->SetFillColor(lcolor);
		rect->SetFillStyle(kSolid);
		rect->Draw();
		fBoxes.push_back((void*)rect);
	}
	if (y1>0.001)
	{
		line=new TLine(x1,y1,x2,y1);
		line->SetLineColor(GetLineColor());
		line->SetLineStyle(GetLineStyle());
		line->SetLineWidth(GetLineWidth());
		line->Draw();
		fLines.push_back((void*)line);
	}

	if (x2<0.999)
	{
		line=new TLine(x2,y1,x2,y2);
		line->SetLineColor(GetLineColor());
		line->SetLineStyle(GetLineStyle());
		line->SetLineWidth(GetLineWidth());
		line->Draw();
		fLines.push_back((void*)line);
	}

	if (y2<0.999)
	{
		line=new TLine(x2,y2,x1,y2);
		line->SetLineColor(GetLineColor());
		line->SetLineStyle(GetLineStyle());
		line->SetLineWidth(GetLineWidth());
		line->Draw();
		fLines.push_back((void*)line);
	}

	if (x1>0.001)
	{
		line=new TLine(x1,y2,x1,y1);
		line->SetLineColor(GetLineColor());
		line->SetLineStyle(GetLineStyle());
		line->SetLineWidth(GetLineWidth());
		line->Draw();
		fLines.push_back((void*)line);
	}
}

ClassImp(CbmEcalDrawer);
