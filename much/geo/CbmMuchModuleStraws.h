/** CbmMuchModuleStraws.h
 **/

#ifndef CBMMUCHMODULESTRAWS_H
#define CBMMUCHMODULESTRAWS_H 1

#include "CbmMuchGeoScheme.h"
#include "CbmMuchModule.h"

class CbmMuchModuleStraws : public CbmMuchModule
{

public:

    /** Default constructor **/
    CbmMuchModuleStraws();

    /** Standard constructor
     *@param iStation  Station index
     *@param iLayer    Layer index
     *@param iSide     Defines side of the layer (0 - Front, 1 - Back)
     *@param iModule   Module index
     *@param position  Location of the module center in global c.s. (all dimensions in [cm])
     *@param size      Size of the module (all dimensions in [cm])
     *@param cutRadius Radius of the cut (if any, otherwise = -1.) [cm]
     **/
    CbmMuchModuleStraws(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius);

    /** Destructor **/
    virtual ~CbmMuchModuleStraws();


    virtual Bool_t InitModule(){ return kTRUE;}
    virtual void DrawModule(Color_t color){}



ClassDef(CbmMuchModuleStraws,1);
};
#endif


