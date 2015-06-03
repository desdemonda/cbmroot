/** @file CbmStsElement.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 27.05.2013
 **/

#include "setup/CbmStsElement.h"

#include "TGeoManager.h"

#include "setup/CbmStsModule.h"
#include "setup/CbmStsSensor.h"
#include "setup/CbmStsSetup.h"
#include "setup/CbmStsStation.h"



// -----   Default constructor   -------------------------------------------
CbmStsElement::CbmStsElement() : TNamed(),
                                 fAddress(0),
                                 fLevel(kStsNofLevels),
                                 fNode(NULL),
                                 fDaughters(),
                                 fMother(NULL)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsElement::CbmStsElement(const char* name, const char* title,
                             Int_t level,
                             TGeoPhysicalNode* node) :
                             TNamed(name, title),
                             fAddress(0),
                             fLevel(kStsSystem),
                             fNode(node),
                             fDaughters(),
                             fMother(NULL)
{
  SetLevel(level);
}
// -------------------------------------------------------------------------



// -----   Add a daughter element   ----------------------------------------
void CbmStsElement::AddDaughter(CbmStsElement* element) {
  if ( ! element ) return;
  if ( element->GetLevel() != fLevel + 1) {
    LOG(FATAL) << fName << ": Trying to add an element of level "
               << element->GetLevel() << " as daughter of level "
               << fLevel << "! Command will be ignored."
               << FairLogger::endl;
    return;

  }
  element->fAddress = CbmStsAddress::SetElementId(fAddress,
                                                  element->GetLevel(),
                                                  GetNofDaughters() );
  element->SetMother(this);
  element->ConstructName();
  fDaughters.push_back(element);
}
// -------------------------------------------------------------------------



// ----- Construct the name of an element   --------------------------------
void CbmStsElement::ConstructName() {

	// Set the name for the STS system
	if ( GetLevel() == kStsSystem ) {
		SetName("STS");
		return;
	}

	// Special case halfladder ("U"p or "D"own)
	if ( GetLevel() == kStsHalfLadder ) {
		TString label;
		switch ( CbmStsAddress::GetElementId(fAddress, kStsHalfLadder) ) {
			case 0: label = "U"; break;
			case 1: label = "D"; break;
			default: break;
		}
		SetName( fMother->GetName() + label );
		return;
	}

	// For other levels: Expand the name of the mother
	TString label;
	switch ( GetLevel() ) {
		case kStsStation: label = "_S"; break;
		case kStsLadder: label = "_L"; break;
		case kStsModule: label = "_M"; break;
		case kStsSensor: label = "_S"; break;
		default: break;
	}
	label += Form("%02i",
			          CbmStsAddress::GetElementId(fAddress, GetLevel()) + 1 );
	SetName( fMother->GetName() + label );

}
// -------------------------------------------------------------------------



// -----   Get a daughter element   ----------------------------------------
CbmStsElement* CbmStsElement::GetDaughter(Int_t index) const {
  if ( index < 0 || index >=GetNofDaughters() ) return NULL;
  return fDaughters[index];
}
// -------------------------------------------------------------------------



// -----   Get number of elements at lower hierarchy levels   --------------
Int_t CbmStsElement::GetNofElements(Int_t level) const {

	Int_t nElements = 0;
	if ( level <= fLevel ) nElements = 0;
	else if ( level == fLevel + 1) nElements = GetNofDaughters();
	else
		for (Int_t iDaughter = 0; iDaughter < GetNofDaughters(); iDaughter++)
			nElements += GetDaughter(iDaughter)->GetNofElements(level);

	return nElements;
}
// -------------------------------------------------------------------------



// -----   Recursively read daughters from geometry   ----------------------
void CbmStsElement::InitDaughters() {

  // --- Catch absence of TGeoManager
  if ( ! gGeoManager ) {
    LOG(ERROR) << fName << ": cannot initialise without TGeoManager!"
               << FairLogger::endl;
    return;
  }

  // --- No daughter elements below sensor level
  if ( fLevel > kStsSensor ) return;

  // --- Catch physical node not being set
  if ( ! fNode ) {
    LOG(ERROR) << fName << ": physical node is not set!"
               << FairLogger::endl;
    return;
  }

  TGeoNode* mNode = fNode->GetNode();   // This node
  TString   mPath = fNode->GetName();   // Full path to this node
  Int_t nDaughters = 0;
  for (Int_t iNode = 0; iNode < mNode->GetNdaughters(); iNode++) {

    // Check name of daughter node for level name
    TString dName = mNode->GetDaughter(iNode)->GetName();
    if ( dName.Contains(CbmStsSetup::GetLevelName(fLevel + 1),
                        TString::kIgnoreCase ) ) {

      // Create physical node
      TString dPath = mPath + "/" + dName;
      TGeoPhysicalNode* pNode = new TGeoPhysicalNode(dPath.Data());

      // Create element and add it as daughter
      TString name = CbmStsSetup::GetLevelName(fLevel+1);
      name += Form("%02i", nDaughters++);

      const char* title = mNode->GetDaughter(iNode)->GetVolume()->GetName();
      CbmStsElement* dElement = NULL;
      switch ( fLevel) {
      	case kStsSystem:
      		dElement = new CbmStsStation(name, title, pNode); break;
      	case kStsHalfLadder:
      		dElement = new CbmStsModule(name, title, pNode); break;
      	case kStsModule:
      		dElement = new CbmStsSensor(name, title, pNode); break;
      	default:
      		dElement = new CbmStsElement(name, title, fLevel+1, pNode); break;
      }
      AddDaughter(dElement);

      // Call method recursively for the daughter elements
      dElement->InitDaughters();

    } // name of daughter node

  } // daughter node loop

}
// -------------------------------------------------------------------------



// -----   Print   ---------------------------------------------------------
void CbmStsElement::Print(Option_t* opt) const {
  LOG(INFO) << setw(10) << right << fAddress << "  "
		    << setw(12) << left << fName
		    << "  type " << setw(22) << fTitle << "  path "
		    << fNode->GetName() << "  " << fNode->GetTitle()
		    << FairLogger::endl;
  if ( opt[0] == 'R' ) {
	  for (Int_t iDaughter = 0; iDaughter < GetNofDaughters(); iDaughter++)
		  GetDaughter(iDaughter)->Print("R");
  }
}
// -------------------------------------------------------------------------



// -----   Set element level   ---------------------------------------------
void CbmStsElement::SetLevel(Int_t level) {
  switch (level) {
    case kStsSystem:     fLevel = kStsSystem;     break;
    case kStsStation:    fLevel = kStsStation;    break;
    case kStsLadder:     fLevel = kStsLadder;     break;
    case kStsHalfLadder: fLevel = kStsHalfLadder; break;
    case kStsModule:     fLevel = kStsModule;     break;
    case kStsSensor:     fLevel = kStsSensor;     break;
    default: LOG(ERROR) << fName << ": trying to set the level to "
                        << level << " while max. level is "
                        << kStsNofLevels - 1 << FairLogger::endl; break;
  }
}
// -------------------------------------------------------------------------


ClassImp(CbmStsElement)

