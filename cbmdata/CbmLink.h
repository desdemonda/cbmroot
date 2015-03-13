/**
 * \file CbmLink.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 *
 * Base data class for storing MC information link.
 **/

#ifndef CBMLINK_H_
#define CBMLINK_H_

#include "TObject.h"
#include <string>

using std::string;

class CbmLink : public TObject
{
public:
   /**
    * \brief Constructor.
    */
   CbmLink();

   /**
    * \brief Standard constructor.
    */
   CbmLink(Float_t weight, Int_t index, Int_t entry = -1, Int_t file = -1);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLink();

   /* Modifiers */
   Int_t GetFile() const { return fFile; }
   Int_t GetEntry() const { return fEntry; }
   Int_t GetIndex() const { return fIndex; }
   Float_t GetWeight() const { return fWeight; }

   /* Accessors */
   void SetFile(Int_t file) { fFile = file; }
   void SetEntry(Int_t entry) { fEntry = entry; }
   void SetIndex(Int_t index) { fIndex = index; }
   void SetWeight(Float_t weight) { fWeight = weight; }

   void AddWeight(Float_t weight) { fWeight += weight; }

   /**
    * \brief Return string representation of the object.
    * \return String representation of the object.
    **/
   virtual string ToString() const;

   friend Bool_t operator==(const CbmLink& lhs, const CbmLink& rhs) {
      return (lhs.GetFile() == rhs.GetFile() && lhs.GetEntry() == rhs.GetEntry() && lhs.GetIndex() == rhs.GetIndex());
   }

private:
   Int_t fFile; // File ID
   Int_t fEntry; // Entry number
   Int_t fIndex; // Index in array
   Float_t fWeight; // Weight

   ClassDef(CbmLink, 1)
};

#endif /* CBMLINK_H_ */
