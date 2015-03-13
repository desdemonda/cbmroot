/* *******************************************************************
// (C)opyright 2005
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
//     - base class for holding the framework's track parameters
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-08-14 12:32:44 $
// $Revision: 1.2 $
//
// *******************************************************************/


#include "../include/trackFrameworkMomentum.h"
#include <math.h>


#define sqr(a)  ((a) * (a)) 


/****************************************************************
 * CLASS trackFrameworkMomentum				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackFrameworkMomentum::trackFrameworkMomentum() : tx(0.), ty(0.), qp(0.) {

  //	setTx(0);
  //	setTy(0);
  //	setQp(0);

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackFrameworkMomentum::trackFrameworkMomentum(const trackFrameworkMomentum& value) : tx(value.tx), ty(value.ty), qp(value.qp) {

  //	this->tx = value.tx;
  //	this->ty = value.ty;
  //	this->qp = value.qp;

}
trackFrameworkMomentum::trackFrameworkMomentum(double _tx, double _ty, double _qp) : tx(_tx), ty(_ty), qp(_qp) {

  //	setTx(_tx);
  //	setTy(_ty);
  //	setQp(_qp);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackFrameworkMomentum::~trackFrameworkMomentum() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackFrameworkMomentum& trackFrameworkMomentum::operator = (const trackFrameworkMomentum& value) {

	this->tx = value.tx;
	this->ty = value.ty;
	this->qp = value.qp;

	return *this;

}

/****************************************************************
 * This method converts the object into a trackMomentum object.	*
 ****************************************************************/

trackFrameworkMomentum::operator trackMomentum() {

	trackMomentum returnValue;

	returnValue.set(getQp() / sqrt(sqr(getTx()) + sqr(getTy()) + 1), PZ);
	returnValue.set(getTy() * returnValue.get(PZ), PY);
	returnValue.set(getTx() * returnValue.get(PZ), PX);

	return returnValue;

}

/****************************************************************
 * returns the value for tx										*
 ****************************************************************/

double trackFrameworkMomentum::getTx() {

	return tx;

}

/****************************************************************
 * returns the value for ty										*
 ****************************************************************/

double trackFrameworkMomentum::getTy() {

	return ty;

}

/****************************************************************
 * returns the value for qp										*
 ****************************************************************/

double trackFrameworkMomentum::getQp() {

	return qp;

}

/****************************************************************
 * sets the value for tx										*
 ****************************************************************/

void trackFrameworkMomentum::setTx(double set) {

	tx = set;

}

/****************************************************************
 * sets the value for ty										*
 ****************************************************************/

void trackFrameworkMomentum::setTy(double set) {

	ty = set;

}

/****************************************************************
 * sets the value for qp										*
 ****************************************************************/

void trackFrameworkMomentum::setQp(double set) {

	qp = set;

}
