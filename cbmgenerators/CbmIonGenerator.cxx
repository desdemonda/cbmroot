// -------------------------------------------------------------------------
// -----                   CbmIonGenerator source file                 -----
// -----       Created  04 Jun 11 by A.Chernogorov / S.Belogurov       -----
// -----       Modified 09 Apr 12 by A.Chernogorov / S.Belogurov       -----
// -------------------------------------------------------------------------

#include "CbmIonGenerator.h"

#include "FairPrimaryGenerator.h"
#include "FairIon.h"
#include "FairRunSim.h"
#include "FairRunSim.h"
#include "FairIon.h"

#include "TMath.h"
#include "TRandom.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TObjArray.h"
#include "TParticle.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Initialisation of static variables   --------------------------
Int_t CbmIonGenerator::fgNIon = 0;
// ------------------------------------------------------------------------

CbmIonGenerator::CbmIonGenerator()
  : FairGenerator(),
    fMult(-1),
    fP(0.),
    fVz(0.),
    fIon(NULL),
    fcollimationFlag(0),
    fmeanX(0.),
    fsigmaX(0.),
    fx(),
    fmeanY(0.),
    fsigmaY(0.),
    fy(),
    fsigmaPxdivP(0.),
    fPxdivPa(),
    fsigmaPydivP(0.),
    fPydivPa()
{
}
// -----   Default constructor + vertex position  -------------------------
CbmIonGenerator::CbmIonGenerator(Int_t z,
				 Int_t a,
				 Int_t q,
				 Int_t mult,
				 Double_t p,
				 Double_t sigmaX,
				 Double_t sigmaY,
				 Double_t sigmaPxdivP,
				 Double_t sigmaPydivP,
				 Double_t meanX,
				 Double_t meanY,
				 Double_t vz) 
  : FairGenerator(),
    fMult(mult),
    fP(Double_t(a) * p),
    fVz(vz),
    fIon(NULL),
    fcollimationFlag(0),
    fmeanX(meanX),
    fsigmaX(sigmaX),
    fx(),
    fmeanY(meanY),
    fsigmaY(sigmaY),
    fy(),
    fsigmaPxdivP(sigmaPxdivP),
    fPxdivPa(),
    fsigmaPydivP(sigmaPydivP),
    fPydivPa()
{
	fgNIon++;
	
	char buffer[20];
	sprintf(buffer, "FairIon%d", fgNIon);
	fIon = new FairIon(buffer, z, a, q);
	FairRunSim* run = FairRunSim::Instance();
	if (!run)
	{
		cout << "-E- CbmIonGenerator: No FairRun instantised!" << endl;
		Fatal("CbmIonGenerator", "No FairRun instantised!");
	}
	
	run->AddNewIon(fIon);
}
//_________________________________________________________________________

// -----   Default constructor + vertex position + collimation  -----------
CbmIonGenerator::CbmIonGenerator(Int_t z,
				 Int_t a,
				 Int_t q,
				 Int_t mult,
				 Double_t p,
				 Double_t sigmaX,
				 Double_t sigmaY,
				 Double_t sigmaPxdivP,
				 Double_t sigmaPydivP,
				 Double_t meanX, Double_t meanY, Double_t vz,
				 Double_t x1, Double_t x2, Double_t x3, Double_t x4,
				 Double_t y1, Double_t y2, Double_t y3, Double_t y4,
				 Double_t PxdivP1, Double_t PxdivP2, Double_t PxdivP3, Double_t PxdivP4,
				 Double_t PydivP1, Double_t PydivP2, Double_t PydivP3, Double_t PydivP4) 
  : FairGenerator(),
    fMult(mult),
    fP(Double_t(a) * p),
    fVz(vz),
    fIon(NULL),
    fcollimationFlag(1),
    fmeanX(meanX),
    fsigmaX(sigmaX),
    fx(),
    fmeanY(meanY),
    fsigmaY(sigmaY),
    fy(),
    fsigmaPxdivP(sigmaPxdivP),
    fPxdivPa(),
    fsigmaPydivP(sigmaPydivP),
    fPydivPa()
{
	// x1 < x2 < 0 < x3 < x4
	if( (x1>x2 || x2>0 || x3>x4 || x3<0) || (y1>y2 || y2>0 || y3>y4 || y3<0) ||
	    (PxdivP1>PxdivP2 || PxdivP2>0 || PxdivP3>PxdivP4 || PxdivP3<0) ||
	    (PydivP1>PydivP2 || PydivP2>0 || PydivP3>PydivP4 || PydivP3<0) )
	{
		cout << "-E- CbmIonGenerator: Wrong trapezoid parameters!" << endl;
		Fatal("CbmIonGenerator", "Wrong trapezoid parameters! (x1 < x2 < 0 < x3 < x4)");
	}
	
	fx[0] = x1; fx[1] = x2; fx[2] = x3; fx[3] = x4;
	fy[0] = y1; fy[1] = y2; fy[2] = y3; fy[3] = y4;
	fPxdivPa[0] = PxdivP1; fPxdivPa[1] = PxdivP2; fPxdivPa[2] = PxdivP3; fPxdivPa[3] = PxdivP4;
	fPydivPa[0] = PydivP1; fPydivPa[1] = PydivP2; fPydivPa[2] = PydivP3; fPydivPa[3] = PydivP4;

	fgNIon++;

	char buffer[20];
	sprintf(buffer, "FairIon%d", fgNIon);
	fIon = new FairIon(buffer, z, a, q);
	FairRunSim* run = FairRunSim::Instance();
	if (!run)
	{
		cout << "-E- CbmIonGenerator: No FairRun instantised!" << endl;
		Fatal("CbmIonGenerator", "No FairRun instantised!");
	}
	
	run->AddNewIon(fIon);
}
//_________________________________________________________________________

// -----   Destructor   ---------------------------------------------------
CbmIonGenerator::~CbmIonGenerator()
{
 // if (fIon) delete fIon;
}
//_________________________________________________________________________

// -----   Public method SetExcitationEnergy   ----------------------------
//void CbmIonGenerator::SetExcitationEnergy(Double_t eExc) {
//  fIon->SetExcEnergy(eExc);
//}
//_________________________________________________________________________

// -----   Public method SetMass   ----------------------------------------
//void CbmIonGenerator::SetMass(Double_t mass) {
//  fIon->SetMass(mass);
//}
//_________________________________________________________________________

// -----   Public method ReadEvent   --------------------------------------
Bool_t CbmIonGenerator::ReadEvent(FairPrimaryGenerator* primGen)
{
	Double_t fPxdivP, fPydivP, fPx, fPy, fPz, fVx, fVy;

	TParticlePDG* thisPart = TDatabasePDG::Instance()->GetParticle(fIon->GetName());
	if (!thisPart)
	{
		cout << "-W- CbmIonGenerator: Ion " << fIon->GetName() << " not found in database!" << endl;
		return kFALSE;
	}

	int pdgType = thisPart->PdgCode();
//*
	cout << "-I- CbmIonGenerator: Generating " << fMult << " ions of type "
		<< fIon->GetName() << " (PDG code " << pdgType << ")" << endl;
	cout << "    Momentum " << fP << " GeV/c from spatial distribution center ("
	<< fmeanX << ", " << fmeanY << ", " << fVz << ") cm" << endl;
//*/
	for (Int_t i=0; i<fMult; i++)
	{
		fVx = gRandom->Gaus(fmeanX, fsigmaX);
		fVy = gRandom->Gaus(fmeanY, fsigmaY);

		fPxdivP = gRandom->Gaus(0., fsigmaPxdivP);
		fPydivP = gRandom->Gaus(0., fsigmaPydivP);

		if (fcollimationFlag == 1)
		{
			fVx = fmeanX + CalculateCollDistrValue(fsigmaX, fx);
			fVy = fmeanY + CalculateCollDistrValue(fsigmaY, fy);
			fPxdivP = CalculateCollDistrValue(fsigmaPxdivP, fPxdivPa);
			fPydivP = CalculateCollDistrValue(fsigmaPydivP, fPydivPa);
		}

		fPx = fPxdivP*fP;
		fPy = fPydivP*fP;
		fPz = TMath::Sqrt(fP*fP - fPx*fPx - fPy*fPy);
		/*
		cout << "-I- CbmIonGenerator: Generating " << i+1 << " ion of " << fMult
		     << " with momentum (" << fPx << ", " << fPy << ", " << fPz
		     << ") GeV/c from (" << fVx << ", " << fVy<< ", " << fVz << ") cm" << endl;
		//*/
//		cout<< fPx/fP <<" "<< fPy/fP <<" "<< fVx <<" "<< fVy << endl;

		primGen->AddTrack(pdgType, fPx, fPy, fPz, fVx, fVy, fVz);

	} // end for

	return kTRUE;

}

//_____________________________________________________________________________

// -----   Private method CalculateCollDistrValue   ---------------------------
Double_t CbmIonGenerator::CalculateCollDistrValue(Double_t fsigma, Double_t* fa)
{
	Double_t fgaus=0., funi=0., fF1=0., fF2=0.;
	// Infinite loop with only return (may happen so that we do not get out of here)
	while(1)
	{
		fgaus = gRandom->Gaus(0., fsigma);
		funi  = gRandom->Uniform(0., 1.);
		fF1 = (fgaus-fa[3]) / (fa[2]-fa[3]);	// a3 <a< a4
		fF2 = (fgaus-fa[0]) / (fa[1]-fa[0]);	// a1 <a< a2
		if ( fgaus>fa[3] || fgaus<fa[0] )
			continue;
		if ( (fgaus>=fa[1] && fgaus<=fa[2]) || (fgaus<fa[1] && funi<=fF2) || (fgaus>fa[2] && funi<=fF1) )
			return fgaus;
		else
			continue;
	}
}
//_____________________________________________________________________________

ClassImp(CbmIonGenerator)
