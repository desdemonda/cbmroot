/** @file CbmStsModule.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 14.05.2013
 **/

#ifndef CBMSTSMODULE_H
#define CBMSTSMODULE_H 1


#include <map>
#include <set>
#include <vector>
#include "CbmStsCluster.h"
#include "CbmStsDigi.h"
#include "digitize/CbmStsSignal.h"
#include "setup/CbmStsElement.h"
#include "setup/CbmStsSensor.h"

class TClonesArray;

using std::map;


/** @class CbmStsModule
 ** @brief Class representing an instance of a readout unit in the CBM-STS.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** The StsModule is the read-out unit in the CBM STS. It consists of one
 ** sensor or two or more daisy-chained sensors (CbmStsSensor), the analogue
 ** cable and the read-out electronics.
 **
 ** The module receives and stores the analogue signals from the sensor(s)
 ** in a buffer. It takes care of interference of signals in one and the
 ** same channel (two signals arriving within a given dead time).
 ** The module digitises the analogue signals and sends them to the
 ** CbmDaq when appropriate.
 **/
class CbmStsModule : public CbmStsElement
{
  public:

    /** Default constructor **/
    CbmStsModule();


    /** Standard constructor
     ** @param name   Name
     ** @param title  Title
     ** @param node   Pointer to relevant TGeoPhysicalNode
     */
    CbmStsModule(const char* name, const char* title,
                 TGeoPhysicalNode* node = NULL);


    /** Destructor **/
    virtual ~CbmStsModule();


    /** Convert ADC channel to charge
     ** @param adcChannel  ADC channel number
     ** @return analog charge [e]
     **/
    Double_t AdcToCharge(UShort_t adcChannel);


    /** Add a cluster to its array **/
    void AddCluster(CbmStsCluster* cluster) { fClusters.push_back(cluster); }


    /** Add a digi to its array
     ** @param digi Pointer to digi object
     ** @param index Index of digi in input TClonesArray
     **/
    void AddDigi(CbmStsDigi* digi, Int_t index);


    /** Add an analogue signal to the buffer
     **
     ** @param channel        channel number
     ** @param time           time of signal [ns]
     ** @param charge         analogue charge [e]
     ** @param index          index of CbmStsPoint in TClonesArray
     ** @param entry          MC entry (event number)
     ** @param file           MC input file number
     **
     ** The signal will be added to the buffer. Interference with
     ** previous signals within the same channels is checked and the
     ** proper action is executed.
     **/
    void AddSignal(Int_t channel, Double_t time, Double_t charge,
    		           Int_t index = 0, Int_t entry = 0, Int_t file = 0);


    /** Get status of the analog buffer
     **
     ** @paramOut nofSignals Number of signals in buffer (active channels)
     ** @paramOut timeFirst  Time of first signal in buffer [ns]
     ** @paramOut timeLast   Time of last signal in buffer [ns]
     **/
    void BufferStatus(Int_t& nofSignals,
    		              Double_t& timeFirst, Double_t& timeLast);


    /** Convert charge to ADC channel.
     ** @param charge  analog charge [e]
     ** @return  ADC channel number
     **
     ** This must be the inverse of AdcToCharge
     **/
    Int_t ChargeToAdc(Double_t charge);


     /** Clear the cluster vector **/
     void ClearClusters() { fClusters.clear(); }


     /** Clear the digi map **/
     void ClearDigis() { fDigis.clear(); }


     /** Create a cluster in the output array
      ** @param clusterStart  First channel of cluster
      ** @param clusterEnd    Last channel of cluster
      ** @param clusterArray  Output TClonesArray
      **/
     void CreateCluster(Int_t clusterStart, Int_t clusterEnd,
    		                TClonesArray* clusterArray);


     /** Find hits from clusters
      ** @param hitArray  Array where hits shall be registered
      ** @return Number of created hits
      **/
     Int_t FindHits(TClonesArray* hitArray);


     /** Get a digi in a channel
      ** @param[in] channel Channel number
      ** @param[out] index  Index of digi in TClonesArray
      ** @return pointer to digi object. NULL if channel is inactive.
      **/
     CbmStsDigi* GetDigi(Int_t channel, Int_t& index);


     /** Number of electronic channels **/
     Int_t GetNofChannels() const { return fNofChannels; };


     /** Current number of clusters  **/
     Int_t GetNofClusters() const { return fClusters.size(); }


     /** Current number of digis **/
     Int_t GetNofDigis() const { return fDigis.size(); }


     /** Initialise the analog buffer
      ** The analog buffer contains a std::multiset for each channel, to be
      ** filled with CbmStsSignal objects. Without this method, a channel
      ** multiset would be instantiated at run time when the first signal
      ** for this channel arrives. Depending on the occupancy of this channel,
      ** this may happen only after several hundreds of events. Consequently,
      ** the memory consumption will increase for the first events until
      ** each channel was activated at least once. This beaviour mimicks a
      ** memory leak and makes it harder to detect a real one in other parts
      ** of the code. This is avoided by instantiating each channel multiset
      ** at initialisation time.
      **/
     void InitAnalogBuffer();


     /** Digitise signals in the analog buffer
      ** @param time  readout time [ns]
      ** @return Number of created digis
      **
      ** All signals with time less than the readout time minus a
      ** safety margin (taking into account dead time and time resolution)
      ** will be digitised and removed from the buffer.
      **/
     Int_t ProcessAnalogBuffer(Double_t readoutTime);


     /** Set the digitisation parameters
      ** @param dynRagne   Dynamic range [e]
      ** @param threshold  Threshold [e]
      ** @param nAdc       Number of ADC channels
      **/
     void SetParameters(Int_t nChannels, Double_t dynRange,
    		                Double_t threshold, Int_t nAdc,
    		                Double_t timeResolution = 0.,
    		                Double_t deadTime = 0.,
    		                Double_t noise = 0.) {
    	 fNofChannels    = nChannels;
    	 fDynRange       = dynRange;
    	 fThreshold      = threshold;
    	 fNofAdcChannels = nAdc;
    	 fTimeResolution = timeResolution;
    	 fDeadTime       = deadTime;
    	 fNoise          = noise;
    	 fIsSet = kTRUE;
    	 InitAnalogBuffer();
     }

     void Status() const;


  private:

    Double_t fNofChannels;     ///< Number of electronic channels
    Double_t fDynRange;        ///< dynamic range [e]
    Double_t fThreshold;       ///< threshold [e]
    Int_t    fNofAdcChannels;  ///< Number of ADC channels
    Double_t fTimeResolution;  ///< Time resolution (sigma) [ns]
    Double_t fDeadTime;        ///< Channel dead time [ns]
    Double_t fNoise;           ///< Equivalent noise charge (sigma) [e]
    Bool_t   fIsSet;           ///< Flag whether parameters are set

    /** Buffer for analog signals, key is channel number.
     ** Because signals do not, in general, arrive time-sorted,
     ** the buffer must hold a (multi)set of signals for each channel
     ** (multiset allowing for different signals at the same time).
     ** Sorting in the multiset is with the less operator of CbmStsSignal,
     ** which compares the time of the signals.
     **/
    map<Int_t, multiset<CbmStsSignal*> > fAnalogBuffer;

    /** Map from channel number to pair of (digi, digiIndex).
     ** Used for cluster finding.
     **/
    map<Int_t, pair<CbmStsDigi*, Int_t> > fDigis;

    /** Vector of clusters. Used for hit finding. **/
    vector<CbmStsCluster*> fClusters;


    /** Digitise an analog charge signal
     ** @param channel Channel number
     ** @param signal  Pointer to signal object
     **/
    void Digitize(Int_t channel, CbmStsSignal* signal);


    /** Prevent usage of copy constructor and assignment operator **/
    CbmStsModule(const CbmStsModule&);
    CbmStsModule& operator=(const CbmStsModule&);
    

    ClassDef(CbmStsModule,1);

};

#endif /* CBMSTSMODULE_H */
