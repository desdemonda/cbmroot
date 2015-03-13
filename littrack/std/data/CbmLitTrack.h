/**
 * \file CbmLitTrack.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Base data class for track.
 **/

#ifndef CBMLITTRACK_H_
#define CBMLITTRACK_H_

#include "base/CbmLitTypes.h"
#include "base/CbmLitEnums.h"
#include "data/CbmLitTrackParam.h"
#include "data/CbmLitFitNode.h"
#include "data/CbmLitHit.h"

#include <string>
#include <vector>
#include <algorithm>
using std::string;
using std::vector;
using std::sort;

/**
 * \class CbmLitTrack
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Base data class for track.
 **/
class CbmLitTrack
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrack():
      fHits(),
      fParamFirst(),
      fParamLast(),
      fFitNodes(),
      fQuality(kLITGOOD),
      fChi2(0),
      fNDF(0),
      fPreviousTrackId(0),
      fLastStationId(0),
      fPDG(211),
      fNofMissingHits(0),
      fRefId(-1),
      fLength(-1.) { }

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrack() {}

   /* Getters */
   Int_t GetNofHits() const { return fHits.size(); }
   LitTrackQa GetQuality() const { return fQuality; }
   litfloat GetChi2() const { return fChi2; }
   Int_t GetNDF() const { return fNDF; }
   Int_t GetPreviousTrackId() const { return fPreviousTrackId; }
   Int_t GetPDG() const {return fPDG;};
   const CbmLitTrackParam* GetParamFirst() const { return &fParamFirst; }
   const CbmLitTrackParam* GetParamLast() const { return &fParamLast; }
   Int_t GetLastStationId() const { return fLastStationId; };
   const CbmLitHit* GetHit(Int_t index) const { return fHits[index];}
   const vector<const CbmLitHit*>& GetHits() const { return fHits;}
   const CbmLitFitNode* GetFitNode(Int_t index) const {return &fFitNodes[index];}
   const vector<CbmLitFitNode>& GetFitNodes() const {return fFitNodes;}
   Int_t GetNofMissingHits() const {return fNofMissingHits;}
   Int_t GetRefId() const {return fRefId;}
   litfloat GetLength() const {return fLength;}

   /* Setters */
   void SetQuality(LitTrackQa quality) { fQuality = quality; }
   void SetChi2(litfloat chi2) { fChi2 = chi2; }
   void SetNDF(Int_t ndf) { fNDF = ndf; }
   void SetPreviousTrackId(Int_t id) { fPreviousTrackId = id; }
   void SetPDG(Int_t pdg) { fPDG = pdg; }
   void SetParamFirst(const CbmLitTrackParam* par) { fParamFirst = *par; }
   void SetParamLast(const CbmLitTrackParam* par) { fParamLast = *par; }
   /* TODO temporarily needed for equal_range algorithm */
   void SetNofHits(Int_t nofHits) { fHits.resize(nofHits); }
   void SetLastStationId(Int_t lastPlaneId) { fLastStationId = lastPlaneId; }
   void SetFitNodes(const vector<CbmLitFitNode>& nodes) {fFitNodes = nodes;}
   void SetNofMissingHits(Int_t nofMissingHits) {fNofMissingHits = nofMissingHits;}
   void SetRefId(Int_t refId) {fRefId = refId;}
   void SetLength(litfloat length) {fLength = length;}

   /**
    * \brief Add hit to track. No additional memory is allocated for hit.
    */
   void AddHit(const CbmLitHit* hit) {
      fHits.push_back(hit);
   }

   /**
    * \brief Remove all hits from track. Do not delete memory.
    */
   void ClearHits() {
      fHits.clear();
   }

   /**
    * \brief Remove hit and corresponding fit node.
    */
   void RemoveHit(Int_t index) {
      fHits.erase(fHits.begin() + index);
      if (!fFitNodes.empty()) { fFitNodes.erase(fFitNodes.begin() + index); };
   }

   /**
    * \brief Sort hits by Z position.
    * \param[in] downstream If downstream is true than hits are sorted in downstream direction otherwise in upstream direction.
    */
   void SortHits(Bool_t downstream = true) {
      if (downstream) { sort(fHits.begin(), fHits.end(), CompareHitPtrZLess()); }
      else { sort(fHits.begin(), fHits.end(), CompareHitPtrZMore()); }
   }

   /**
    * \brief Return true if track parameters are correct.
    * \return True if track parameters are correct.
    */
   Bool_t CheckParams() const {
      std::vector<litfloat> covFirst(fParamFirst.GetCovMatrix());
      std::vector<litfloat> covLast(fParamLast.GetCovMatrix());
      for (Int_t i = 0; i < 15; i++) {
         if (std::abs(covFirst[i]) > 10000. ||
               std::abs(covLast[i]) > 10000.) { return false; }
      }
      if (GetNofHits() < 1) { return false; }
      return true;
   }

   /**
    * \brief Return string representation of class.
    * \return String representation of class.
    */
   string ToString() const {
      std::stringstream ss;
      ss << "Track: quality=" <<  fQuality << ", chi2=" << fChi2
         << ", ndf=" << fNDF << ", previousTrackId=" << fPreviousTrackId
         << ", lastStationId=" << fLastStationId << ", pdg=" << fPDG
         << ", nofHits=" << fHits.size() << ", nofFitNodes=" << fFitNodes.size() << std::endl;
      return ss.str();
   }

private:
   vector<const CbmLitHit*> fHits; // Array of hits
   CbmLitTrackParam fParamFirst; // First track parameter
   CbmLitTrackParam fParamLast; // Last track parameter
   vector<CbmLitFitNode> fFitNodes; // Array of fit nodes
   LitTrackQa fQuality; // Track quality
   litfloat fChi2; // Chi-square
   Int_t fNDF; // Number of degrees of freedom
   Int_t fPreviousTrackId; // Index of the previous track, i.e. STS
   Int_t fLastStationId; // Last detector station where track has a hit
   Int_t fPDG; // PDG code
   Int_t fNofMissingHits; // Number of missing hits
   Int_t fRefId; // Reference to MC
   litfloat fLength; // Track length
};

/**
 * \brief Comparator for STL sorting algorithms.
 */
class CompareTrackPtrChiSqOverNdfLess :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   Bool_t>
{
public:
   Bool_t operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return ( (track1->GetChi2() / track1->GetNDF()) < (track2->GetChi2() / track2->GetNDF()) );
   }
};


/**
 * \brief Comparator for STL sorting algorithms.
 */
class CompareTrackPtrPrevTrackIdLess :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   Bool_t>
{
public:
   Bool_t operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return track1->GetPreviousTrackId() < track2->GetPreviousTrackId();
   }
};


/**
 * \brief Comparator for STL sorting algorithms.
 */
class CompareTrackPtrNofHitsMore :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   Bool_t>
{
public:
   Bool_t operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return track1->GetNofHits() > track2->GetNofHits();
   }
};


/**
 * \brief Comparator for STL sorting algorithms.
 */
class CompareTrackPtrNofHitsLess :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   Bool_t>
{
public:
   Bool_t operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return track1->GetNofHits() < track2->GetNofHits();
   }
};


/**
 * \brief Comparator for STL sorting algorithms.
 */
class CompareTrackPtrLastStationIdMore :
   public std::binary_function<
   const CbmLitTrack*,
   const CbmLitTrack*,
   Bool_t>
{
public:
   Bool_t operator()(const CbmLitTrack* track1, const CbmLitTrack* track2) const {
      return track1->GetLastStationId() > track2->GetLastStationId();
   }
};

#endif /*CBMLITTRACK_H_*/
