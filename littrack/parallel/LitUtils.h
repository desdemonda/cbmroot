/**
  * \file LitUtils.h
 *
 * \brief Useful classes and functions.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 */

#ifndef LITUTILS_H_
#define LITUTILS_H_

#include <string>
#include <sstream>

namespace lit {
namespace parallel {

/**
 * \fn template <class T>std::string ToString(const T& value)
 * \brief Function converts object into a std::string.
 *
 * Note that operator << has to be defined for the class/type
 * in order to be able to convert to a string.
 *
 * \param[in,out] par Reference to track parameters.
 * \param[in] mat Reference to material
 */
template <class T>
std::string ToString(
   const T& value)
{
   std::stringstream ss;
   ss.precision(5);
   ss << value;
   return ss.str();
}



/**
 * \class DeleteObject
 * \brief Functor class for convenient memory release.
 *
 * For example if one has an std::vector<Hit*> of pointers
 * to Hit objects. The hit objects where allocated with new
 * operator than one can release memory with the following code:
 * for_each(hits.begin(), hits.end(), DeleteObject());
 *
 * \param[in,out] par Reference to track parameters.
 * \param[in] mat Reference to material
 */
class DeleteObject
{
public:
   template<typename T>
   void operator()(const T* ptr) const {
      delete ptr;
   }
};

} // namespace parallel
} // namespace lit
#endif /* LITUTILS_H_ */
