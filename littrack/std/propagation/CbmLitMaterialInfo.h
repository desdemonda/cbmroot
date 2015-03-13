/** CbmLitMaterialInfo.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2008
 * @version 1.0
 **
 ** Data class to store information about detector material.
 **/

#ifndef CBMLITMATERIALINFO_H_
#define CBMLITMATERIALINFO_H_

#include "base/CbmLitFloat.h"

#include <string>
#include <sstream>

using std::string;

class CbmLitMaterialInfo
{
public:
   /* Constructor */
   CbmLitMaterialInfo():
      fLength(0.),
      fRL(0.),
      fRho(0.),
      fZ(0.),
      fA(0.),
      fZpos(0.),
      fName(""){}

   // This constructor never used!
//   /* Constructor with assignment
//    * @param length Length of the material [cm]
//    * @param rl Radiation length [cm]
//    * @param rho Density [g/cm^3]
//    * @param Z
//    * @param A
//    * @param zpos Z position of the material
//    */
//   CbmLitMaterialInfo(
//      litfloat length,
//      litfloat rl,
//      litfloat rho,
//      litfloat Z,
//      litfloat A,
//      litfloat zpos,
//      string name):
//      fLength(length),
//      fRL(rl),
//      fRho(rho),
//      fZ(Z),
//      fA(A),
//      fZpos(zpos),
//      fName(name) {}

   /* Destructor */
   virtual ~CbmLitMaterialInfo() {};

   /*@return Length of the material */
   litfloat GetLength() const { return fLength;}

   /*@return Radiation length */
   litfloat GetRL() const { return fRL;}

   /*@return Density */
   litfloat GetRho() const { return fRho;}

   /*@return Atomic number */
   litfloat GetZ() const { return fZ;}

   /*@return  Atomic mass */
   litfloat GetA() const { return fA;}

   /*@return Z position of the material */
   litfloat GetZpos() const { return fZpos;}

   const string& GetName() const { return fName;}

   /* Sets length of the material */
   void SetLength(litfloat length) {fLength = length;}

   /* Sets radiation length of the material */
   void SetRL(litfloat rl) {fRL = rl;}

   /* Sets density */
   void SetRho(litfloat rho) {fRho = rho;}

   /* Sets atomic number */
   void SetZ(litfloat Z) {fZ = Z;}

   /* Sets atomic mass */
   void SetA(litfloat A) {fA = A;}

   /* Sets Z position of the material */
   void SetZpos(litfloat zpos) {fZpos = zpos;}

   void SetName(const string& name) {fName = name;}

   /* @return String representation of the class */
   virtual std::string ToString() const {
      std::stringstream ss;
      ss << "MaterialInfo: length=" << fLength << " rl=" << fRL
         << " rho=" << fRho << " Z=" << fZ << " A=" << fA << " zpos=" << fZpos
         << " name=" << fName << std::endl;
      return ss.str();
   }

private:
   litfloat fLength; // Length of the material [cm]
   litfloat fRL; // Radiation length [cm]
   litfloat fRho; // Density [g/cm^3]
   litfloat fZ; // Atomic number
   litfloat fA; // Atomic mass
   litfloat fZpos; // Z position of the material
   string fName; // Name of material
};

#endif /*CBMLITMATERIALINFO_H_*/
