#ifndef CBMRICHTRBPULSEROQA_H
#define CBMRICHTRBPULSEROQA_H


#include "FairTask.h"
#include <vector>

using namespace std;

class TClonesArray;
class TH1D;
class TH2D;


class CbmRichTrbPulserQa : public FairTask
{
public:
	/*
	 * Constructor.
	 */
	CbmRichTrbPulserQa();

	/*
	 * Destractor.
	 */
    virtual ~CbmRichTrbPulserQa();

    /**
     * \brief Inherited from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Inherited from FairTask.
     */
    virtual void Exec(
         Option_t* option);

    /**
     * \brief Inherited from FairTask.
     */
    virtual void Finish();

    /*
     * Histogram initialization.
     */
    void InitHist();

    /*
     * Draw histograms.
     */
    void DrawHist();


private:
    TClonesArray* fRichHits; // Array of RICH hits


    UInt_t fEventNum; // Event counter


    CbmRichTrbPulserQa(const CbmRichTrbPulserQa&);
    CbmRichTrbPulserQa operator=(const CbmRichTrbPulserQa&);

    ClassDef(CbmRichTrbPulserQa, 1);
};

#endif
