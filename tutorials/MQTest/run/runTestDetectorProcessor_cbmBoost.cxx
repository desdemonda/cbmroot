/**
 * runTestDetectorProcessorBoost.cxx
 *
 *  Created on: Oct 26, 2012
 *      Author: dklein
 */

#include <iostream>
#include <csignal>

#include "FairMQLogger.h"
#include "CbmMicroSliceMerger.h"
#include "FairMQProcessor.h"
//#include "CbmProcessor.h"
#ifdef NANOMSG
  #include "FairMQTransportFactoryNN.h"
#else
  #include "FairMQTransportFactoryZMQ.h"
#endif



#include "CbmMicroSliceMergerTask.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>


using std::cout;
using std::cin;
using std::endl;
using std::stringstream;

typedef boost::archive::binary_iarchive TBoostBinPayloadIn; // boost binary format
typedef boost::archive::text_iarchive TBoostTextPayloadIn; // boost text format
typedef boost::archive::binary_oarchive TBoostBinPayloadOut; // boost binary format
typedef boost::archive::text_oarchive TBoostTextPayloadOut; // boost text format
typedef CbmMicroSliceMergerTask<TBoostBinPayloadIn, TBoostBinPayloadOut> TProcessorTask;



CbmMicroSliceMerger<TProcessorTask> processor;

static void s_signal_handler (int signal)
{
  cout << endl << "Caught signal " << signal << endl;

  processor.ChangeState(CbmMicroSliceMerger<TProcessorTask>::STOP);
  processor.ChangeState(CbmMicroSliceMerger<TProcessorTask>::END);

  cout << "Shutdown complete. Bye!" << endl;
  exit(1);
}

static void s_catch_signals (void)
{
  struct sigaction action;
  action.sa_handler = s_signal_handler;
  action.sa_flags = 0;
  sigemptyset(&action.sa_mask);
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGTERM, &action, NULL);
}

int main(int argc, char** argv)
{
  if ( argc != 18 ) 
  {
      cout<<"argc="<<argc<<endl;
    
      cout << "Usage: testDetectorProcessor \tMicroSliceNumber TimeSliceIndex ID numIoTreads\n"
              << "\t\tinputSocketType inputRcvBufSize inputMethod inputAddress\n"
              << "\t\toutputSocketType outputSndBufSize outputMethod outputAddress\n" << endl;
    return 1;
  }

  s_catch_signals();

  LOG(INFO) << "PID: " << getpid();

#ifdef NANOMSG
  FairMQTransportFactory* transportFactory = new FairMQTransportFactoryNN();
#else
  FairMQTransportFactory* transportFactory = new FairMQTransportFactoryZMQ();
#endif

  processor.SetTransport(transportFactory);

  int i = 1;
  uint64_t MSNumber;
  stringstream(argv[i]) >> MSNumber;
  processor.SetMicroSliceNum(MSNumber);// MicroSlice number to merge
  ++i;
  uint64_t TSIndex;
  stringstream(argv[i]) >> TSIndex;
  processor.SetTimeSliceIdx(TSIndex);// TimeSlice index
  ++i;
  processor.SetProperty(CbmMicroSliceMerger<TProcessorTask>::Id, argv[i]);
  ++i;
  
  int numIoThreads;
  stringstream(argv[i]) >> numIoThreads;
  processor.SetProperty(CbmMicroSliceMerger<TProcessorTask>::NumIoThreads, numIoThreads);
  ++i;

  
  int numInputs;
  stringstream(argv[i]) >> numInputs;
  processor.SetProperty(CbmMicroSliceMerger<TProcessorTask>::NumInputs, numInputs);
  ++i;
  processor.SetProperty(CbmMicroSliceMerger<TProcessorTask>::NumOutputs, 1);


  processor.ChangeState(CbmMicroSliceMerger<TProcessorTask>::INIT);

  
    for (int iInput = 0; iInput < numInputs; iInput++)
    {
        //cout<<""<< argv[i] <<endl;
        processor.SetProperty(CbmMicroSliceMerger<TProcessorTask>::InputSocketType, argv[i], iInput);
        ++i;//cout<<""<< argv[i] <<endl;
        int inputRcvBufSize;
        stringstream(argv[i]) >> inputRcvBufSize;
        processor.SetProperty(CbmMicroSliceMerger<TProcessorTask>::InputRcvBufSize, inputRcvBufSize, iInput);
        ++i;//cout<<""<< argv[i] <<endl;
        processor.SetProperty(CbmMicroSliceMerger<TProcessorTask>::InputMethod, argv[i], iInput);
        ++i;//cout<<""<< argv[i] <<endl;
        processor.SetProperty(CbmMicroSliceMerger<TProcessorTask>::InputAddress, argv[i], iInput);
        ++i;
    }

  processor.SetProperty(CbmMicroSliceMerger<TProcessorTask>::OutputSocketType, argv[i], 0);
  ++i;
  int outputSndBufSize;
  stringstream(argv[i]) >> outputSndBufSize;
  processor.SetProperty(CbmMicroSliceMerger<TProcessorTask>::OutputSndBufSize, outputSndBufSize, 0);
  ++i;
  processor.SetProperty(CbmMicroSliceMerger<TProcessorTask>::OutputMethod, argv[i], 0);
  ++i;
  processor.SetProperty(CbmMicroSliceMerger<TProcessorTask>::OutputAddress, argv[i], 0);
  ++i;


  processor.ChangeState(CbmMicroSliceMerger<TProcessorTask>::SETOUTPUT);
  processor.ChangeState(CbmMicroSliceMerger<TProcessorTask>::SETINPUT);
  
  try
  {
      processor.ChangeState(CbmMicroSliceMerger<TProcessorTask>::RUN);
  }
  catch (boost::archive::archive_exception& e)
  {
      LOG(ERROR) << e.what();
  }  
  
  char ch;
  cin.get(ch);

  processor.ChangeState(CbmMicroSliceMerger<TProcessorTask>::STOP);
  processor.ChangeState(CbmMicroSliceMerger<TProcessorTask>::END);

  return 0;
}

