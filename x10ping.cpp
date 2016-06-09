////////////////////////////////////////////////////////////////////
// Package:		x10ping implementation
// File:		x10ping.cpp
// Purpose:		program used to monitor and control a device via x10
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:  
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

// system headers
#include <stdio.h>
//#include "stdafx.h"

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// custom header
#include "util/func.h"
#include "logger/logger.h"
#include "config/config.h"
#include "gui/frame.h"
#include "ping.h"
#include "x10.h"
#include "worker.h"

// main header
#include "x10ping.h"

IMPLEMENT_APP(CX10PingApp)

//		***** CLASS CX10PingApp *****
////////////////////////////////////////////////////////////////////
// events table
BEGIN_EVENT_TABLE(CX10PingApp, wxApp)
//  EVT_KEY_DOWN(CX10PingApp::OnKeyDown)
//  EVT_KEY_UP(CX10PingApp::OnKeyUp)
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	On Init
// Class:	CX10PingApp
// Purpose:	initialize mai application
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
bool CX10PingApp::OnInit()
{
	char strMsg[255];
	char strTmp[255];
	m_bStartup = 0;
	
	sprintf( m_strX10PingPath, "%s", "" );

	if( argc > 1 )
	{
		if( strcmp( argv[1], "-s" ) == 0 && argc > 2 )
		{
//			sprintf( strMsg, "DEBUG :arg: found cmd argv = -s %s", argv[2] );

			// set path and build logging path
			sprintf( m_strX10PingPath, "%s", argv[2] );
			m_bStartup = 1;

		} else
		{
//			sprintf( strMsg, "DEBUG :arg: no argument found, argc=%d", argc );
			m_bStartup = 0;
		}
	}

	// Initialize Logger
	m_pLogger = new CLogger( );
	if( m_bStartup == 1 )
	{
		sprintf( strTmp, "%s%s%s", m_strX10PingPath, "\\", X10PING_LOG_FILE );
		m_pLogger->SetLoggingToFile( strTmp );

	} else
	{
		m_pLogger->SetLoggingToFile( X10PING_LOG_FILE );
	}
	// intialize config
	if( m_bStartup == 1 )
	{
		m_pConfig = new CConfig( m_strX10PingPath );
	} else
	{
		m_pConfig = new CConfig( NULL );
	}

	// ping object
	m_pPing = new CPing( );
	m_pPing->SetLogger( m_pLogger );
	// x10 object
	m_pX10 = new CX10( );
//	m_pX10->SetHouseCode( HOUSE_CODE_A );
	m_pX10->m_pLogger = m_pLogger;
	m_pX10->m_pConfig = m_pConfig;
	m_pConfig->m_pX10 = m_pX10;


	// frame initialization
	m_pFrame = new CGUIFrame(NULL, _T("x10ping net monitoring tool"),
								wxPoint(-1, -1), wxSize(600, 400));
	// attach config file to object frame
	m_pFrame->m_pConfig = m_pConfig;
	// get GUI reference for logger
	m_pLogger->SetView( m_pFrame->m_pTextLogger );
	// set x10 object reference
	m_pFrame->m_pX10 = m_pX10;

	// Show the frame
	m_pFrame->Show(TRUE);
	SetTopWindow(m_pFrame);

	// set frame logger as well - after frame inititialization
	m_pFrame->m_pLogger = m_pLogger;

	// init my ping
	m_pPing->Init( );
	// now init my x10
	m_pX10->Init( );

	// initialize working thread
	if( m_bStartup == 1 )
	{
		m_pX10PingWorker = new CX10PingWorker( m_pFrame, m_pLogger, m_pConfig, m_strX10PingPath );
	} else
	{
		m_pX10PingWorker = new CX10PingWorker( m_pFrame, m_pLogger, m_pConfig, NULL );
	}

	if ( m_pX10PingWorker->Create() != wxTHREAD_NO_ERROR )
	{
		m_pLogger->Add( "ERROR :: worker thread initialization failure\n" );
	} else
	{
		m_pLogger->Add( "INFO :: thread initialization ok\n" );
	}
	m_pConfig->m_pX10PingWorker = m_pX10PingWorker;

	// start the thread
	if ( m_pX10PingWorker->Run() != wxTHREAD_NO_ERROR )
	{
		m_pLogger->Add( "ERROR :: worker thread running failure\n" );
	} else
	{
		m_pLogger->Add( "INFO :: worker thread running ok\n" );
	}

	// transmit references
	m_pFrame->SetEngine( m_pX10PingWorker );
	m_pFrame->m_pCX10PingApp = this;
	m_pX10PingWorker->m_pX10PingApp = this;
	m_pX10PingWorker->m_pPing = m_pPing;
	m_pX10PingWorker->m_pX10 = m_pX10;

	// debug
//	m_pLogger->Log( (char*) wxGetCwd().GetData() );
//	m_pLogger->Log( strMsg );

	return TRUE;
};

////////////////////////////////////////////////////////////////////
// Method:	OnExit
// Class:	CX10PingApp
// Purose:	Managing closing all app properly on close event received
//			from various objects
// Input:	nothing
// Output:	status ?
////////////////////////////////////////////////////////////////////
int CX10PingApp::OnExit( )
{
//	int b = -1;

	// uncoment for theaded version
//	m_pX10PingWorker->SaveNetDevices( );

//	if( m_pX10PingWorker->IsDetached() ) 
//		b = 1;
//	else
//		b = 0;
//	m_pX10PingWorker->m_bIsExit = 1;
//	m_pX10PingWorker->DoPause( );
//	while( m_pX10PingWorker->IsRunning() == 1 )
//	{
//		Sleep( 300 );
//	}

	//	wxThread::Sleep( 5500 );

//	m_pX10PingWorker->m_bLife = 0;
//	while( m_pX10PingWorker->m_bIsProcessing == 1 )
//	{
//		wxThread::Sleep( 400 );
//	}

//	m_pX10PingWorker->DoPause( );

//	wxThread::Sleep( 500 );

//	int nCode = (int) m_pX10PingWorker->Wait( );

//	m_pX10PingWorker->Pause( );
//	while( m_pX10PingWorker->IsRunning() == 1 )
//	{
//		Sleep( 300 );
//	}
//	wxThread::Sleep( 200 );
	if( m_pX10PingWorker != NULL ) 
	{
		m_pX10PingWorker->Delete( );
		m_pX10PingWorker = NULL;
	}
//	while( m_pX10PingWorker->IsAlive() == 1 )
//	{
//		Sleep( 300 );
//	}
//	wxThread::Sleep( 500 );
//	m_pX10PingWorker->m_bLife = 0;
//	wxThread::Sleep( 200 );

//	while( m_pX10PingWorker->TestDestroy() == false )
//	{
//		Sleep( 400 );
//	}

//	m_pX10PingWorker->Wait( );

	//	while( m_pX10PingWorker->Wait( ) ) 
//	{
//		Sleep( 100 );
//	}
//	delete( m_pX10PingWorker );


//	m_pConfig->Save( );
//	wxThread::Sleep( 5000 );

	delete( m_pX10 );
	delete( m_pPing );
	delete( m_pConfig );
	delete( m_pLogger );
//	delete( m_pFrame );

//	delete( m_pX10PingWorker );

	m_pX10 = NULL;
	m_pPing = NULL;
	m_pConfig = NULL;
	m_pLogger = NULL;

//	Exit( );
	return( true );
}
