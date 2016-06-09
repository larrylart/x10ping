////////////////////////////////////////////////////////////////////
// Package:		x10ping definition
// File:		x10ping.h
// Purpose:		program used to monitor and control a device via x10
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:  
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _X10PING_H
#define _X10PING_H

#define X10PING_LOG_FILE	"./x10ping.log"

class CGUIFrame;
class CConfig;
class CLogger;
class CPing;
class CX10;
class CX10PingWorker;

#include "wx/wxprec.h"
#include "wx/thread.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// house id to letter mappings
const wxString m_vectStrX10House[16] = { "A", "B", "C", "D", "E", "F", "G",
				"H", "I", "J", "K", "L", "M", "N", "O", "P" };

/////////////////////////////////////////////////
// class:	CX10PingApp
////////////////////////////////////////////////
class CX10PingApp: public wxApp
{

// public data
public:
	CX10			*m_pX10;
	CPing			*m_pPing;
	CConfig			*m_pConfig;
	CLogger			*m_pLogger;
	CGUIFrame		*m_pFrame;
	CX10PingWorker	*m_pX10PingWorker;

	// x10ping path
	char m_strX10PingPath[255];

	// other flags
	int m_bStartup;

// private methods/data
protected:
//    void OnKeyDown( wxKeyEvent& event );
//	void OnKeyUp( wxKeyEvent& event );
	bool OnInit( );
	int OnExit( );
//	virtual int OnRun( );

	DECLARE_EVENT_TABLE()
};


#endif
