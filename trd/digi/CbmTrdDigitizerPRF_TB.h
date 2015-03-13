#ifndef CBMTRDDIGITIZERPRF_TB_H
#define CBMTRDDIGITIZERPRF_TB_H

#include "FairTask.h"

#include <map>
using std::map;
using std::pair;

class CbmTrdPoint;
class CbmTrdDigi;
class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdRadiator;
class CbmMatch;
class TClonesArray;

class CbmTrdDigitizerPRF_TB : public FairTask {
public:

	/**
	 * \brief Standard constructor.
	 **/
	CbmTrdDigitizerPRF_TB(CbmTrdRadiator* radiator);

	/**
	 * \brief Destructor.
	 **/
	virtual ~CbmTrdDigitizerPRF_TB();

	/**
	 * \brief Inherited from FairTask.
	 **/
	virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    **/
	virtual void SetParContainers();

   /**
    * \brief Inherited from FairTask.
    **/
	virtual void Exec(Option_t * option);

	void SetTriangularPads(Bool_t triangles);
	virtual void Finish();
private:

	CbmTrdDigitizerPRF_TB& operator=(const CbmTrdDigitizerPRF_TB&);
	CbmTrdDigitizerPRF_TB(const CbmTrdDigitizerPRF_TB&);

	Double_t CalcPRF(Double_t x, Double_t W, Double_t h);

	void ScanPadPlane(const Double_t* local_point, Double_t clusterELoss);

	void ScanPadPlaneTriangle(const Double_t* local_point, Double_t clusterELoss);

	Double_t TriangleIntegration(Bool_t even, Double_t displacement_x, Double_t W, Double_t displacement_y, Double_t H, Double_t h);

	Double_t TriangleIteration(Bool_t even, Int_t step, Double_t displacement_x, Double_t W, Double_t displacement_y, Double_t H, Double_t h);

	void SplitTrackPath(const CbmTrdPoint* point, Double_t ELoss);

	void AddDigi(Int_t pointId, Int_t address, Double_t charge, Double_t time);

	Bool_t fDebug;
	Bool_t fTrianglePads;

 	static const Double_t fDetectorDeadTime; //[ns] taken from GARFIELD for 3.5 x 3.5 x 5 mm at 100V/mm drift voltage
	Double_t fTime;
	Int_t fnRow;
	Int_t fnCol;
	Int_t fLayerId;
	Int_t fModuleId;
	Int_t fMCPointId;
	Int_t fnDigi;
	Int_t fnPoint;

	TClonesArray* fPoints; //! Trd MC points
	TClonesArray* fDigis; //! TRD digis
	TClonesArray* fDigiMatches; //! Corresponding MCPoints to TRD digis
	TClonesArray* fMCTracks;  //! MC Track information

	CbmTrdDigiPar* fDigiPar;    //!
	CbmTrdModule* fModuleInfo; //!
	CbmTrdRadiator* fRadiator;  //!

	std::map<Int_t, pair<CbmTrdDigi*, CbmMatch*> > fDigiMap; // Temporary storage for digis
	std::map<Int_t, std::vector<Int_t> > fTimeAddressMap; // [ns], [address]
	std::map<Int_t ,Double_t > fAddressTimeMap; //[address], [ns]

	ClassDef(CbmTrdDigitizerPRF_TB, 2);
};

#endif // CBMTRDDIGITIZER_PRF_H
