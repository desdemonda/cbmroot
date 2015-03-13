#ifndef CBMTRDHITPRODUCERCLUSTER_H
#define CBMTRDHITPRODUCERCLUSTER_H 

#include "FairTask.h"

class CbmTrdDigiPar;
class TClonesArray;

class CbmTrdHitProducerCluster : public FairTask
{
public:

   CbmTrdHitProducerCluster();

   virtual ~CbmTrdHitProducerCluster();

   virtual InitStatus Init();

   virtual void SetParContainers();

   virtual void Exec(Option_t * option);
   void SetTriangularPads(Bool_t triangles);
private:
   Bool_t fTrianglePads;
   Double_t CalcDisplacement(Double_t Q_left, Double_t Q_center, Double_t Q_right, Double_t padWidth);
   Double_t CalcY(Double_t padWidth, Double_t padHeight, Double_t rect_Dis, Double_t para_Dis, Int_t digiRow);
   void TriangularPadReconstruction(Int_t clusterId);
   void CenterOfGravity(
         Int_t clusterId);

   TClonesArray* fDigis; /** Input array of CbmTrdDigi **/
   TClonesArray* fClusters; /** Input array of CbmTrdCluster **/
   TClonesArray* fHits; /** Output array of CbmTrdHit **/

   CbmTrdDigiPar* fDigiPar;

   Int_t fRecoTriangular;

   CbmTrdHitProducerCluster(const CbmTrdHitProducerCluster&);
   CbmTrdHitProducerCluster& operator=(const CbmTrdHitProducerCluster&);

   ClassDef(CbmTrdHitProducerCluster,2);
};
#endif
