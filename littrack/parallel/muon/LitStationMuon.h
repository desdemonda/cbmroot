/**
 * \file LitStationMuon
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Station in muon detector layout.
 **/

#ifndef LITSTATIONMUON_H_
#define LITSTATIONMUON_H_

#include "LitSubstationMuon.h"
#include "../LitEnums.h"

#include <vector>

namespace lit {
namespace parallel {

/**
 * \class LitStationMuon
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Station in muon detector layout.
 **/
template<class T>
class LitStationMuon
{
public:
   /**
    * \brief Constructor.
    */
   LitStationMuon():
      fType(kLITPIXELHIT),
      fSubstations() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitStationMuon() {}

   /**
    * \brief Add substation to station.
    * \param[in] substation Substation to be added.
    */
   void AddSubstation(const LitSubstationMuon<T>& substation) {
      fSubstations.push_back(substation);
   }

   /**
    * \brief Return substation by index.
    * \param[in] index Index of substation.
    * \return Substation by index.
    */
   const LitSubstationMuon<T>& GetSubstation(unsigned short index) const {
      return fSubstations[index];
   }

   /**
    * \brief Return number of substations in station.
    * \return Number of substaions in station.
    */
   unsigned char GetNofSubstations() const { return fSubstations.size(); }

   /**
    * \brief Return type of hits for station.
    * \return Type of hits for station.
    */
   LitHitType GetType() const { return fType; }

   /**
    * \brief Set type of hits for station.
    * \param[in] type Type value.
    */
   void SetType(LitHitType type) { fType = type; }

   /**
    * \brief Return std::string representation for the class.
    * \return std::string representation for the class.
    */
   std::string ToString() const {
      std::string str = "LitStationMuon: type=" + lit::parallel::ToString<LitHitType>(GetType())
            + ", nofSubstations=" + lit::parallel::ToString<int>((int)GetNofSubstations()) + "\n";
      for (unsigned char i = 0; i < GetNofSubstations(); i++) {
         str += "    " + lit::parallel::ToString<int>((int)i) + " " + GetSubstation(i).ToString();
      }
      return str;
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return std::ostream for continuous output.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitStationMuon& station) {
      strm << station.ToString();
      return strm;
   }

private:
   LitHitType fType; // Type of hits on the station
   std::vector<LitSubstationMuon<T> > fSubstations; // Array of substations in the station
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitStationMuon<fvec> LitStationMuonVec;
typedef LitStationMuon<fscal> LitStationMuonScal;

} // namespace parallel
} // namespace lit
#endif /* LITSTATIONMUON_H_ */
