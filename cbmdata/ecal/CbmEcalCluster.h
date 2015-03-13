#ifndef CBMECALCLUSTER_H
#define CBMECALCLUSTER_H

#include "TObject.h"
#include "TArrayI.h"
#include "TArrayD.h"
#include <list>

#include "CbmEcalCell.h"

class CbmEcalRecParticle;
class CbmEcalMaximum;

struct CbmEcalClusterSortProcess : public std::binary_function<CbmEcalCell*, CbmEcalCell*, Bool_t>
{
  inline Bool_t operator()(const CbmEcalCell* left, const CbmEcalCell* right) const
  {
    if (left->GetTotalEnergy()<right->GetTotalEnergy())
      return kTRUE;
    return kFALSE;
  }
};

/** A temporary cluster needed for debugging of cluster finder procedure **/
class CbmEcalCluster : public TObject
{
friend class CbmEcalReco;
friend class CbmEcalReco2;
friend class CbmEcalRecoCorr;
friend class CbmEcalClusterFinder;
friend class CbmEcalClusterFinderV2;
public:
  /** An empty constructor **/
  CbmEcalCluster();
  /** A standard constructor **/ 
  CbmEcalCluster(Int_t num, const std::list<CbmEcalCell*>& cluster);
  /** A more advanced constructor. Should use this. **/
  CbmEcalCluster(Int_t num, const std::list<CbmEcalCell*>& cluster, const std::list<CbmEcalMaximum*>& maximums, const std::list<Double_t>& energy);
  /** Number of cluster in event **/
  inline Int_t Number() const {return fNum;}
  /** Size of cluster **/
  inline Int_t Size() const {return fSize;}
  /** Number of maximums in cluster **/
  inline Int_t Maxs() const {return fMaxs;}
  /** Energy of cluster **/
  inline Double_t Energy() const {return fEnergy;}
  /** Average type of cells in cluster **/
  inline Double_t Type() const {return fType;}
  /** Second moment **/
  inline Double_t Moment() const {return fMoment;}
  /** Moment over X axis **/
  inline Double_t MomentX() const {return fMomentX;}
  /** Moment over Y axis **/
  inline Double_t MomentY() const {return fMomentY;}
  /** Coordinates of cluster centre of gravity **/
  inline Double_t X() const {return fX;}
  inline Double_t Y() const {return fY;}
  /** \chi^2 of cluster after fitting **/
  inline Double_t Chi2() const {return fChi2;}

  /** Getters for cells and peaks **/
  inline Int_t CellNum(Int_t i) const {return fCellNums[i];}
  inline Int_t PeakNum(Int_t i) const {return fPeakNums[i];}
  inline Double_t PreEnergy(Int_t i) const {return fPreEnergy[i];}
  inline CbmEcalMaximum* Maximum(Int_t i) const {return fMaximums[i];}
  /** An virtual destructor **/
  virtual ~CbmEcalCluster();
private:
  /** An initialization **/
  void Init(std::list<CbmEcalCell*>& cls);
  /** Cluster number **/
  Int_t fNum;
  /** Cluster size in cells
   ** A separate variable. fCells not stored **/
  Int_t fSize;
  /** Number of maximums in cluster **/
  Int_t fMaxs;
  /** Energy of cluster **/
  Double_t fEnergy;
  /** Type is an average of type of cells in cluster **/
  Double_t fType;
  /** Second moment **/
  Double_t fMoment;
  /** Moment over X axis **/
  Double_t fMomentX;
  /** Moment over Y axis **/
  Double_t fMomentY;
  /** Coordinates of cluster centre of gravity **/
  Double_t fX;
  Double_t fY;
  /** \chi^2 after fitting **/
  Double_t fChi2;
  
  /** Serial numbers of cells in cluster **/
  TArrayI fCellNums;
  /** Serial numbers of peaks in cluster **/
  TArrayI fPeakNums;
  /** An energy deposition in peak areas  (preclusters)  **/
  TArrayD fPreEnergy;
  /** Serial numbers of maximums in system **/
  CbmEcalMaximum** fMaximums;		//!

  CbmEcalCluster(const CbmEcalCluster&);
  CbmEcalCluster& operator=(const CbmEcalCluster&);

  ClassDef(CbmEcalCluster, 1)
};

#endif
