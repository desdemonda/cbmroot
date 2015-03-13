#ifndef ROC_OPERLIST_H
#define ROC_OPERLIST_H

#include <vector>

#include <stdint.h>

namespace roc {

   struct Oper {
      bool     isput;
      uint32_t addr;
      uint32_t value;

      Oper() : isput(false), addr(0), value(0) {}
      
      Oper(bool _isput, uint32_t _addr, uint32_t _value = 0) :
         isput(_isput),
         addr(_addr),
         value(_value)
      {
      }

      Oper(const Oper& src) :
         isput(src.isput),
         addr(src.addr),
         value(src.value)
      {
      }

   };

   class OperList {
      protected:
         std::vector<Oper>  fList;
         
         int               fErrorOper; // number of operation failed, -1 ok
         int               fErrorCode; // error code of operation, 0 - ok

      public:
         OperList();
         OperList(int noper, bool* isput, uint32_t* addr, uint32_t* value);
         OperList(const OperList& src);
         virtual ~OperList();

         int number() const { return (int) fList.size(); }
         Oper& oper(int n) { return fList[n]; }
         const Oper& oper(int n) const { return fList[n]; }
         bool isput(int n) const { return oper(n).isput; }

         void clear() { fList.clear(); fErrorOper = -1; fErrorCode = 0; }

         void addOper(const Oper& oper)
           { fList.push_back(oper); }

         void addOper(bool isput, uint32_t addr, uint32_t value)
           { addOper(Oper(isput, addr, value)); }

         void addPut(uint32_t addr, uint32_t value) { addOper(true, addr, value); }

         void addGet(uint32_t addr) { addOper(false, addr, 0); }
         
         void print();
         
         int getErrorOper() const { return fErrorOper; }
         void setErrorOper(int n) { fErrorOper = n; }
         
         int getErrorCode() const { return fErrorCode; } 
         void setErrorCode(int code) { fErrorCode = code; }
   };

}



#endif

