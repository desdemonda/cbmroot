/** @file CbmTofDigitizerBDF.h
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @date 19.07.2013
 **/

/** @class CbmTofDigitizerBDF
 ** @brief CBM ToF digitizer using beam data distributions as hit/cluster models
 ** @author Pierre-Alain Loizeau <loizeau@physi.uni-heidelberg.de>
 ** @version 1.0
 **/
#ifndef CBMTOFDIGITIZERBDF_H
#define CBMTOFDIGITIZERBDF_H  1

// TOF Classes and includes
   // Input/Output
class CbmTofPoint;
class CbmTofDigi;
class CbmTofDigiExp;
   // Geometry
class CbmTofGeoHandler;
class CbmTofDetectorId;
class CbmTofDigiPar;
class CbmTofDigiBdfPar;
class CbmTofCell;

// FAIR classes and includes
#include "FairTask.h"

// ROOT Classes and includes
class TClonesArray;
class TH1;
class TH2;
#include "TTimeStamp.h"
class TRandom3;

// C++ Classes and includes
#include <vector>

class TofChargeDistributions
{
   public:
      TofChargeDistributions(){};
      ~TofChargeDistributions(){};
      Double_t Gauss1D( Double_t *px, Double_t *par );
      Double_t Gauss2D( Double_t *px, Double_t *par );
};

class CbmTofDigitizerBDF : public FairTask
{
   public:

      /**
       ** @brief Constructor.
       **/
      CbmTofDigitizerBDF();

      /**
       ** @brief Constructor.
       **/
      CbmTofDigitizerBDF(const char *name, Int_t verbose = 1, Bool_t writeDataInOut = kTRUE);
      /**
       ** @brief Destructor.
       **/
      virtual ~CbmTofDigitizerBDF();

      /**
       ** @brief Inherited from FairTask.
       **/
      virtual InitStatus Init();

      /**
       ** @brief Inherited from FairTask.
       **/
      virtual void SetParContainers();

      /**
       ** @brief Inherited from FairTask.
       **/
      virtual void Exec(Option_t * option);

      /**
       ** @brief Inherited from FairTask.
       **/
      virtual void Finish();

      void SetInputFileName (TString FileName) { fsBeamInputFile = FileName; }
   protected:

   private:
      /**
       ** @brief Copy constructor.
       **/
      CbmTofDigitizerBDF(const CbmTofDigitizerBDF&);
      /**
       ** @brief Copy operator.
       **/
      CbmTofDigitizerBDF& operator=(const CbmTofDigitizerBDF&);

      // Functions common for all clusters approximations
      /**
       ** @brief Recover pointer on input TClonesArray: TofPoints, ...
       **/
      Bool_t   RegisterInputs();
      /**
       ** @brief Create and register output TClonesArray of Tof Digis.
       **/
      Bool_t   RegisterOutputs();
      /**
       ** @brief Initialize other parameters not included in parameter classes.
       **/
      Bool_t   InitParameters();
      /**
       ** @brief Load the beamtime values designed in the parameters: histograms or single values.
       ** @brief In case of histograms, compute the projection to [0;1].
       ** @brief This is done for each RPC type.
       **/
      Bool_t   LoadBeamtimeValues();

      // Histogramming functions
      Bool_t   CreateHistos();
      Bool_t   FillHistos();
      Bool_t   WriteHistos();
      Bool_t   DeleteHistos();

      // Functions for the merging of "gap digis" and "multiple hits digis" into "channel digis"
      /**
       ** @brief Merge the digis on he same readout channel.
       ** @brief Should take care of overlapping digis and of fee double-hit resolution
       **/
      Bool_t   MergeSameChanDigis();

      // Functions for the Cluster Radius generation
      /**
       ** @brief Generate a value for the cluster radius from the beamtime data corresponding
       ** @brief to this SmType/Rpc pair.
       **/
      Double_t GenerateClusterRadius( Int_t iSmType, Int_t iRpc );

      // Functions for a direct use of the cluster size
      /**
       ** @brief Convert TofPoints in input TClonesArray to Tof Digis using directly the
       ** @brief cluster size distribution from beamtime. The charge is encoded as ToT in ns and
       ** @brief shared among strips using a gauss distribution centered on the TofPoints.
       ** @brief Works only for strip RPC !!!!!
       ** @brief Generate higher cluster size than input as after digi merging a combination of strips
       ** @brief fired by each gap remains (e.g. size 1.5, 8 gaps can make 0 1 2l 2r 1 1 0 2l, ending in
       ** @brief size 3) => FIXME!
       ** @brief Returns the number of processed TofPoints
       **/
      Bool_t   DigitizeDirectClusterSize();

      // Functions for a simple "Flat disc" cluster approximation
      /**
       ** @brief Convert TofPoints in input TClonesArray to Tof Digis using an approximation of the
       ** @brief cluster charge distribution by a "Flat disc". The charge is encoded as ToT in ns.
       ** @brief Returns the number of processed TofPoints
       **/
      Bool_t   DigitizeFlatDisc();

      /**
       ** @brief Compute geometrical intersection area of a cluster and a channel
       **/
      Double_t ComputeClusterAreaOnChannel( Int_t iChanId, Double_t dClustRadius,
                                                Double_t dClustCentX, Double_t dClustCentY);

      // Functions for a 2D Gauss cluster approximation
      /**
       ** @brief Convert TofPoints in input TClonesArray to Tof Digis using an approximation of the
       ** @brief cluster charge distribution by a 2D symetric Gauss function, with the approx. that
       ** @brief sigmaX = sigmaY = cluster size/2 and the following limits:
       ** @brief d(ch. center, cluster) < 3*sigma AND Tot(Ch) > 0.2ns
       ** @brief Returns the number of processed TofPoints
       **/
      Bool_t   DigitizeGaussCharge();

      // Auxiliary functions
         // Area
      /**
       ** @brief Compute triangle area from its corners
       **/
      Double_t TriangleArea( Double_t dXa, Double_t dYa, Double_t dXb, Double_t dYb,
                               Double_t dXc, Double_t dYc );
      /**
       ** @brief Compute area of a disc section from the disc radius and the distance of the
       ** @brief section base to the disc center
       **/
      Double_t DiscSectionArea( Double_t dDiscRadius, Double_t dDistBaseToCenter);
         // Points
      /**
       ** @brief Compute the x position of the intersection of a circle with the upper or
       ** @brief lower edge of a channel. Assume 1 single intersection exists in edge range!
       ** @brief => Please check/insure before that it is the case!
       ** @brief Returns 0 if no intersection (neg. root) or 2 intersections
       **/
      Double_t CircleIntersectPosX( Int_t iChanId, Double_t dClustRadius,
                                       Double_t dClustCentX, Double_t dClustCentY,
                                       Bool_t bUpperSide );
      /**
       ** @brief Compute the y position of the intersection of a circle with the left or
       ** right edge of a channel. Assume single intersection in edge range!
       ** @brief => Please check/insure before that it is the case!
       ** @brief Returns 0 if no intersection (neg. root) or 2 intersections
       **/
      Double_t CircleIntersectPosY( Int_t iChanId, Double_t dClustRadius,
                                       Double_t dClustCentX, Double_t dClustCentY,
                                       Bool_t bRightSide );
      /**
       ** @brief Compute the distance from the cluster center to the base of a disc
       ** @brief section, described by its 2 endpoints.
       ** @brief Assumes that the endpoints are on the circle
       ** @brief => Please make sure of it before calling this method.
       **/
      Double_t DistanceCircleToBase( Double_t dClustRadius,
                                        Double_t dBaseXa, Double_t dBaseYa,
                                        Double_t dBaseXb, Double_t dBaseYb );

      // Fee properties and constants
      Double_t            fdFeeGainSigma;
      Double_t            fdFeeTotThr;
      Double_t            fdTimeResElec;
      Double_t            fdSignalPropSpeed;

      // Beamtime variables [nbSmType]
      std::vector< TH1* > fh1ClusterSizeProb;
      std::vector< TH1* > fh1ClusterTotProb;

      // Channel variables [nbSmType][NbSm*NbRpc][NbChannel*NbSides]
      std::vector< std::vector< std::vector< Double_t > > > fdChannelGain;   // <- Generated from parameter FeeGainSigma

      // ToF geometry variables
      CbmTofGeoHandler      * fGeoHandler;
      CbmTofDetectorId      * fTofId;
      CbmTofDigiPar         * fDigiPar;
      CbmTofCell            * fChannelInfo;
      CbmTofDigiBdfPar      * fDigiBdfPar;
      Int_t fiNbElecChTot;
      std::vector< std::vector< std::vector< Int_t > > > fvRpcChOffs;  // Offset in channel index for first channel of each RPC

      // Input variables
      TClonesArray          * fTofPointsColl; // TOF MC points
      TClonesArray          * fMcTracksColl;  // MC tracks

      // Intermediate storage variables
         // Store all digis to allow merging
      std::vector< std::vector< std::vector< std::vector< CbmTofDigi* > > > >
               fStorDigi; //[nbType][nbSm*nbRpc][nbCh*NbSide][nDigis]
      std::vector< std::vector< std::vector< std::vector< CbmTofDigiExp* > > > >
               fStorDigiExp; //[nbType][nbSm*nbRpc][nbCh*NbSide][nDigis]
      std::vector< std::vector< std::vector< std::vector< Int_t > > > >
               fStorDigiMatch; //[nbType][nbSm*nbRpc][nbCh*NbSide][nDigis]
         // Temporary storing of the Track/Point/Digi info to make sure all gap/Tofpoint
         // linked to same track in same channel give same result
      std::vector< std::vector < ULong64_t > > fvlTrckChAddr;   // [nbMcTracks][nbChannelFiredByTrk]

      // Random generators
      TRandom3 * fRandStart;
      TRandom3 * fRandEff;
      TRandom3 * fRandRadius;
      TRandom3 * fRandCharge;
      TRandom3 * fRandRes;

      // Output variables
      Bool_t                  fbWriteDigisInOut;
      TClonesArray          * fTofDigisColl;  // TOF Digis
      TClonesArray          * fTofDigiMatchPointsColl; // MC original points
      Int_t  fiNbDigis;                       // Index of the CbmTofDigi/CbmTofDigiExp TClonesArray

      // Generic
      Int_t fVerbose;

      // Histograms
      TH1 * fhTofPointsPerTrack;
      TH2 * fhTofPtsInTrkVsGapInd;
      TH2 * fhTofPtsInTrkVsGapIndPrm;
      TH2 * fhTofPtsInTrkVsGapIndSec;
      TH2 * fhTofPtsPosVsGap[10];
      /*
      TH2 * fhTofPointsPerTrackVsPdg;
      TH1 * fhMeanPointPerTrack;
      TH2 * fhMeanPointPerTrack2d;
      TH1 * fhMeanDigiPerPoint;
      TH1 * fhMeanFiredPerPoint;
      */
      TH1 * fhEvtProcTime;
      TH1 * fhDigiMergeTime;
      TH1 * fhDigiNbElecCh;
      TH2 * fhProcTimeEvtSize;
      TH1 * fhMeanDigiPerTrack;
      TH1 * fhMeanFiredPerTrack;
      TH1 * fhPtTime;
      TH1 * fhDigiTime;
      TH1 * fhDigiTimeRes;
      TH2 * fhDigiTimeResB;
      TH1 * fhToTDist;

      TH1 * fhElecChOccup;
      TH1 * fhMultiDigiEvtElCh;
      TH1 * fhFiredEvtElCh;
      TH1 * fhMultiProbElCh;

      TTimeStamp fStart;
      TTimeStamp fStop;
      Double_t   fdDigitizeTime;
      Double_t   fdMergeTime;

      TString fsBeamInputFile;

   ClassDef(CbmTofDigitizerBDF, 1);
};

#endif // CBMTOFDIGITIZERBDF_H
