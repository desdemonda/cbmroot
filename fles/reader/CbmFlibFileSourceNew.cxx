// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          CbmFlibFileSourceNew                        -----
// -----                    Created 01.11.2013 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include "CbmFlibFileSourceNew.h"

#include "CbmDaqBuffer.h"

#include "TimesliceInputArchive.hpp"
#include "Timeslice.hpp"
#include "TimesliceSubscriber.hpp"
#include "MicrosliceContents.hpp"

#include "FairLogger.h"

#include <iostream>
#include <fstream>

CbmFlibFileSourceNew::CbmFlibFileSourceNew()
  : FairSource(),
    fFileName(""),
    fInputFileList(new TObjString()),
    fFileCounter(0),
    fHost("localhost"),
    fPort(5556),
    fUnpackers(),
    fBuffer(CbmDaqBuffer::Instance()),
    fTSNumber(0),
    fTSCounter(0),
    fTimer(),
    fSource(NULL)
{
}

CbmFlibFileSourceNew::CbmFlibFileSourceNew(const CbmFlibFileSourceNew& source)
  : FairSource(source),
    fFileName(""),
    fInputFileList(),
    fFileCounter(0),
    fHost("localhost"),
    fPort(5556),
    fUnpackers(),
    fBuffer(CbmDaqBuffer::Instance()),
    fTSNumber(0),
    fTSCounter(0),
    fTimer(),
    fSource(NULL)
{
}

CbmFlibFileSourceNew::~CbmFlibFileSourceNew()
{
}

Bool_t CbmFlibFileSourceNew::Init()
{
  if ( 0 == fFileName.Length() ) {
    TString connector = Form("tcp://%s:%i", fHost.Data(), fPort);
    LOG(INFO) << "Open TSPublisher at " << connector << FairLogger::endl;
    fInputFileList.Add(new TObjString(connector));
    fSource = new fles::TimesliceSubscriber(connector.Data());
    if ( !fSource) { 
      LOG(FATAL) << "Could not connect to publisher." << FairLogger::endl;
    } 
  } else {
    // --- Open input file 
    TObjString* tmp =
      dynamic_cast<TObjString*>(fInputFileList.At(fFileCounter));
    fFileName = tmp->GetString();

    LOG(INFO) << "Open the Flib input file " << fFileName << FairLogger::endl;
    // Check if the input file exist
    FILE* inputFile = fopen(fFileName.Data(), "r");
    if ( ! inputFile )  {
      LOG(FATAL) << "Input file " << fFileName << " doesn't exist." << FairLogger::endl;
    }
    fclose(inputFile);
    fSource = new fles::TimesliceInputArchive(fFileName.Data());
    if ( !fSource) { 
      LOG(FATAL) << "Could not open input file." << FairLogger::endl;
    } 
  }

  for (auto it=fUnpackers.begin(); it!=fUnpackers.end(); ++it) {
    LOG(INFO) << "Initialize " << it->second->GetName() << 
      " for systemID 0x" << std::hex << it->first << std::dec << FairLogger::endl;
    it->second->Init();
    //    it->second->Register();
  }
  
  return kTRUE;
}

#ifdef _NewFairSource
Int_t CbmFlibFileSourceNew::ReadEvent(UInt_t) 
#else
Int_t CbmFlibFileSourceNew::ReadEvent()       
#endif
{
  
  //  fTimer.Start();
  //while( fFileCounter < fInputFileList.GetSize() ) {
    while (auto timeslice = fSource->get()) {
      fTSCounter++;
      if ( 0 == fTSCounter%100 ) {
	/*
        fTimer.Stop();
	Double_t rtime = fTimer.RealTime(); 
	Double_t ctime = fTimer.CpuTime();
	*/
	LOG(INFO) << "Analyse Event " << fTSCounter << FairLogger::endl;
	/*
        LOG(INFO) << "Real time/100 timeslices: " << rtime << " s" << FairLogger::endl; 
        LOG(INFO) << "CPU time/100 timeslices: " << ctime << " s" << FairLogger::endl; 
	fTimer<.Start();
	*/
      }
      const fles::Timeslice& ts = *timeslice;
      auto tsIndex = ts.index();
      if( (tsIndex != (fTSNumber+1)) &&( fTSNumber != 0) ) {
	LOG(WARNING) << "Missed Timeslices. Old TS Number was " << fTSNumber 
		     << " New TS Number is " << tsIndex << FairLogger::endl;
      }
      fTSNumber=tsIndex;    
      for (size_t c {0}; c < ts.num_components(); c++) {
	auto systemID = ts.descriptor(c, 0).sys_id;
	
	if(gLogger->IsLogNeeded(DEBUG)) {
	  PrintMicroSliceDescriptor(ts.descriptor(c, 0));
	}

	auto it=fUnpackers.find(systemID);
	if (it == fUnpackers.end()) {
	  //LOG(FATAL) << "Could not find unpacker for system id 0x" << 
	  //std::hex << systemID << std::dec << FairLogger::endl;
	} else {
	  it->second->DoUnpack(ts, c);
	}
      }
      return 0;
    }
    /*
    // Check if there is another file in the list
    fFileCounter += 1;

    if ( fInputFileList.GetSize() > fFileCounter ) {
      delete fSource;
      TObjString* tmp =
	dynamic_cast<TObjString*>(fInputFileList.At(fFileCounter));
      fFileName = tmp->GetString();
      LOG(INFO) << "Open the Flib input file " << fFileName << FairLogger::endl;
      // Check if the input file exist
      FILE* inputFile = fopen(fFileName.Data(), "r");
      if ( ! inputFile )  {
        LOG(FATAL) << "Input file " << fFileName << " doesn't exist." << FairLogger::endl;
      }
      fclose(inputFile);
      fSource = new fles::TimesliceInputArchive(fFileName.Data());
    }
    */
    // } 
  for (auto it=fUnpackers.begin(); it!=fUnpackers.end(); ++it) {
    LOG(INFO) << "Finish " << it->second->GetName() << 
      " for systemID 0x" << std::hex << it->first << std::dec << FairLogger::endl;
    it->second->Finish();
    //    it->second->Register();
  }
  return 1; // no more data; trigger end of run
}

void CbmFlibFileSourceNew::PrintMicroSliceDescriptor(const fles::MicrosliceDescriptor& mdsc)
{
  LOG(INFO) << "Header ID: Ox" << std::hex << static_cast<int>(mdsc.hdr_id) 
	    << std::dec << FairLogger::endl;
  LOG(INFO) << "Header version: Ox" << std::hex << static_cast<int>(mdsc.hdr_ver) 
	    << std::dec << FairLogger::endl;
  LOG(INFO) << "Equipement ID: " << mdsc.eq_id << FairLogger::endl;
  LOG(INFO) << "Flags: " << mdsc.flags << FairLogger::endl;
  LOG(INFO) << "Sys ID: Ox" << std::hex << static_cast<int>(mdsc.sys_id) 
	    << std::dec << FairLogger::endl;
  LOG(INFO) << "Sys version: Ox" << std::hex << static_cast<int>(mdsc.sys_ver) 
	    << std::dec << FairLogger::endl;
  LOG(INFO) << "Microslice Idx: " << mdsc.idx << FairLogger::endl; 
  LOG(INFO) << "Checksum: " << mdsc.crc << FairLogger::endl;
  LOG(INFO) << "Size: " << mdsc.size << FairLogger::endl;
  LOG(INFO) << "Offset: " << mdsc.offset << FairLogger::endl;
}

Bool_t CbmFlibFileSourceNew::CheckTimeslice(const fles::Timeslice& ts)
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

void CbmFlibFileSourceNew::Close()
{
}

void CbmFlibFileSourceNew::Reset()
{
  for (auto it=fUnpackers.begin(); it!=fUnpackers.end(); ++it) {
    it->second->Reset();
  }
}

ClassImp(CbmFlibFileSourceNew)
