// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           CbmTSUnpackSpadic                       -----
// -----                    Created 07.11.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
#include "CbmTSUnpackSpadic.h"

#include "CbmSpadicRawMessage.h"

#include "TimesliceReader.hpp"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

CbmTSUnpackSpadic::CbmTSUnpackSpadic()
  : CbmTSUnpack(),
    fSpadicRaw(new TClonesArray("CbmSpadicRawMessage", 10)),
    fEpochMarkerArray(),
    fSuperEpochArray(),
    fEpochMarker(0),
    fSuperEpoch(0)
{
  for (Int_t i=0; i < NrOfSyscores; ++i) { 
    for (Int_t j=0; j < NrOfHalfSpadics; ++j) { 
      fEpochMarkerArray[i][j] = 0;
      fSuperEpochArray[i][j] = 0;
    }
  }
}

CbmTSUnpackSpadic::~CbmTSUnpackSpadic()
{
}

Bool_t CbmTSUnpackSpadic::Init()
{
  LOG(INFO) << "Initializing" << FairLogger::endl; 

  FairRootManager* ioman = FairRootManager::Instance();
  if (ioman == NULL) {
    LOG(FATAL) << "No FairRootManager instance" << FairLogger::endl;
  }
  ioman->Register("SpadicRawMessage", "spadic raw data", fSpadicRaw, kTRUE);

  return kTRUE;
}

Bool_t CbmTSUnpackSpadic::DoUnpack(const fles::Timeslice& ts, size_t component)
{
  LOG(DEBUG) << "Unpacking Spadic Data" << FairLogger::endl; 

  spadic::TimesliceReader r;
  Int_t counter=0;

  r.add_component(ts, component);

  for (auto addr : r.sources()) {
    LOG(DEBUG) << "---- reader " << addr << " ----" << FairLogger::endl;
    while (auto mp = r.get_message(addr)) {
      if(gLogger->IsLogNeeded(DEBUG)) {
	print_message(*mp);
      }
      
      Int_t link = ts.descriptor(component, 0).eq_id;
      Int_t address = addr;

      if ( mp->is_epoch_marker() ) { 
        FillEpochInfo(link, addr, mp->epoch_count());
      } 

      if ( mp->is_hit() ) { 

	GetEpochInfo(link, addr);

        Int_t triggerType = static_cast<Int_t>(mp->hit_type());
        Int_t infoType = static_cast<Int_t>(mp->info_type());
        Int_t stopType = static_cast<Int_t>(mp->stop_type());
	Int_t groupId = mp->group_id();
	Int_t channel = mp->channel_id();
	Int_t time = mp->timestamp();
        Int_t bufferOverflow = mp->buffer_overflow_count();
	Int_t samples = mp->samples().size();
	Int_t* sample_values = new Int_t[samples];

	Int_t counter1=0;
	for (auto x : mp->samples()) {
	  sample_values[counter1] = x;
	  ++counter1;
	}

	new( (*fSpadicRaw)[fSpadicRaw->GetEntriesFast()] )
	  CbmSpadicRawMessage(link, address, channel, fEpochMarker, time, 
			      fSuperEpoch, triggerType, infoType, stopType, groupId,
			      bufferOverflow, samples, sample_values);
	++counter;
	delete[] sample_values;
      }
    }
  }
  return kTRUE;
}

void CbmTSUnpackSpadic::print_message(const spadic::Message& m)
{
  LOG(DEBUG) << "v: " << (m.is_valid() ? "o" : "x");
  LOG(DEBUG) << " / gid: " << static_cast<int>(m.group_id());
  LOG(DEBUG) << " / chid: " << static_cast<int>(m.channel_id());
  if ( m.is_hit() ) { 
    LOG(DEBUG) << " / ts: " << m.timestamp();
    LOG(DEBUG) << " / samples (" << m.samples().size() << "):";
    for (auto x : m.samples()) {
      LOG(DEBUG) << " " << x;
    }
    LOG(DEBUG) << FairLogger::endl;
  } else {
    if ( m.is_epoch_marker() ) { 
      LOG(DEBUG) << " This is an Epoch Marker" << FairLogger::endl; 
    } else if ( m.is_epoch_out_of_sync() ) { 
      LOG(INFO) << " This is an out of sync Epoch Marker" << FairLogger::endl; 
    } else {
      LOG(DEBUG) << " This is not known" << FairLogger::endl;
    }
  }
}

void CbmTSUnpackSpadic::FillEpochInfo(Int_t link, Int_t addr, Int_t epoch_count) 
{
  auto it=groupToExpMap.find(link);
  if (it == groupToExpMap.end()) {
    LOG(FATAL) << "Could not find an entry for equipment ID" << 
      std::hex << link << std::dec << FairLogger::endl;
  } else {
    if ( epoch_count < fEpochMarkerArray[it->second][addr] ) {
      fSuperEpochArray[it->second][addr]++;
      LOG(DEBUG) << "Overflow of EpochCounter for Syscore" 
		<< it->second << "_Spadic"  
		<< addr << FairLogger::endl;
    } else if ((epoch_count - fEpochMarkerArray[it->second][addr]) !=1 ) {
      LOG(INFO) << "Missed epoch counter for Syscore" 
		<< it->second << "_Spadic"  
		<< addr << FairLogger::endl;
    }
    fEpochMarkerArray[it->second][addr] = epoch_count; 
  }

}

void CbmTSUnpackSpadic::GetEpochInfo(Int_t link, Int_t addr) 
{
  auto it=groupToExpMap.find(link);
  if (it == groupToExpMap.end()) {
    LOG(FATAL) << "Could not find an entry for equipment ID" << 
      std::hex << link << std::dec << FairLogger::endl;
  } else {
    fEpochMarker = fEpochMarkerArray[it->second][addr]; 
    fSuperEpoch = fSuperEpochArray[it->second][addr]; 
  }
  
}

void CbmTSUnpackSpadic::Reset()
{
  fSpadicRaw->Clear();
}

void CbmTSUnpackSpadic::Finish()
{
  for (Int_t i=0; i < NrOfSyscores; ++i) { 
    for (Int_t j=0; j < NrOfHalfSpadics; ++j) { 
      LOG(DEBUG) << "There have been " << fSuperEpochArray[i][j] 
		<< " SuperEpochs for Syscore" << i << "_Spadic" 
		<< j << " in this file" << FairLogger::endl;
    }
  }

}


/*
void CbmTSUnpackSpadic::Register()
{
}
*/


ClassImp(CbmTSUnpackSpadic)
