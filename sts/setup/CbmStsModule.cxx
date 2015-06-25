/** @file CbmStsModule.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 14.05.2013
 **/

#include "TClonesArray.h"
#include "TRandom.h"
#include "FairLogger.h"
#include "CbmMatch.h"
#include "CbmStsAddress.h"
#include "CbmStsCluster.h"
#include "CbmStsDigi.h"
#include "digitize/CbmStsDigitize.h"
#include "setup/CbmStsModule.h"
#include "setup/CbmStsSensorType.h"
#include "setup/CbmStsSetup.h"


// -----   Default constructor   -------------------------------------------
CbmStsModule::CbmStsModule() : CbmStsElement(),
                               fNofChannels(2048),
                               fDynRange(0.),
                               fThreshold(0.),
                               fNofAdcChannels(0),
                               fTimeResolution(0),
                               fDeadTime(0.),
                               fNoise(0.),
                               fIsSet(kFALSE),
                               fDeadChannels(),
                               fAnalogBuffer(),
                               fDigis(),
                               fClusters()
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsModule::CbmStsModule(const char* name, const char* title,
                           TGeoPhysicalNode* node) :
                           CbmStsElement(name, title, kStsModule, node),
                           fNofChannels(2048),
                           fDynRange(0.),
                           fThreshold(0.),
                           fNofAdcChannels(0),
                           fTimeResolution(0),
                           fDeadTime(0.),
                           fNoise(0.),
                           fIsSet(0),
                           fAnalogBuffer(),
                           fDeadChannels(),
                           fDigis(),
                           fClusters()
{
}
// -------------------------------------------------------------------------



// --- Destructor   --------------------------------------------------------
CbmStsModule::~CbmStsModule() {

	// --- Clean analog buffer
	map<Int_t, multiset<CbmStsSignal*> >::iterator chanIt;
	multiset<CbmStsSignal*>::iterator sigIt;
	for (chanIt = fAnalogBuffer.begin(); chanIt != fAnalogBuffer.end();
			 chanIt++) {
		for ( sigIt = (chanIt->second).begin(); sigIt != (chanIt->second).end();
				  sigIt++) {
			delete (*sigIt);
		}
	}

}
// -------------------------------------------------------------------------



// -----   Convert ADC channel to analog charge (channel mean)   -----------
Double_t CbmStsModule::AdcToCharge(UShort_t adcChannel) {
	return fThreshold + fDynRange / Double_t(fNofAdcChannels) *
			   ( Double_t(adcChannel) + 0.5 );
}
// -------------------------------------------------------------------------



// -----   Add a digi to the digi map   ------------------------------------
void CbmStsModule::AddDigi(CbmStsDigi* digi, Int_t index) {

	// --- Check whether digi really belongs to the module
	if ( CbmStsAddress::GetMotherAddress(digi->GetAddress(),
			                                 kStsModule ) != fAddress ) {
		LOG(ERROR) << GetName() << ": Module address is " << fAddress
				       << ", trying to add a digi with module address "
				       << CbmStsAddress::GetMotherAddress(digi->GetAddress(),
				       			                             kStsModule )
		           << FairLogger::endl;
		return;
	}

	// --- Get channel number
	Int_t channel = CbmStsAddress::GetElementId(digi->GetAddress(),
			                                        kStsChannel);
	if ( ! ( channel >= 0 && channel < fNofChannels ) ) {
		LOG(ERROR) << GetName() << ": Illegal channel number " << channel
				       << " (module has " << fNofChannels << " channels)."
				       << FairLogger::endl;
		return;
	}

	// --- Check channel for existing entry
	// --- In the event-by-event digitiser, it can happen that there
	// --- are two digis in one channel within the same event - if
	// --- their time difference is larger than the dead time.
	// --- Usually, the later digi corresponds to a spiralling delta
	// --- electron, the time of which may be as large as several
	// --- microseconds. Here, we neglect such digis. A proper treatment
	// --- must be done within a time-based cluster finding procedure.
	if ( fDigis.find(channel) != fDigis.end() ) return;

	// --- Add digi to map
	fDigis[channel] = pair<CbmStsDigi*, Int_t>(digi, index);
	return;
}
// -------------------------------------------------------------------------



// -----   Add a signal to the buffer   ------------------------------------
void CbmStsModule::AddSignal(Int_t channel, Double_t time,
                             Double_t charge, Int_t index, Int_t entry,
                             Int_t file) {

	// --- Debug
	LOG(DEBUG3) << GetName() << ": Receiving signal " << charge
			        << " in channel " << channel << " at time "
			        << time << " s" << FairLogger::endl;
	
	// --- Discard charge if the channel is dead
	if ( fDeadChannels.count(channel) ) return;

	// --- If the channel is not yet active: create a new set and insert
	// --- new signal into it.
	if ( fAnalogBuffer.find(channel) == fAnalogBuffer.end() ) {
		CbmStsSignal* signal = new CbmStsSignal(time, charge,
				                                    index, entry, file);
		fAnalogBuffer[channel].insert(signal);
		LOG(DEBUG4) << GetName() << ": Activating channel " << channel
				        << FairLogger::endl;
		return;
	}  //? Channel not yet active

	// --- The channel is active: there are already signals in.
	// --- Loop over all signals in the channels and compare their time.
	//TODO: Loop over all signals is not needed, since they are time-ordered.
	Bool_t isMerged = kFALSE;
	multiset<CbmStsSignal*>::iterator it;
	for (it = fAnalogBuffer[channel].begin();
			 it != fAnalogBuffer[channel].end(); it++) {

		// Time between new and old signal smaller than dead time: merge signals
		if ( TMath::Abs( (*it)->GetTime() - time ) < fDeadTime ) {

			// Current implementation of merging signals:
			// Add charges, keep first signal time
			// TODO: Check with STS electronics people on more realistic behaviour.
			LOG(DEBUG4) << GetName() << ": channel " << channel
					        << ", new signal at t = " << time
					        << " ns is merged with present signal at t = "
					        << (*it)->GetTime() << " ns" << FairLogger::endl;
			(*it)->SetTime( TMath::Min( (*it)->GetTime(), time) );
			(*it)->AddLink(charge, index, entry, file);
			isMerged = kTRUE;  // mark new signal as merged
			LOG(DEBUG4) << "    New signal: time " << (*it)->GetTime()
					        << ", charge " << (*it)->GetCharge()
					        << ", number of links " << (*it)->GetMatch().GetNofLinks()
					        << FairLogger::endl;
			break;  // Merging should be necessary only for one buffer signal

		} //? Time difference smaller than dead time

	} // Loop over signals in buffer for this channel

	// --- If signal was merged: no further action
	if ( isMerged ) return;

	// --- Arriving here, the signal did not interfere with existing ones.
	// --- So, it is added to the analog buffer.
	CbmStsSignal* signal = new CbmStsSignal(time, charge,
                                          index, entry, file);
	fAnalogBuffer[channel].insert(signal);
	LOG(DEBUG4) << GetName() << ": Adding signal at t = " << time
			        << " ns, charge " << charge << " in channel " << channel
			        << FairLogger::endl;

}
// -------------------------------------------------------------------------



// -----   Status of analog buffer   ---------------------------------------
void CbmStsModule::BufferStatus(Int_t& nofSignals,
		                            Double_t& timeFirst,
		                            Double_t& timeLast) {

	map<Int_t, multiset<CbmStsSignal*> >::iterator chanIt;
	multiset<CbmStsSignal*>::iterator sigIt;

	Int_t nSignals   = 0;
	Double_t tFirst  = -1.;
	Double_t tLast   = -1.;
	Double_t tSignal = -1.;

	// --- Loop over active channels
	for ( chanIt  = fAnalogBuffer.begin();
			  chanIt != fAnalogBuffer.end(); chanIt++) {

		// --- Loop over signals in channel
		for ( sigIt  = (chanIt->second).begin();
				  sigIt != (chanIt->second).end(); sigIt++) {

			tSignal = (*sigIt)->GetTime();
			nSignals++;
			tFirst = tFirst < 0. ? tSignal : TMath::Min(tFirst, tSignal);
			tLast  = TMath::Max(tLast, tSignal);
		} // signals in channel

	} // channels in module

	nofSignals = nSignals;
	timeFirst  = tFirst;
	timeLast   = tLast;

}
// -------------------------------------------------------------------------



// -----   Convert analog charge to ADC channel number   -------------------
Int_t CbmStsModule::ChargeToAdc(Double_t charge) {
	if ( charge < fThreshold ) return -1;
	Int_t adc = Int_t ( (charge - fThreshold) * Double_t(fNofAdcChannels)
			        / fDynRange );
	return ( adc < fNofAdcChannels ? adc : fNofChannels - 1 );
}
// -------------------------------------------------------------------------



// -----   Create a cluster in the output TClonesArray   -------------------
void CbmStsModule::CreateCluster(Int_t clusterStart, Int_t clusterEnd,
		                             TClonesArray* clusterArray) {

	// --- Create new cluster
	Int_t nClusters = clusterArray->GetEntriesFast();
	CbmStsCluster* cluster =
			new ( (*clusterArray)[nClusters] ) CbmStsCluster();

	// --- Calculate total charge, cluster position and spread
	Double_t sum1 = 0.;  // sum of charges
	Double_t sum2 = 0.;  // sum of channel * charge
	Double_t sum3 = 0.;  // sum of channel^2 * charge
	Double_t tsum = 0.;  // sum of digi times
	Int_t index = -1;
	for (Int_t channel = clusterStart; channel <= clusterEnd; channel++) {
		CbmStsDigi* digi = GetDigi(channel, index);
		if ( ! digi ) continue;
		cluster->AddDigi(index);
		Double_t charge = AdcToCharge(digi->GetCharge());
		sum1 += charge;
		sum2 += charge * Double_t(channel);
		sum3 += charge * Double_t(channel) * Double_t(channel);
		tsum += digi->GetTime();
	}
	if ( sum1 > 0. ) {
		sum2 /= sum1;
		sum3 /= sum1;
	}
	else {
		sum2 = -1.;
		sum3 = -1.;
	}
	// --- Cluster time is average of all digi time
	if ( cluster->GetNofDigis() ) tsum /= cluster->GetNofDigis();
	else tsum = 0.;
	cluster->SetProperties(sum1, sum2, sum3, tsum);
	cluster->SetAddress(fAddress);

	LOG(DEBUG2) << GetName() << ": Created new cluster from channel "
			        << clusterStart << " to " << clusterEnd << ", charge "
			        << sum1 << ", time " << tsum << "ns, channel mean "
			        << sum2 << FairLogger::endl;
}
// -------------------------------------------------------------------------



// -----   Digitise an analog charge signal   ------------------------------
void CbmStsModule::Digitize(Int_t channel, CbmStsSignal* signal) {

	// --- Add noise to the signal
	Double_t charge = signal->GetCharge();
	if ( fNoise > 0.)
		charge = signal->GetCharge() + gRandom->Gaus(0., fNoise);

	// --- No action if charge is below threshold
	if ( charge < fThreshold ) return;

	// --- Construct channel address from module address and channel number
	UInt_t address = CbmStsAddress::SetElementId(GetAddress(),
			                                         kStsChannel, channel);

	// --- Digitise charge
	// --- Prescription according to the information on the STS-XYTER
	// --- by C. Schmidt.
	UShort_t adc = 0;
	if ( charge > fDynRange ) adc = fNofAdcChannels - 1;
	else adc = UShort_t( (charge - fThreshold) / fDynRange
				     * Double_t(fNofAdcChannels) );

	// --- Digitise time
	// Note that the time is truncated at 0 to avoid negative times. This
	// will show up in event-by-event simulations, since the digi times
	// in this case are mostly below 1 ns.
	ULong64_t dTime
		= ULong64_t( TMath::Max(0., gRandom->Gaus(signal->GetTime(),
				                                      fTimeResolution)) );

	// --- Send the message to the digitiser task
	LOG(DEBUG4) << GetName() << ": charge " << signal->GetCharge()
			        << ", dyn. range " << fDynRange << ", threshold "
			        << fThreshold << ", # ADC channels "
			        << fNofAdcChannels << FairLogger::endl;
	LOG(DEBUG3) << GetName() << ": Sending message. Address " << address
			        << ", time " << dTime << ", adc " << adc << FairLogger::endl;
	CbmStsDigitize* digitiser = CbmStsSetup::Instance()->GetDigitizer();
	if ( digitiser ) digitiser->CreateDigi(address, dTime, adc,
			                                   signal->GetMatch());

	return;
}
// -------------------------------------------------------------------------



// -----   Find hits   -----------------------------------------------------
Int_t CbmStsModule::FindHits(TClonesArray* hitArray) {

	// --- Call FindHits method in each daughter sensor
	Int_t nHits = 0;
	for (Int_t iSensor = 0; iSensor < GetNofDaughters(); iSensor++) {
		CbmStsSensor* sensor = dynamic_cast<CbmStsSensor*>(GetDaughter(iSensor));
		nHits += sensor->FindHits(fClusters, hitArray);
	}

	LOG(DEBUG2) << GetName() << ": Clusters " << fClusters.size()
			        << ", sensors " << GetNofDaughters() << ", hits "
			        << nHits << FairLogger::endl;
	return nHits;
}
// -------------------------------------------------------------------------



// -----   Get a digi in a channel   ---------------------------------------
CbmStsDigi* CbmStsModule::GetDigi(Int_t channel, Int_t& index) {

	if ( ! ( channel >= 0 && channel < fNofChannels) ) {
		LOG(ERROR) << GetName() << ": Illegal channel number " << channel
				       << FairLogger::endl;
		return NULL;
	}

	map<Int_t, pair<CbmStsDigi*, Int_t> >::iterator it = fDigis.find(channel);
	if (it == fDigis.end() ) {
		index = -1;
		return NULL;  // inactive channel
	}

	index = (it->second).second;  // index of digi in this channel
	return (it->second).first;    // digi in this channel
}
// -------------------------------------------------------------------------



// -----  Initialise the analog buffer   -----------------------------------
void CbmStsModule::InitAnalogBuffer() {

	 for (Int_t channel = 0; channel < fNofChannels; channel++) {
		 multiset<CbmStsSignal*> mset;
		 fAnalogBuffer[channel] = mset;
	 } // channel loop

}
// -------------------------------------------------------------------------



// -----   Process the analog buffer   -------------------------------------
Int_t CbmStsModule::ProcessAnalogBuffer(Double_t readoutTime) {

	// --- Counter
	Int_t nDigis = 0;

	// --- Time limit up to which signals are digitised and sent to DAQ.
	// --- Up to that limit, it is guaranteed that future signals do not
	// --- interfere with the buffered ones. The readoutTime is the time
	// --- of the last processed StsPoint. All coming points will be later
	// --- in time. So, the time limit is defined by this time minus
	// --- 5 times the time resolution (maximal deviation of signal time
	// --- from StsPoint time) minus the dead time, within which
	// --- interference of signals can happen.
	Double_t timeLimit = readoutTime - 5. * fTimeResolution - fDeadTime;

	// --- Iterate over active channels
	map<Int_t, multiset<CbmStsSignal*> >::iterator chanIt;
	for (chanIt = fAnalogBuffer.begin();
			 chanIt != fAnalogBuffer.end(); chanIt++) {

		// --- Digitise all signals up to the specified time limit
		set<CbmStsSignal*>::iterator sigIt = (chanIt->second).begin();
		set<CbmStsSignal*>::iterator oldIt = sigIt;
		while ( sigIt != (chanIt->second).end() ) {

			// --- Exit loop if signal time is larger than time limit
			// --- N.b.: Readout time < 0 means digitise everything
			if ( readoutTime >= 0. && (*sigIt)->GetTime() > timeLimit ) break;

			// --- Digitise signal
			Digitize( chanIt->first, (*sigIt) );
			nDigis++;

			// --- Increment iterator before it becomes invalid
			oldIt = sigIt;
			sigIt++;

			// --- Delete digitised signal
			delete (*oldIt);
			(chanIt->second).erase(oldIt);

		} // Iterate over signals in channel
	}  // Iterate over channels

	return nDigis;
}
// -------------------------------------------------------------------------



// -----   Create list of dead channels   ----------------------------------
Int_t CbmStsModule::SetDeadChannels(Double_t percentage) {

	Double_t fraction = percentage;

	// --- Catch illegal percentage values
	if ( percentage < 0. ) {
		LOG(WARNING) << GetName() << ": illegal percentage of dead channels "
				<< percentage << ", is set to 0." << FairLogger::endl;
		fraction = 0.;
	}
	if ( percentage > 100. ) {
		LOG(WARNING) << GetName() << ": illegal percentage of dead channels "
				<< percentage << ", is set to 100." << FairLogger::endl;
		fraction = 100.;
	}

	// --- Re-set dead channel list
	fDeadChannels.clear();

	// --- Number of dead channels
	Int_t nOfDeadChannels = fraction * fNofChannels / 100;

	// --- Case percentage < 50: randomise inactive channels
	// --- N.b.: catches also zero fraction (nOfDeadChannels = 0)
	// --- N.b.: set::insert has no effect if element is already present
	if ( nOfDeadChannels < (fNofChannels / 2) ) {
		while ( fDeadChannels.size() < nOfDeadChannels )
			fDeadChannels.insert( Int_t( gRandom->Uniform(fNofChannels) ) );
	}

	// --- Case percentage > 50: randomise active channels
	// --- N.b.: catches also unity fraction (nOfDeadChannels = fNofChannels)
	// --- N.b.: set::erase has no effect if element is not present
	else {
		for (Int_t channel = 0; channel < fNofChannels; channel++)
			fDeadChannels.insert(channel);
		while ( fDeadChannels.size() > nOfDeadChannels )
			fDeadChannels.erase( Int_t ( gRandom->Uniform(fNofChannels) ) );
	}

	return fDeadChannels.size();
}
// -------------------------------------------------------------------------



// -----   Status info   ---------------------------------------------------
void CbmStsModule::Status() const {
	LOG(INFO) << GetName() << ": Signals " << fAnalogBuffer.size()
			      << ", digis " << fDigis.size()
			      << ", clusters " << fClusters.size() << FairLogger::endl;
}
// -------------------------------------------------------------------------


ClassImp(CbmStsModule)
