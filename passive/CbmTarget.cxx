/** @file CbmTarget.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 08.10.2013
 **/

#include "CbmTarget.h"

#include "TGeoManager.h"
#include "TGeoMatrix.h"

#define RESET "\033[0m"
#define RED   "\033[1m\033[31m"
#define GREEN "\033[32m"


// -----   Default constructor   --------------------------------------------
CbmTarget::CbmTarget() : FairModule("Target", "CBM target"),
                         fZ(0),
                         fThickness(0.),
                         fDiameter(0.),
                         fDensity(0.),
                         fPosX(0.), fPosY(0.), fPosZ(0.),
                         fMaterial(""),
                         fBuildFromFile(kFALSE) {
}
// --------------------------------------------------------------------------



// -----   Constructor with file name   -------------------------------------
CbmTarget::CbmTarget(const char* fileName) :
                         FairModule("Target", "CBM target"),
                         fZ(0.),
                         fThickness(0.),
                         fDiameter(0.),
                         fDensity(0.),
                         fPosX(0.), fPosY(0.), fPosZ(0.),
                         fMaterial(""),
                         fBuildFromFile(kFALSE) {
 SetGeometryFileName(fileName);
}
// --------------------------------------------------------------------------



// -----   Constructor with properties   ------------------------------------
CbmTarget::CbmTarget(const char* element, Double_t thickness,
                     Double_t diameter, Double_t density) :
                     FairModule("Target", "CBM target"),
                     fZ(0),
                     fThickness(thickness),
                     fDiameter(diameter),
                     fDensity(density),
                     fPosX(0.), fPosY(0.), fPosZ(0.),
                     fMaterial(element),
                     fBuildFromFile(kFALSE) {
}
// --------------------------------------------------------------------------



// -----   Constructor with properties   ------------------------------------
CbmTarget::CbmTarget(Int_t z, Double_t thickness,
                     Double_t diameter, Double_t density) :
                     FairModule("Target", "CBM target"),
                     fZ(z),
                     fThickness(thickness),
                     fDiameter(diameter),
                     fDensity(density),
                     fPosX(0.), fPosY(0.), fPosZ(0.),
                     fMaterial(""),
                     fBuildFromFile(kFALSE) {
}
// --------------------------------------------------------------------------



// -----   Construct the geometry   -----------------------------------------
void CbmTarget::ConstructGeometry() {

  std::cout << std::endl;
  LOG(INFO) << GetName() << ": Constructing geometry..."
            << FairLogger::endl;


  // --- Construct from ROOT file if specified
  if ( fBuildFromFile ) {
    if ( ! fgeoName.EndsWith(".geo.root") ) {
      LOG(INFO)  << GetName() << ": geometry file is " << fgeoName
                 << FairLogger::endl;
      LOG(FATAL) << GetName() << ": only ROOT geometry files are supported!"
                 << FairLogger::endl;
      return;
    }
    LOG(DEBUG) << GetName() << ": constructing geometry from " << fgeoName
               << FairLogger::endl;
    ConstructRootGeometry();
    return;
  }


  // --- Get TGeoManager instance
  TGeoManager* geoMan = gGeoManager;
  if ( ! geoMan ) {
    LOG(FATAL) << GetName() << ": no TGeoManager instance!"
               << FairLogger::endl;
    return;
  }
  LOG(DEBUG2) << GetName() << ": Found TGeoManager " << geoMan->GetName() << FairLogger::endl;


  // --- Get target element
  TGeoElement* targElem = NULL;
  if ( fZ ) {
    targElem = geoMan->GetElementTable()->GetElement(fZ);
    if ( ! targElem ) {
      LOG(FATAL) << GetName() << ": unknown element " << fZ
                 << FairLogger::endl;
      return;
    }
    fMaterial = targElem->GetTitle();
  }
  else if ( ! fMaterial.IsNull() ) {
    targElem = geoMan->GetElementTable()->FindElement(fMaterial.Data());
    if ( ! targElem ) {
      LOG(FATAL) << GetName() << ": unknown element "
                 << fMaterial << FairLogger::endl;
    }
    fZ = targElem->Z();
  }
  else {
    LOG(FATAL) << GetName() << ": target material not specified!"
               << FairLogger::endl;
    return;
  }
  LOG(DEBUG) << GetName() << ": Use material " << fMaterial << ", z =  "
             << fZ << FairLogger::endl;


  // --- Get density, if not set through the constructor
  fDensity = fDensity > 0. ? fDensity : GetStandardDensity(fZ);
  if ( fDensity < 0. )
    LOG(FATAL) << GetName() << ": no standard density for element "
               << fMaterial << " available: density must be set explicitly."
               << FairLogger::endl;


  // --- Create target medium
  TGeoMaterial* targMat = new TGeoMaterial("targetMaterial",
                                           targElem, fDensity);
  if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG1) ) targMat->Print();
  // The medium ID (second argument) has no meaning for transport
  TGeoMedium* targMedium = new TGeoMedium("targetMedium", 9999, targMat);
  targMedium->SetParam(0, 1.);     // is active
  targMedium->SetParam(1, 1.);     // is in magnetic field
  targMedium->SetParam(2, 20.);    // max. field [kG]
  targMedium->SetParam(6, 0.001);  // boundary crossing precisison [cm]


  // Get mother node
  TGeoNode* motherNode = geoMan->FindNode(fPosX, fPosY, fPosZ);
  if ( ! motherNode ) {
    LOG(FATAL) << "No mother node found at target position!"
               << FairLogger::endl;
    return;
  }
  LOG(DEBUG) << GetName() << ": Mother node is " << motherNode->GetName()
             << FairLogger::endl;


  // Construct the transformation matrix for positioning of the target
  // in its mother volume. The matrix is the inverse of the global
  // transformation matrix of the mother node (thus assuring that the
  // target is correctly placed w.r.t. the global c.s.) plus a translation
  // in the global c.s. defined by the desired target position.
  TGeoHMatrix r1 = geoMan->GetCurrentMatrix()->Inverse();
  TGeoTranslation r2 = TGeoTranslation(fPosX, fPosY, fPosZ);
  TGeoHMatrix* targetMatrix = new TGeoHMatrix("targetToGlobal");
  *targetMatrix = r1 * r2;


  // Create target volume and add it as node to the mother volume
  TGeoVolume* target = geoMan->MakeTube("target", targMedium, 0.,
                                        fDiameter/2., fThickness/2.);
  motherNode->GetVolume()->AddNode(target, 0, targetMatrix);


  // Check the resulting transformation from target to global
  TGeoNode* testNode = geoMan->FindNode(fPosX, fPosY, fPosZ);
  LOG(DEBUG2) << GetName() << ": test node is " << testNode->GetName()
              << FairLogger::endl;
  TGeoHMatrix* testMatrix = geoMan->GetCurrentMatrix();
  if ( FairLogger::GetLogger()->IsLogNeeded(DEBUG1) ) testMatrix->Print();

  Print();
  std::cout << std::endl;
}
// --------------------------------------------------------------------------



// -----   Output to stdout   -----------------------------------------------
void CbmTarget::Print(Option_t* opt) {
  if ( fBuildFromFile ) LOG(INFO) << GetName() << ": geometry file "
                                  << fgeoName << FairLogger::endl;
  else {
  LOG(INFO) << GetName() << ": material " << fMaterial << ", density "
            << fDensity << " g/cm^3, thickness " << fThickness * 10000.
            << " mum, diameter " << fDiameter << " cm " << FairLogger::endl;
  LOG(INFO) << GetName() << ": position (" << fPosX << ", " << fPosY
            << ", " << fPosZ << ") cm" << FairLogger::endl;
  }
}
// --------------------------------------------------------------------------



// -----   Set a geometry file (overloaded from base class)   ---------------
void CbmTarget::SetGeometryFileName(TString name, TString version) {
  fBuildFromFile = kTRUE;
  LOG(INFO) << "Using target file name " << name << FairLogger::endl;
  return FairModule::SetGeometryFileName(name, version);
}
// --------------------------------------------------------------------------



// -----   Get the density at standard conditions   -------------------------
Double_t CbmTarget::GetStandardDensity(Int_t charge) {

  // TODO: Better implementation with array or the like

  switch (charge) {
    case  4: return  1.848; break;  // Beryllium
    case  6: return  2.260; break;  // Carbon
    case 49: return  7.310; break;  // Indium
    case 79: return 19.320; break;  // Gold
    case 82: return 11.342; break;  // Lead
    default: return -1.; break;     // Not found
  }

  return -1.;
}
// --------------------------------------------------------------------------


ClassImp(CbmTarget)



