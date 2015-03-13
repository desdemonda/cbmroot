/** @file CbmTarget.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 08.10.2013
 **/

#ifndef CBMTARGET_H
#define CBMTARGET_H 1


#include "FairModule.h"


/** @class CbmTarget
 ** @brief Class for constructing the geometry of the CBM target
 ** @author Volker Friese <v.friese@gsi.de>
 **
 ** The target of CBM can be created either on the fly, by specifying
 ** the target material (element), its thickness, its diameter and
 ** the density of the material. In this case, a single volume of shape
 ** TUBE will be positioned at the target position, as daughter of the
 ** node already present there. If no position is specified by the method
 ** SetPosition, the target is placed at the origin of the global coordinate
 ** system.
 **
 ** In case a geometry file is specified, the geometry will be constructed
 ** from this file in the same manner as for all other modules.
 **/
class CbmTarget : public FairModule {

  public:

    /** Default constructor **/
    CbmTarget();


    /** Constructor with file name
     ** @param fileName  Name of geometry file
     **
     ** This constructor will force the target geometry to be
     ** constructed from a geometry file (ROOT format only).
     **/
    CbmTarget(const char* fileName);


    /** Constructor with target properties
     ** @param element   Element name of target material
     **                  (Full name or element symbol, e.g. "Gold", "Au");
     ** @param thickness Thickness in z [cm]
     ** @param diameter  Diameter [cm]. Default is 0.25 cm.
     ** @param density   Density [g/cm^3]. If negative (default), density under standard
     **                  conditions will be taken, if available.
     **
     ** By using this constructor, the target geometry will be constructed
     ** as a tube with material and parameters specified as arguments.
     **/
    CbmTarget(const char* element, Double_t thickness,
              Double_t diameter = 0.25, Double_t density = -1.);


    /** Constructor with target properties
     ** @param z         Atomic charge of target material element
     ** @param thickness Thickness in z [cm]
     ** @param diameter  Diameter [cm]. Default is 0.25 cm.
     ** @param density   Density [g/cm^3]. If negative, density under standard
     **                  conditions will be taken.
     **
     ** By using this constructor, the target geometry will be constructed
     ** as a tube with material and parameters specified as arguments.
     **/
    CbmTarget(Int_t z, Double_t thickness,
              Double_t diameter = 0.25, Double_t density = -1.);


    /** Destructor **/
    virtual ~CbmTarget() { };


    /** Built the geometry **/
    virtual void ConstructGeometry();


    /** Output to stdout **/
    virtual void Print(Option_t* opt = "");


    /** Set the geometry file name
     ** @param name    name of geometry file
     ** @param geoVer  not used
     **
     ** If a geometry file is set by this method, the target geometry
     ** will be built from this file; the parameters will have no effect.
     **/
    virtual void SetGeometryFileName(TString name, TString geoVer = "0");


    /** Set the position of the target w.r.t. the global coordinate system.
     ** @param posX  target centre position in x [cm]
     ** @param posY  target centre position in y [cm]
     ** @param posZ  target centre position in z [cm]
     **/
    void SetPosition(Double_t x, Double_t y, Double_t z) {
      fPosX = x;
      fPosY = y;
      fPosZ = z;
    }


  private:

    Int_t    fZ;              ///< Atomic charge of target material
    Double_t fThickness;      ///< Thickness [cm]
    Double_t fDiameter;       ///< Diameter [cm]
    Double_t fDensity;        ///< Density of target material [g/cm^3]
    Double_t fPosX;           ///< Target centre position in x [cm]
    Double_t fPosY;           ///< Target centre position in y [cm]
    Double_t fPosZ;           ///< Target centre position in z [cm]
    TString  fMaterial;       ///< Material name
    Bool_t   fBuildFromFile;  ///< Flag for building from geometry file

    /** Get the standard density (at standard conditions)
     ** @param  charge  atomic charge of material
     ** @return density [g/cm^3]
     **/
    Double_t GetStandardDensity(Int_t charge);


    ClassDef(CbmTarget,2)

};

#endif //CBMTARGET_H

