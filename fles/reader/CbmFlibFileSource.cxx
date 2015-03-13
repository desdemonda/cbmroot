// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          CbmFlibFileSource                        -----
// -----                    Created 01.11.2013 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "CbmFlibFileSource.h"
#include "CbmSpadicRawMessage.h"

#include "TimesliceInputArchive.hpp"
#include "Timeslice.hpp"
#include "TimesliceSubscriber.hpp"
#include "MicrosliceContents.hpp"

// note M. Krieger, 2014-08-15: these includes should not be needed, please test
//#if 0
//#include "Message.hpp"
//#include "message_reader.h"
//#endif

#include "TimesliceReader.hpp"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
#include <fstream>

CbmFlibFileSource::CbmFlibFileSource()
  : FairSource(),
    fFileName(""),
    fHost("localhost"),
    fPort(5556),
    fSpadicRaw(new TClonesArray("CbmSpadicRawMessage", 10)),
    fTSCounter(0),
    fSource(NULL)
{
}



/*
CbmFlibFileSource::CbmFlibFileSource(const CbmFlibFileSource& source)
  : FairSource(source),
    fFileName(""),
    fHost("localhost"),
    fPort(5556),
    fSpadicRaw(NULL),
    fSource(NULL)
{
}
*/

CbmFlibFileSource::~CbmFlibFileSource()
{
}

Bool_t CbmFlibFileSource::Init()
{
  if ( 0 == fFileName.Length() ) {
    TString connector = Form("tcp://%s:%i", fHost.Data(), fPort);
    LOG(INFO) << "Open TSPublisher at " << connector << FairLogger::endl;
    fSource = new fles::TimesliceSubscriber(connector.Data());
    if ( !fSource) { 
      LOG(FATAL) << "Could not connect to publisher." << FairLogger::endl;
    } 
  } else {
    LOG(INFO) << "Open the Flib input file " << fFileName << FairLogger::endl;
    // Check if the input file exist
    FILE* inputFile = fopen(fFileName.Data(), "r");
    if ( ! inputFile )	{
      LOG(FATAL) << "Input file " << fFileName << " doesn't exist." << FairLogger::endl;
    }
    fclose(inputFile);
    // Open the input file
    fSource = new fles::TimesliceInputArchive(fFileName.Data());
    //fSource.reset(new fles::TimesliceInputArchive(fFileName.Data()));
    if ( !fSource) { 
      LOG(FATAL) << "Could not open input file." << FairLogger::endl;
    } 
  }

  FairRootManager* ioman = FairRootManager::Instance();
  ioman->Register("SpadicRawMessage", "spadic raw data", fSpadicRaw, kTRUE);
  
  return kTRUE;

}

#ifdef _NewFairSource
Int_t CbmFlibFileSource::ReadEvent(UInt_t)
#else
Int_t CbmFlibFileSource::ReadEvent()
#endif 
{
  fSpadicRaw->Clear();
  while (auto timeslice = fSource->get()) {
    fTSCounter++;
    if ( 0 == fTSCounter%100 ) {
      LOG(INFO) << "Analyse Event " << fTSCounter << FairLogger::endl;
    }
    const fles::Timeslice& ts = *timeslice;
    for (size_t c {0}; c < ts.num_components(); c++) {
      auto systemID = ts.descriptor(c, 0).sys_id;
      
      if(gLogger->IsLogNeeded(DEBUG)) {
	PrintMicroSliceDescriptor(ts.descriptor(c, 0));
      }
      switch (systemID) {
      case 0xFA:
	LOG(INFO) << "It is flesnet pattern generator data" << FairLogger::endl;
	break;
      case 0xF0:
	LOG(INFO) << "It is flib pattern generator data" << FairLogger::endl;
	break;
      case 0xBC:
	LOG(INFO) << "It is spadic data with wrong system ID" 
		  << FairLogger::endl;
        UnpackSpadicCbmNetMessage(ts, c);
	break;
      case 0x40:
	LOG(DEBUG) << "It is spadic data with correct system ID" 
		  << FairLogger::endl;
        UnpackSpadicCbmNetMessage(ts, c);
	break;
      default:
	LOG(INFO) << "Not known now" << FairLogger::endl;
      }
    }
    return 0;
  }

  return 1; // no more data; trigger end of run
}

void CbmFlibFileSource::PrintMicroSliceDescriptor(const fles::MicrosliceDescriptor& mdsc)
{
  LOG(DEBUG) << "Header ID: Ox" << std::hex << static_cast<int>(mdsc.hdr_id) 
	    << std::dec << FairLogger::endl;
  LOG(DEBUG) << "Header version: Ox" << std::hex << static_cast<int>(mdsc.hdr_ver) 
	    << std::dec << FairLogger::endl;
  LOG(DEBUG) << "Equipement ID: " << mdsc.eq_id << FairLogger::endl;
  LOG(DEBUG) << "Flags: " << mdsc.flags << FairLogger::endl;
  LOG(DEBUG) << "Sys ID: Ox" << std::hex << static_cast<int>(mdsc.sys_id) 
	    << std::dec << FairLogger::endl;
  LOG(DEBUG) << "Sys version: Ox" << std::hex << static_cast<int>(mdsc.sys_ver) 
	    << std::dec << FairLogger::endl;
  LOG(DEBUG) << "Microslice Idx: " << mdsc.idx << FairLogger::endl; 
  LOG(DEBUG) << "Checksum: " << mdsc.crc << FairLogger::endl;
  LOG(DEBUG) << "Size: " << mdsc.size << FairLogger::endl;
  LOG(DEBUG) << "Offset: " << mdsc.offset << FairLogger::endl;
}

    
void CbmFlibFileSource::UnpackSpadicCbmNetMessage(const fles::Timeslice& ts, size_t component)
{
  spadic::TimesliceReader r;
  Int_t counter=0;

  r.add_component(ts, component);

  for (auto addr : r.sources()) {
//    std::cout << "---- reader " << addr << " ----" << std::endl;
    while (auto mp = r.get_message(addr)) {
      if (gLogger->IsLogNeeded(DEBUG)) {
        print_message(*mp);
      }
      Int_t link = ts.descriptor(component, 0).eq_id;
      Int_t address = addr;
      Int_t channel = mp->channel_id();
      Int_t epoch = -1;
      Int_t time = mp->timestamp();
      Int_t samples = mp->samples().size();
      Int_t* sample_values = new Int_t[samples];
      Int_t counter1=0;
      for (auto x : mp->samples()) {
        sample_values[counter1] = x;
        ++counter1;
      }
      new( (*fSpadicRaw)[counter] )
	CbmSpadicRawMessage(link, address, channel, epoch, time, -1, -1, -1, -1,  -1, -1, samples, sample_values);
      ++counter;
      delete[] sample_values;
    }
  }

}

void CbmFlibFileSource::print_message(const spadic::Message& m)
{
  std::cout << "v: " << (m.is_valid() ? "o" : "x");
  std::cout << " / gid: " << static_cast<int>(m.group_id());
  std::cout << " / chid: " << static_cast<int>(m.channel_id());
  if ( m.is_hit() ) { 
    std::cout << " / ts: " << m.timestamp();
    std::cout << " / samples (" << m.samples().size() << "):";
    for (auto x : m.samples()) {
      std::cout << " " << x;
    }
    std::cout << std::endl;
  } else {
    if ( m.is_epoch_marker() ) { 
      std::cout << " This is an Epoch Marker" << std::endl; 
    } else if ( m.is_epoch_out_of_sync() ) { 
      std::cout << " This is an out of sync Epoch Marker" << std::endl; 
    } else {
      std::cout << " This is not known" << std::endl;
    }
  }
}




Bool_t CbmFlibFileSource::CheckTimeslice(const fles::Timeslice& ts)
{
    if ( 0 == ts.num_components() ) {
      LOG(ERROR) << "No Component in TS " << ts.index() 
		 << FairLogger::endl;
      return 1;
    }
    LOG(INFO) << "Found " << ts.num_components() 
	      << " different components in tiemeslice" << FairLogger::endl; 
    return kTRUE;
}

void CbmFlibFileSource::Close()
{
}

void CbmFlibFileSource::Reset()
{
}


ClassImp(CbmFlibFileSource)
