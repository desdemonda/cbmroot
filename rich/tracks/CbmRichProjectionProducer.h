/**
* \file CbmRichProjectionProducer.h
*
* \brief Project track by straight line from imaginary plane
* to the mirror and reflect it to the photodetector plane.
*
* \author P.Stolpovsky
* \date 2005
**/

#ifndef CBM_RICH_PROJECTION_PRODUCER
#define CBM_RICH_PROJECTION_PRODUCER

#include "CbmRichProjectionProducerBase.h"
#include "CbmRichRecGeoPar.h"

class TClonesArray;
class CbmGeoRichPar;
class TObjArray;

/**
* \class CbmRichProjectionProducer
*
* \brief Project track by straight line from imaginary plane
* to the mirror and reflect it to the photodetector plane.
*
* \author P.Stolpovsky
* \date 2005
**/
class CbmRichProjectionProducer : public CbmRichProjectionProducerBase
{
public:

   /**
    * \brief Standard constructor.
    * \param[in] zflag Flag whether to use point in imaginary plane (zflag=1) or mirror point (zflag=2) for extrapolation.
    */
   CbmRichProjectionProducer(
         int zflag);

   /**
    * \brief Destructor.
    */
   virtual ~CbmRichProjectionProducer();

   /**
    * \brief Initialization of the task.
    */
   virtual void Init();

   /**
    * \brief Initialization  of Parameter Containers.
    */
   virtual void SetParContainers();

   /**
    * \brief Execute task.
    * \param[out] richProj Output array of created projections.
    */
   virtual void DoProjection(
         TClonesArray* richProj);

private:
   TClonesArray* fListRICHImPlanePoint; // Starting points&directions

   int fNHits; // Number of hits
   int fEvent; // number of events

   CbmRichRecGeoPar fGP;

  /**
   * \brief Copy constructor.
   */
  CbmRichProjectionProducer(const CbmRichProjectionProducer&);

  /**
   * \brief Assignment operator.
   */
  CbmRichProjectionProducer& operator=(const CbmRichProjectionProducer&);
};

#endif
