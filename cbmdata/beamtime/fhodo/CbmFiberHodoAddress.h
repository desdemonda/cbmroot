/**
 * \file CbmFiberHodoAddress.h
 * \author F. Uhlig <f.uhlig@gsi.de>
 * \date 11.12.13
 * \brief Helper class to convert unique channel ID back and forth.
 *
 *                                    3         2         1          shift length
 * Current definition:               10987654321098765432109876543210
 * System ID (kFHODO=10) on bits 0-3 00000000000000000000000000001111 <<0  15
 * Station ID on bits 4-7            00000000000000000000000011110000 <<4  15
 * Side ID on bits 8-8               00000000000000000000000100000000 <<8   1
 * Strip ID on bits 9-14             00000000000000000111111000000000 <<9  63
 *
 **/

#ifndef CBMFIBERHODODETECTORID_H
#define CBMFIBERHODODETECTORID_H 1

#include "CbmDetectorList.h"
#include <cassert>

class CbmFiberHodoAddress
{
public:

   /**
   * \brief Return address from system ID, side and strip IDs.
   * \param[in] stationId Station ID.
   * \param[in] sideId Layer ID.
   * \param[in] stripId Strip ID.
   * \return Address from system ID, side and strip IDs.
   **/
   static Int_t GetAddress(Int_t stationId,
                           Int_t sideId,
                           Int_t stripId) 
   {
     assert(!(stationId < 0 || stationId > fgkStationIdLength 
            || sideId < 0 || sideId > fgkSideIdLength 
            || stripId < 0 || stripId > fgkStripIdLength));
     return (kFHODO << fgkSystemIdShift) | (stationId << fgkStationIdShift) 
            | (sideId << fgkSideIdShift) | (stripId << fgkStripIdShift); 
   }

   /**
   * \brief Return System identifier from address.
   * \param[in] address Unique channel address.
   * \return System identifier from address.
   **/
   static Int_t GetSystemId(Int_t address) {
      return (address & (fgkSystemIdLength << fgkSystemIdShift)) >> fgkSystemIdShift;
   }

   /**
   * \brief Return station ID from address.
   * \param[in] address Unique channel address.
   * \return Station ID from address.
   **/
   static Int_t GetStationId(Int_t address) {
      return (address & (fgkStationIdLength << fgkStationIdShift)) >> fgkStationIdShift;
   }

   /**
   * \brief Return side ID from address.
   * \param[in] address Unique channel address.
   * \return Layer ID from address.
   **/
   static Int_t GetSideId(Int_t address) {
      return (address & (fgkSideIdLength << fgkSideIdShift)) >> fgkSideIdShift;
   }

   /**
   * \brief Return strip ID from address.
   * \param[in] address Unique channel address.
   * \return Strip ID from address.
   **/
   static Int_t GetStripId(Int_t address) {
      return (address & (fgkStripIdLength << fgkStripIdShift)) >> fgkStripIdShift;
   }

   /**
   * \brief Return layer address from address.
   * \param[in] address Unique channel address.
   * \return layer address from address.
   **/
   static Int_t GetLayerAddress(Int_t address) {
      return (address & (fgkLayerAddressLength << fgkLayerAddressStart)) >> fgkLayerAddressStart;
   }
   


private:

   // Length of the index of the corresponding volume
   static const Int_t fgkSystemIdLength = 15; // 2^4 - 1
   static const Int_t fgkStationIdLength = 15; // 2^4 - 1
   static const Int_t fgkSideIdLength = 1; // 2^1 - 1
   static const Int_t fgkStripIdLength = 63; // 2^6 - 1
   // Number of a start bit for each volume
   static const Int_t fgkSystemIdShift = 0;
   static const Int_t fgkStationIdShift = 4;
   static const Int_t fgkSideIdShift = 8;
   static const Int_t fgkStripIdShift = 9;
   // Start bit and length for unique layer address.
   // For unique layer address use all from station and side ID.
   static const Int_t fgkLayerAddressStart = 4;
   static const Int_t fgkLayerAddressLength = 31; // 2^5 - 1

};

#endif
