#include "roc/OperList.h"

#include <stdio.h>

roc::OperList::OperList() :
   fList(),
   fErrorOper(-1),
   fErrorCode(0)
{

}

roc::OperList::OperList(int noper, bool* isput, uint32_t* addr, uint32_t* value) :
   fList(),
   fErrorOper(-1),
   fErrorCode(0)
{
   for (int n=0;n<noper;n++)
      addOper(isput[n], addr[n], value[n]);
}

roc::OperList::OperList(const OperList& src) :
   fList(),
   fErrorOper(-1),
   fErrorCode(0)
{
   for (int n=0;n<src.number();n++)
      addOper(src.oper(n));
}

roc::OperList::~OperList()
{
   fList.clear();

}

void roc::OperList::print()
{
   printf("OperList size = %d\n", number());
   for (int n=0;n<number();n++)
      printf("  %s %08x %08x\n", (isput(n) ? "Put" : "Get"), oper(n).addr, oper(n).value);
   printf("  ErrCode=%d  ErrOper=%d\n", fErrorCode, fErrorOper);
   
}
