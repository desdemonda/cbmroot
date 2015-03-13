// $Id: RlogMsg.ipp 20 2013-08-10 16:42:54Z mueller $
//
// Copyright 2013- by Walter F.J. Mueller <W.F.J.Mueller@gsi.de>
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
// Imported Rev 490/490 1.0 from Retro -> CbmNet
// ---------------------------------------------------------------------------

/*!
  \file
  \version $Id: RlogMsg.ipp 20 2013-08-10 16:42:54Z mueller $
  \brief   Implemenation (inline) of RlogMsg.
*/

// all method definitions in namespace CbmNet
namespace CbmNet {

//------------------------------------------+-----------------------------------
//! FIXME_docs

inline void RlogMsg::SetTag(char tag)
{
  fTag = tag;
  return;
}

//------------------------------------------+-----------------------------------
//! FIXME_docs

inline void RlogMsg::SetString(const std::string& str)
{
  fStream.str(str);
  return;
}

//------------------------------------------+-----------------------------------
//! FIXME_docs

inline char RlogMsg::Tag() const
{
  return fTag;
}

//------------------------------------------+-----------------------------------
//! FIXME_docs

inline std::string RlogMsg::String() const
{
  return fStream.str();
}

//------------------------------------------+-----------------------------------
//! FIXME_docs

inline std::ostream& RlogMsg::operator()()
{
  return fStream;
}

//------------------------------------------+-----------------------------------
/*! 
  \relates RlogMsg
  \brief FIXME_docs
*/

template <class T>
inline std::ostream& operator<<(RlogMsg& lmsg, const T& val)
{
  lmsg() << val;
  return lmsg();
}

} // end namespace CbmNet
