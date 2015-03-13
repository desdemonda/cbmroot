/**
 * \file CbmLitFieldGridCreator.h
 *
 * \brief Class creates grid with magnetic field values at a certain Z position.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 *
 */
#ifndef CBMLITFIELDGRIDCREATOR_H_
#define CBMLITFIELDGRIDCREATOR_H_

#include <vector>
#include "../../parallel/LitFieldGrid.h"
class FairField;

class CbmLitFieldGridCreator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitFieldGridCreator();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFieldGridCreator();

   /**
    * \brief Set acceptance angle for X.
    * \param[in] xangle Angle value.
    */
   void SetXangle(float xangle) {fXangle = xangle;}

   /**
    * \brief Set acceptance angle for Y.
    * \param[in] yangle Angle value.
    */
   void SetYangle(float yangle) {fYangle = yangle;}

   /**
    * \brief Set cell size for X.
    * \param[in] cellSizeX Cell size value.
    */
   void SetCellSizeX(int cellSizeX) {fCellSizeX = cellSizeX;}

   /**
    * \brief Set cell size for Y.
    * \param[in] cellSizeY Cell size value.
    */
   void SetCellSizeY(int cellSizeY) {fCellSizeY = cellSizeY;}

   /**
    * \brief Main function which creates grid with magnetic field values in (X, Y) slice.
    *
    * \param[in] Z Z position of the slice.
    * \param[out] grid Output grid.
    */
   void CreateGrid(
      fscal Z,
      lit::parallel::LitFieldGrid& grid);

private:
   FairField* fField; // Magnetic field

   float fXangle; // Acceptance angle for X [grad]
   float fYangle; // Acceptance angle for Y [grad]

   float fCellSizeX; // Cell size along X [cm]
   float fCellSizeY; // Cell size along Y [cm]
};

#endif /* CBMLITFIELDGRIDCREATOR_H_ */
