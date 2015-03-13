/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - base class for the first look-up-tables
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:36:37 $
// $Revision: 1.3 $
//
// *******************************************************************/


#include "../include/prelut.h"


/****************************************************************
 * Default constructor											*
 ****************************************************************/

prelut::prelut() 
  : def(),
    border()
{

	init(0, 0, 0, 0, 0);

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

prelut::prelut(double dim3Min, double dim3Max, int dim3Step) 
  : def(),
    border()
{

	init(dim3Min, dim3Max, dim3Step, 0, 0);

}
prelut::prelut(double dim3StartEntry, double dim3StopEntry) 
  : def(),
    border()
{

	init(0, 0, 0, dim3StartEntry, dim3StopEntry);

}
prelut::prelut(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry) 
  : def(),
    border()
{

	init(dim3Min, dim3Max, dim3Step, dim3StartEntry, dim3StopEntry);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

prelut::~prelut() {

}

/****************************************************************
 * Method inits the variables based on the detector stations.	*
 ****************************************************************/

void prelut::init(double dim3Min, double dim3Max, int dim3Step, double dim3StartEntry, double dim3StopEntry) {

	def.dim3Min  = dim3Min;
	def.dim3Max  = dim3Max;
	def.dim3Step = dim3Step;
	if (dim3StartEntry > dim3StopEntry)
		def.dim3StartEntry = dim3StopEntry;
	else
		def.dim3StartEntry = dim3StartEntry;
	def.dim3StopEntry = dim3StopEntry;
	border.start = 0;
	border.stop  = 0;

}

/****************************************************************
 * Method inits the variables based on the detector stations.	*
 ****************************************************************/

void prelut::set(double dim3Min, double dim3Max, int dim3Step) {

	def.dim3Min  = dim3Min;
	def.dim3Max  = dim3Max;
	def.dim3Step = dim3Step;

}
void prelut::set(double dim3StartEntry, double dim3StopEntry) {

	if (dim3StartEntry > dim3StopEntry)
		def.dim3StartEntry = dim3StopEntry;
	else
		def.dim3StartEntry = dim3StartEntry;
	def.dim3StopEntry = dim3StopEntry;

}

/****************************************************************
 * This method returns the Definition of the prelut object.		*
 ****************************************************************/

prelutDefinition prelut::getPrelutDefinition() {

	return def;

}

/****************************************************************
 * This method returns the border.								*
 ****************************************************************/

prelutHoughBorder prelut::getBorder() {

	return border;

}
