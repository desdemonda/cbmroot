#include "Application.hpp"
#include "TimesliceReceiver.hpp"
#include "TimesliceInputArchive.hpp"
#include <iostream>

#include <sys/stat.h>
#include <boost/format.hpp>

Application::Application(Parameters const& par) 
  : _par(par)
  , _source()
  , _trbBridge()

{
   if (!_par.shm_identifier().empty()) {
      _source.reset(new fles::TimesliceReceiver(_par.shm_identifier()));
   } else {
      fles::TimesliceInputArchive * inputArchive = new fles::TimesliceInputArchive(_par.input_archive()); 
      _source.reset(inputArchive);

      // quite ugly, but there's not better way to find out when the data was recorded
      // actually, mtime should point to the timestamp, when data was last modified
      // but let's ignore this for the moment
      struct stat attrib;
      stat(_par.input_archive().c_str(), &attrib);
      
      _trbBridge.setArchiveCreationTime(attrib.st_mtime);
      _trbBridge.setMCSize(_par.MCSize() );
      _trbBridge.setOfflineProcessing(true);
   }

   if (!_par.output_archive().empty())
      _trbBridge.openFile( _par.output_archive() );

   if (_par.analyze()) {
      _trbBridge.setFlags( _trbBridge.getFlags() | TrbBridge::SSEStructCheck );
   }
   
   if (_par.showExceptions()) {
      _trbBridge.setFlags( _trbBridge.getFlags() | TrbBridge::ShowExceptions );
   }

   if (_par.client_index() != -1) {
      std::cout << "trbnet_client " << _par.client_index() << ": "  << par.shm_identifier() << std::endl;
   }
}

Application::~Application()
{
    if (_par.client_index() != -1) {
        std::cout << "trbnet_client " << _par.client_index() << ": ";
    }
    std::cout << "total timeslices processed: " << _count << std::endl;
}

void Application::run()
{
   unsigned int skipCounter = 0;
   unsigned int skipMicroslices = 0;
   
   
   while (auto timeslice = _source->get()) {
      if (_par.skipTimeslices() <= skipCounter) {
         _trbBridge.importTimeslice(*timeslice, skipMicroslices);
         _count++;
      } else {
         skipCounter++;
      }
      
      if ((_par.timesliceLimit() > 0) && (static_cast<unsigned int>(_par.timesliceLimit()) < _count)) {
         std::cout << "Timeslice limit reached" << std::endl;
         break;
      }
      skipMicroslices = _par.overlapSize();
   }
   
   //std::cout << _trbBridge.getCurrentStatistics().dump();
}
