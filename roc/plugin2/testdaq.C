// This macro starts daq on specified ROC board and
// reads up to 10000 messages from it.
// All ROC configuration must be done in rocutil or rocGui.
// All messages will be displayed.
// brd->suspendDaq will be called after first 1000 messages,
// therefore actual number of read messages can be less than 10000
//
// It is recommended to redirect output to the file during script run like:
//    [shell]  root -l testdaq.C -q >output.txt

#ifndef __CINT__

#include "roc/Board.h"
#include "roc/Iterator.h"
#include "roc/Message.h"

#endif

void testdaq()
{
   roc::Board* brd = roc::Board::Connect("abb1");
   
   if (brd==0) return;
   
   brd->startDaq();

   int numdata(0);
   bool didsuspend(false);
   
   roc::Iterator iter(brd);
   
   while (numdata<10000) {
      if (!iter.next(1.)) {
         if (didsuspend) break;
         didsuspend = true;
         brd->suspendDaq();
      }

      iter.printMessage(7);
      
      if (iter.msg().isStopDaqMsg()) break;

      if ((numdata++ == 5000) && !didsuspend) {
         didsuspend = true;
         brd->suspendDaq();
      }
   }
   
   
   printf("Num read messages = %d\n", numdata);
   
   brd->stopDaq();

   roc::Board::Close(brd);
}
