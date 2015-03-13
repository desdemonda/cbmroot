/** @file CbmStsElement.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 27.05.2013
 **/

#ifndef CBMSTSELEMENT_H
#define CBMSTSELEMENT_H 1


#include "TGeoPhysicalNode.h"
#include "TNamed.h"
#include "CbmStsAddress.h"



/** @class CbmStsElement
 ** @brief Class representing an element of the STS setup
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** A CbmStsElement represents an element in the STS setup
 ** hierarchy (e.g., station, ladder, module,...). It has
 ** a unique address, a level (enum CbmStsElementLevel),
 ** a pointer to a TGeoPhysicalNode and an array of daughter
 ** elements. It is thus an alignable object.
 **/
class CbmStsElement : public TNamed
{

  public:

    /** Default constructor **/
    CbmStsElement();


    /** Standard constructor
     ** @param name     Element name
     ** @param title    Volume name (type)
     ** @param level    Element level (enum)
     ** @param node     Pointer to physical node in geometry
     **/
    CbmStsElement(const char* name, const char* title,
                  Int_t level, TGeoPhysicalNode* node = NULL);


    /** Destructor **/
    virtual ~CbmStsElement() { };


    /** Add a daughter element
     ** The daughter must be one element level higher than the mother.
     ** @param element  Pointer to element to be added as daughter.
     **/
    virtual void AddDaughter(CbmStsElement* element);


    /** Get unique address
     ** @return Element address
     **/
    UInt_t GetAddress() const { return fAddress; }


    /** Get a daughter element
     ** @param index  Index of daughter element
     **/
    CbmStsElement* GetDaughter(Int_t index) const;


    /** Get the index within the mother element
     ** @return Index of element in mother
     **/
    Int_t GetIndex() const {
      return CbmStsAddress::GetElementId(fAddress, fLevel);
    }


    /** Get the element level
     ** @return Element level (type enum EStsElementLevel)
     **/
    EStsElementLevel GetLevel() const { return fLevel; }


    /** Get the mother element **/
    CbmStsElement* GetMother() const { return fMother; }


    /** Get number of daughter elements
     ** @return Number of daughters
     **/
    Int_t GetNofDaughters() const { return fDaughters.size(); }


    /** Get number of elements at given level
     ** @param level  Element level (see enum EStsElementLevel)
     ** @return Number of elements at given level with this
     **         element as ancestor
     */
    Int_t GetNofElements(Int_t level) const;


    TGeoPhysicalNode* GetPnode() { return fNode; }


    /** Initialise daughters from geometry **/
    void InitDaughters();


    /** Print **/
    virtual void Print(Option_t* opt = "") const;


  protected:

    UInt_t fAddress;                       ///< Unique address
    EStsElementLevel fLevel;               ///< Level in hierarchy
    TGeoPhysicalNode* fNode;               ///< Pointer to geometry
    vector<CbmStsElement*> fDaughters;     ///< Array of daughters
    CbmStsElement* fMother;                ///< Mother element


    /** Construct the name of the element **/
    void ConstructName();


    /** Set the element level from integer
     ** Protection against being out of range.
     ** @param  level  Element level
     **/
    void SetLevel(Int_t level);


    /** Set the mother element
     ** @param Pointer to mother element
     **/
    void SetMother(CbmStsElement* mother) { fMother = mother; }


private:
    // --- Prevent usage of copy constructor and assignment operator
    CbmStsElement(const CbmStsElement&);
    CbmStsElement& operator=(const CbmStsElement&);


    ClassDef(CbmStsElement,1);

};

#endif /* CBMSTSELEMENT_H */
