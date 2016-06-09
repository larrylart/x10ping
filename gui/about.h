////////////////////////////////////////////////////////////////////
// Package:		GUI About definition
// File:		about.h
// Purpose:		define about window
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:  
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _ABOUT_H 
#define _ABOUT_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// class:	CNnroAbout
///////////////////////////////////////////////////////
class CX10PingAbout : public wxDialog
{
// methods
public:
    CX10PingAbout(wxWindow *parent);
};


#endif
