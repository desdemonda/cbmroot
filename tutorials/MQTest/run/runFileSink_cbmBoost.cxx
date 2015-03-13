/**
 * runFileSinkBoost.cxx
 *
 * @since 2013-01-21
 * @author A. Rybalchenko, N. Winckler
 */

#include <iostream>
#include <csignal>

#include "FairMQLogger.h"
#include "CbmMQFileSink.h"

#ifdef NANOMSG
  #include "FairMQTransportFactoryNN.h"
#else
  #include "FairMQTransportFactoryZMQ.h"
#endif

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

using std::cout;
using std::cin;
using std::endl;
using std::stringstream;

typedef boost::archive::binary_iarchive TBoostBinPayload; // boost binary format
typedef boost::archive::text_iarchive TBoostTextPayload; // boost text format
typedef CbmMQFileSink<TBoostBinPayload> TSink;

TSink filesink;

static void s_signal_handler (int signal)
{
  cout << endl << "Caught signal " << signal << endl;

  filesink.ChangeState(TSink::STOP);
  filesink.ChangeState(TSink::END);

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
  if ( argc != 7 ) {
    cout << "Usage: fileSink \tID numIoTreads\n"
              << "\t\tinputSocketType inputRcvBufHSize inputMethod inputAddress\n"
              << endl;
    return 1;
  }

  s_catch_signals();

  LOG(INFO) << "PID: " << getpid();

#ifdef NANOMSG
  FairMQTransportFactory* transportFactory = new FairMQTransportFactoryNN();
#else
  FairMQTransportFactory* transportFactory = new FairMQTransportFactoryZMQ();
#endif

  filesink.SetTransport(transportFactory);

  int i = 1;

  filesink.SetProperty(TSink::Id, argv[i]);
  ++i;

  int numIoThreads;
  stringstream(argv[i]) >> numIoThreads;
  filesink.SetProperty(TSink::NumIoThreads, numIoThreads);
  ++i;

  filesink.SetProperty(TSink::NumInputs, 1);
  filesink.SetProperty(TSink::NumOutputs, 0);

  filesink.ChangeState(TSink::INIT);
  filesink.InitOutputFile(argv[1]);


  filesink.SetProperty(TSink::InputSocketType, argv[i], 0);
  ++i;
  int inputRcvBufSize;
  stringstream(argv[i]) >> inputRcvBufSize;
  filesink.SetProperty(TSink::InputRcvBufSize, inputRcvBufSize, 0);
  ++i;
  filesink.SetProperty(TSink::InputMethod, argv[i], 0);
  ++i;
  filesink.SetProperty(TSink::InputAddress, argv[i], 0);
  ++i;


  filesink.ChangeState(TSink::SETOUTPUT);
  filesink.ChangeState(TSink::SETINPUT);
  
  
  try
  {
      filesink.ChangeState(TSink::RUN);
  }
  catch (boost::archive::archive_exception& e)
  {
      LOG(ERROR) << e.what();
  }
  
  char ch;
  cin.get(ch);
  
  //sleep(3);

  filesink.ChangeState(TSink::STOP);
  filesink.ChangeState(TSink::END);

  return 0;
}

