//============================================================================
/*! \file FebBase.h
 *  \author W.F.J.Mueller/GSI, significant adaptions
 *  \author Norbert Abel/KIP, initial version
 */
//============================================================================

#ifndef NXYTER_FEBBASE_H
#define NXYTER_FEBBASE_H

#include <vector>

#include "roc/I2cDevice.h"
#include "roc/Peripheral.h"
#include "nxyter/NxChip.h"
#include "nxyter/MainAdc.h"

namespace nxyter {

   class FebBase : public roc::Peripheral {
      protected:
         int        fPort;                  //!< port the feb is connected to
         int        fType;                  //!< FEB class/type
         std::vector<nxyter::NxChip*>  fNxChip;  //!< vector of NxChip objects
         std::vector<bool>             fNxOffline;//!< nx offline flag
         nxyter::MainAdc fMainAdc;          //!< MainAdc object
         bool       fMode4nx;               //!< 4nx mode selector
         std::vector<int> fAdcChannel;      //!< ADC channel mux settings
         roc::I2cDevice   fMonAdcDevice;    //!< monitor ADC I2C device
         roc::I2cDevice   fEepromDevice;    //!< EEProm I2C device
  
      public:
         enum FebType { kFeb1nx  = 100,     //!< single Nx FEB class
                        kFeb1nxGenB,        //!< 1nx general purpose FEB Rev B
                        kFeb1nxGenC,        //!< 1nx general purpose FEB Rev C
                        kFeb1nxGenD,        //!< 1nx general purpose FEB Rev D
                        kFeb2nx  = 200,     //!< dual Nx FEB class
                        kFeb2nxGas,         //!< 2nx FEB for gas detectors
                        kFeb4nx  = 400,     //!< quad Nx FEB class
                        kFeb4nxBT           //!< 4nx FEB CBM beam tracker
         };

      protected:
         FebBase();
         FebBase(roc::Board* board, int port, int type);
      public:
         virtual ~FebBase();

         //! Returns port (connector) number on ROC (0 or 1)
         int getPortNumber() const
           { return fPort; }

         //! Returns class of FEB, kFeb1nx, kFeb2nx, or kFeb4nx.
         int getFebClass() const
         { return 100*(fType/100); }

         //! Returns type of FEB, encoding see FebBase::FebType
         int getFebType() const
           { return fType; }

         //! Returns the number connected nXYTER on this FEB
         int numNx() const
           { return (int) fNxChip.size(); }

         nxyter::NxChip&   nx(int nxind);

         //! Gives access to the MainADC object
         nxyter::MainAdc&  adc()
           { return fMainAdc; }

         virtual int getFebState();
         virtual int initRoc();

         virtual int probe();
         virtual int setToDefault(bool ispos0=false, bool ispos1=false);
         virtual void printNxHeadLine(std::ostream& os, int nxind);
         virtual void printRegisters(std::ostream& os, 
                                     int domask=nxyter::kDoAll);

         void resetNxI2cBus();
         void resetNxI2cRegister();

         //! Set 'offline' status flag for nXYTER \a nxind.
         /*!
          * If a nXYTER is set offline it will be silently ignored in all
          * FEB level methods which loop over all nXYTERs, in particular
          * initRoc(), setToDefault(), setNxRegisterAll(). The 'offline'
          * flag does not affect the direct access to the nXYTER resources
          * via nx().
          * \param nxind see \ref glo_nxyter_nxind
          * \param isoff when \c true, nXYTER is set inactive
          * \throws std::out_of_range if \a nxind out of range
          * \sa getNxOffline()
          */
         void setNxOffline(int nxind, bool isoff)
           { fNxOffline.at(nxind) = isoff; }

         //! Returns 'offline' status flag for nXYTER \a nxind.
         /*!
          * \param nxind see \ref glo_nxyter_nxind
          * \throws std::out_of_range if \a nxind out of range
          * \sa setNxOffline()
          */
         bool getNxOffline(int nxind)
           { return fNxOffline.at(nxind); }

         int setNxTestModes(bool testpuls, bool testtrig, int calselect); 
         int setNxRegisterAll(uint8_t reg, uint8_t val, bool veri=true); 

         //! Returns monitor ADC support status.
         /*!
          * \returns \c true if the type of FEB supports a monitoring ADC.
          */
         bool monAdcSupport()
           { return fMonAdcDevice.getSlaveAddr() != 0; }

         //! Returns EEProm support status.
         /*!
          * \returns \c true if the type of FEB supports an EEProm.
          */
         bool eepromSupport()
           { return fEepromDevice.getSlaveAddr() != 0; }
           
         // ! return number of monitored ADC channels, 0 if not supported
         int getNumMonAdc();

         int getMonAdc(int ch, uint16_t &val);

      protected:
         void addNxChip(int addr, int nxnum, int adcmux);

      public:
         static const char* typeToString(int type);
         static int stringToType(const char* str);

         static void discoverFebs(roc::Board* board,
                                  int& typeport0, int& typeport1, 
                                  bool liberal=false);

         static nxyter::FebBase* newFeb(roc::Board* board, int port, int type);
   };

}

//! ostream insertion for nxyter::FebBase
/*!
 * Just calls nxyter::FebBase::printRegisters()
 * \relates nxyter::FebBase
 */
inline std::ostream& operator<<(std::ostream& os, nxyter::FebBase& obj)
  { obj.printRegisters(os); return os; }

#endif
