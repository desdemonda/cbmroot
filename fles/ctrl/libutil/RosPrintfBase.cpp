// $Id: RosPrintfBase.cpp 20 2013-08-10 16:42:54Z mueller $
//
// Copyright 2011- by Walter F.J. Mueller <W.F.J.Mueller@gsi.de>
//
// This program is free software; you may redistribute and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 2, or at your option any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY, without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for complete details.
// 
// Imported Rev 488/364 1.0 from Retro -> CbmNet
// ---------------------------------------------------------------------------

/*!
  \file
  \version $Id: RosPrintfBase.cpp 20 2013-08-10 16:42:54Z mueller $
  \brief   Implemenation of RosPrintfBase .
*/

#include <sstream>

#include "RosPrintfBase.hpp"

using namespace std;

// all method definitions in namespace CbmNet
namespace CbmNet {

//------------------------------------------+-----------------------------------
/*!
  \relates RosPrintfBase
  \brief string insertion
*/

std::string& operator<<(std::string& os, const RosPrintfBase& obj)
{
  std::ostringstream sos;
  obj.ToStream(sos);
  os += sos.str();
  return os;
}

} // end namespace CbmNet
