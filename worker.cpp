////////////////////////////////////////////////////////////////////
// Package:		worker implementation
// File:		worker.cpp
// Purpose:		manage checking, switching tasks
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:	Larry Lart on 01-Aug-2006
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

// system headers
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "wx/wxprec.h"
#include "wx/thread.h"
#include <wx/string.h>
#include <wx/regex.h>
#include <wx/sound.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// custom headers
#ifdef WIN32
#include "util/func.h"
#endif

#include "ping.h"
#include "x10.h"
#include "logger/logger.h"
#include "gui/frame.h"
#include "config/config.h"
#include "network/netdevice.h"
#include "x10ping.h"
#include "util/smtp.h"

// main header include
#include "worker.h"

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CX10PingWorker
// Purpose:	Initialize my worker thread
// Input:	pointer to reference frame
// Output:	nothing	
////////////////////////////////////////////////////////////////////
CX10PingWorker::CX10PingWorker( CGUIFrame *pFrame, CLogger *pLogger, 
								CConfig *pConfig, char* strPath ) : wxThread( wxTHREAD_DETACHED )
{ 
	// logger reference
	m_pLogger = pLogger;
	// get frame refrence
	m_pFrame = pFrame;
	// get config refrence
	m_pConfig = pConfig;
	
	m_nDevice = 0;
	m_bIsJustStarted = 1;

	// process path if any
	//NET_DEVICE_CONFIG_FILE_NAME
	if( strPath != NULL ) 
	{
		sprintf( m_strNetDevConfigFile, "%s%s%s", strPath, "\\", NET_DEVICE_CONFIG_FILE_NAME );
	} else
	{
		sprintf( m_strNetDevConfigFile, "%s", NET_DEVICE_CONFIG_FILE_NAME );
	}

//	for( int i=0; i<MAX_DEV_NUMBER; i++ )
//	{
//		m_vectDeviceCounter[i] = 0;
//		m_vectDeviceHouseId[i] = -1;
//		m_vectDeviceUnitId[i] = -1;
//
//		m_vectCheckMethod[i] = -1;
//		m_vectFailuresToAction[i] = -1;
//		m_vectActionLogic[i] = -1;
//		m_vectActionFunction[i] = -1;
//		m_vectRebootTime[i] = -1;
//
//		m_vectCheckTimeFrame[i] = -1;
//		m_vectCheckStartTimeH[i] = -1;
//		m_vectCheckStartTimeM[i] = -1;
//		m_vectCheckEndTimeH[i] = -1;
//		m_vectCheckEndTimeM[i] = -1;
//	}

	// start life
	m_bLife = 1;

	// default stoped
	m_bIsMonitorStarted = 0;
	m_bIsPaused = 1;
	m_bPauseLoop = 1;

	m_bChanged = 0;

	m_bEmailAuth = 0;
	m_bIsProcessing = 0;
	m_bIsExit == 0;

	m_pMutex = new wxMutex( );

	return;
}
////////////////////////////////////////////////////////////////////
// Method:	On Exit
// Class:	CX10PingWorker
// Purpose:	do soemthing on exit
// Input:	nothing
// Output:	nothing	
////////////////////////////////////////////////////////////////////
CX10PingWorker::~CX10PingWorker( )
{
/*	int i = 0;

	// destroy - clean my place
	delete( m_pMutex );

	// delete my devices
	for( i = 0; i < m_nDevice; i++ )
	{
		delete( m_vectNetDevice[i] );
		m_vectNetDevice[i] = NULL;
	}
*/
	m_pX10 = NULL;
	m_pLogger = NULL;
	m_pX10PingApp =  NULL;
	m_pFrame = NULL;
	m_pConfig = NULL;
	m_pPing = NULL;
	
}

////////////////////////////////////////////////////////////////////
// Method:	On Exit
// Class:	CX10PingWorker
// Purpose:	do soemthing on exit
// Input:	nothing
// Output:	nothing	
////////////////////////////////////////////////////////////////////
void CX10PingWorker::OnExit( )
{
//	int i = 0;
//
//	// destroy - clean my place
//	delete( m_pMutex );
//
//	// delete my devices
//	for( i = 0; i < m_nDevice; i++ )
//	{
//		delete( m_vectNetDevice[i] );
//		m_vectNetDevice[i] = NULL;
//	}
	int i = 0;

	// destroy - clean my place
	delete( m_pMutex );

	// delete my devices
	for( i = 0; i < m_nDevice; i++ )
	{
		delete( m_vectNetDevice[i] );
		m_vectNetDevice[i] = NULL;
	}

//	Exit( 0 );
	return;
}


////////////////////////////////////////////////////////////////////
// Method:	Update Config
// Class:	CX10PingWorker
// Purose:	update configuration variables
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CX10PingWorker::UpdateConfig( )
{
	// check if valid reference
	if( !m_pConfig ) return;
	int bChange = 0;

	// get serial port number
	if( m_pConfig->GetIntVar( SECTION_ID_NETWORK, CONF_NETWORK_LOOP_CHECK_DELAY, m_nLoopCheckDelay ) == 0 )
	{
		m_nLoopCheckDelay = NETWORK_DEFAULT_LOOP_CHECK_DELAY;
		m_pConfig->SetIntVar( SECTION_ID_NETWORK, CONF_NETWORK_LOOP_CHECK_DELAY, m_nLoopCheckDelay );
		bChange = 1;
	}
	// get controler type
	if( m_pConfig->GetIntVar( SECTION_ID_NETWORK, CONF_DEVICE_PING_RETRY, m_nRetry ) == 0 )
	{
		m_nRetry = NETWORK_DEFAULT_NO_RETRY;
		m_pConfig->SetIntVar( SECTION_ID_NETWORK, CONF_DEVICE_PING_RETRY, m_nRetry );
		bChange = 1;
	}
	// X 10 Delay After OFF
	if( m_pConfig->GetIntVar( SECTION_ID_NETWORK, CONF_NETWORK_X10_DELAY_AFTER_OFF, m_nX10DelayAfterOFF ) == 0 )
	{
		m_nX10DelayAfterOFF = X10_DELAY_AFTER_OFF;
		m_pConfig->SetIntVar( SECTION_ID_NETWORK, CONF_NETWORK_X10_DELAY_AFTER_OFF, m_nX10DelayAfterOFF );
		bChange = 1;
	}
	// X 10 Delay After ON
	if( m_pConfig->GetIntVar( SECTION_ID_NETWORK, CONF_NETWORK_X10_DELAY_AFTER_ON, m_nX10DelayAfterON ) == 0 )
	{
		m_nX10DelayAfterON = X10_DELAY_AFTER_ON;
		m_pConfig->SetIntVar( SECTION_ID_NETWORK, CONF_NETWORK_X10_DELAY_AFTER_ON, m_nX10DelayAfterON );
		bChange = 1;
	}

	// runtime :: monitor on startup
	if( m_pConfig->GetIntVar( SECTION_ID_RUNTIME, CONF_MONITOR_ON_STARTUP, m_bIsMonitorStarted ) == 0 )
	{
		// if no config values do defaults
		m_bIsMonitorStarted = 0;
		m_pConfig->SetIntVar( SECTION_ID_RUNTIME, CONF_MONITOR_ON_STARTUP, m_bIsMonitorStarted );
		bChange = 1;
	} else
	{
		if( m_bIsMonitorStarted == 1 )
		{
			m_bIsPaused = 0;
			m_bPauseLoop = 0;
			m_pFrame->m_pStartMonButton->Enable( 0 );
			m_pFrame->m_pStopMonButton->Enable( 1 );

			if( m_bIsJustStarted == 1 )
			{
				// do iconize on startup

				//wxIconizeEvent event( GetId() );
				//event.iconize = 0;
				//event.SetEventObject( m_pFrame );
				m_pFrame->Iconize(1);
				//m_pFrame->GetEventHandler()->ProcessEvent( event );
				m_bIsJustStarted = 0;
			}

		} else
		{
			m_bIsPaused = 1;
			m_bPauseLoop = 1;
			m_pFrame->m_pStartMonButton->Enable( 1 );
			m_pFrame->m_pStopMonButton->Enable( 0 );

		}
	}

	// get email server address if any
	if( m_pConfig->GetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_SERVER, m_strEmailServer ) == 0 )
	{
		sprintf( m_strEmailServer, "%s", "" );
		m_pConfig->SetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_SERVER, m_strEmailServer );
		m_bEmailServer = 0;
	} else
	{
		m_bEmailServer = 1;
	}

	if( m_pConfig->GetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_FROM, m_strEmailFrom ) == 0 )
	{
		sprintf( m_strEmailFrom, "%s", "" );
		m_pConfig->SetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_FROM, m_strEmailServer );
		m_bEmailServer = 0;
	} else
	{
		m_bEmailServer = 1;
	}

	if( m_pConfig->GetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_TO, m_strEmailTo ) == 0 )
	{
		sprintf( m_strEmailTo, "%s", "" );
		m_pConfig->SetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_TO, m_strEmailServer );
		m_bEmailServer = 0;
	} else
	{
		m_bEmailServer = 1;
	}


	// get email auth flag if any
	if( m_pConfig->GetIntVar( SECTION_ID_NOTIFY, CONF_EMAIL_AUTH, m_bEmailAuth ) == 0 )
	{
		m_bEmailAuth = 0;
		m_pConfig->SetIntVar( SECTION_ID_NOTIFY, CONF_EMAIL_AUTH, m_bEmailAuth );

	} else
	{
		if( m_pConfig->GetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_AUTH_USER, m_strEmailAuthUser ) == 0 )
		{
			sprintf( m_strEmailAuthUser, "%s", "" );
			m_pConfig->SetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_AUTH_USER, m_strEmailAuthUser );
			m_bEmailAuth = 0;
		}

		if( m_pConfig->GetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_AUTH_PASSWD, m_strEmailAuthPasswd ) == 0 )
		{
			sprintf( m_strEmailAuthPasswd, "%s", "" );
			m_pConfig->SetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_AUTH_PASSWD, m_strEmailAuthPasswd );
			m_bEmailAuth = 0;
		}

	}

	// get audio notification file name - m_strNotifyAudioFile
	if( m_pConfig->GetStrVar( SECTION_ID_NOTIFY, CONF_AUDIO_NOTIFY_FILE, m_strNotifyAudioFile ) == 0 )
	{
		sprintf( m_strNotifyAudioFile, "%s%s", m_pX10PingApp->m_strX10PingPath, "./bunny_troubles.wav" );
		m_pConfig->SetStrVar( SECTION_ID_NOTIFY, CONF_AUDIO_NOTIFY_FILE, m_strNotifyAudioFile );
		m_bAudioNotification = 0;
	} else
	{
		m_bAudioNotification = 1;
	}

	// mark change 
	IsChanged( 1 );

	// need to reset the counter on config change
	for( int i=0; i<m_nDevice; i++ )
	{
//		m_vectDeviceCounter[i] = 0;
		m_vectNetDevice[i]->SetCounter( 0 );
//		m_vectDeviceHouseId[i] = -1;
//		m_vectDeviceUnitId[i] = -1;
	}


	// check if default updates then save config
	if( bChange > 0 ) m_pConfig->Save( );

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	Save Net Devices
// Class:	CX10PingWorker
// Purpose:	save my my list of net devices and their config
// Input:	nothing
// Output:	status
/////////////////////////////////////////////////////////////////////
int CX10PingWorker::SaveNetDevices( const char* strNetDevConfigFile )
{
	FILE* pFile = NULL;
	unsigned int i = 0;
	int nVarValue = 0;

	if( strcmp( strNetDevConfigFile, "") != 0 )
		strcpy( m_strNetDevConfigFile, strNetDevConfigFile );

	// open config file to write
	pFile = fopen( m_strNetDevConfigFile, "w" );
	// check if there is any configuration to load
	if( !pFile ) return( 0 );

	// for every device
	for( i=0; i<m_nDevice; i++ )
	{
		// print header =  device net address
		fprintf( pFile, "\n\n\[%s\]\n", m_vectNetDevice[i]->m_strName );
		fprintf( pFile, "X10HouseId=%d\n", m_vectNetDevice[i]->m_nHouseId );
		fprintf( pFile, "X10DeviceId=%d\n", m_vectNetDevice[i]->m_nUnitId );
		fprintf( pFile, "LastKnownNetDeviceState=%d\n", m_vectNetDevice[i]->m_nUnitState );

		fprintf( pFile, "DeviceCheckMethod=%d\n", m_vectNetDevice[i]->m_nCheckMethod );
		fprintf( pFile, "FailuresToAction=%d\n", m_vectNetDevice[i]->m_nFailuresToAction );
		fprintf( pFile, "ActionLogic=%d\n", m_vectNetDevice[i]->m_nActionLogic );
		fprintf( pFile, "ActionFunction=%d\n", m_vectNetDevice[i]->m_nActionFunction );
		fprintf( pFile, "ActionNotify=%d\n", m_vectNetDevice[i]->m_nActionNotify );
		fprintf( pFile, "RebootTime=%d\n", m_vectNetDevice[i]->m_nRebootTime );

		fprintf( pFile, "CheckTimeFrame=%d\n", m_vectNetDevice[i]->m_nCheckTimeFrame );

		fprintf( pFile, "CheckStartTime=%.2d:%.2d\n", m_vectNetDevice[i]->m_nCheckStartTimeH,
							m_vectNetDevice[i]->m_nCheckStartTimeM );

		fprintf( pFile, "CheckEndTime=%.2d:%.2d\n", m_vectNetDevice[i]->m_nCheckEndTimeH,
							m_vectNetDevice[i]->m_nCheckEndTimeM );

	}
	// close my file handler
	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	LoadNetDevices
// Class:	CX10PingWorker
// Purpose:	load my net device from file f they exist
// Input:	nothing
// Output:	status
/////////////////////////////////////////////////////////////////////
int CX10PingWorker::LoadNetDevices( const char* strNetDevConfigFile )
{
	FILE* pFile = NULL;
	char strLine[2000];
	char strVarName[256];
	char strVarValue[256];
	int bFound = 0;
	int i = 0;

	// define my regex 
	wxRegEx reSection( "\\[([a-zA-Z0-9_\\-\\.]+)\\]" );
	wxRegEx reVariable( "([a-zA-Z0-9_\\-]+)\\ *=\\ *([\\:\\-\\+\\.0-9]+)" );
	wxRegEx reTime( "([0-9]+)\\:([0-9]+)" );


	if( strcmp( strNetDevConfigFile, "") != 0 )
		strcpy( m_strNetDevConfigFile, strNetDevConfigFile );

	// now clear all existing devices
	// delete my devices
	for( i = 0; i < m_nDevice; i++ )
	{
		delete( m_vectNetDevice[i] );
		m_vectNetDevice[i] = NULL;
	}
	m_nDevice = 0;
	ClearGuiDeviceList( );

	// Read the index
	pFile = fopen( m_strNetDevConfigFile, "r" );

	// check if there is any configuration to load
	if( !pFile ) return( 0 );

	// Reading lines from cfg file
	while( !feof( pFile ) )
	{
		// clear thy buffers
		bzero( strLine, 2000 );
		bzero( strVarName, 256 );
		bzero( strVarValue, 256 );

		// get one line out of the config file
		fgets( strLine, 2000, pFile );

		// check if section start
		if( reSection.Matches( strLine ) )
		{
			if( m_nDevice >= MAX_DEV_NUMBER )  return( -1 );

			if( bFound == 1 ) DisplayGuiDeviceList( );

			// create new device
			m_vectNetDevice[m_nDevice] = new CNetDevice( this );

			wxString strSection = reSection.GetMatch(strLine, 1 );
			strcpy( m_vectNetDevice[m_nDevice]->m_strName, strSection.GetData() );
			m_vectNetDevice[m_nDevice]->m_nUnitId = 0;
			m_vectNetDevice[m_nDevice]->m_nHouseId = -1;
			m_vectNetDevice[m_nDevice]->m_nUnitState = DEVICE_STATE_DOWN;
			m_vectNetDevice[m_nDevice]->m_nActionNotify = -1;
			m_vectNetDevice[m_nDevice]->SetCounter( 0 );

			m_nDevice++;
			bFound = 1;

		} else if( bFound == 1 && reVariable.Matches( strLine ) )
		{
			wxString strVarName = reVariable.GetMatch(strLine, 1 );
			wxString strVarValue = reVariable.GetMatch(strLine, 2 );

			if( strVarName.CmpNoCase( "LastKnownNetDeviceState" ) == 0 )
			{
				m_vectNetDevice[m_nDevice-1]->m_nUnitState = atoi( strVarValue );
			} else if( strVarName.CmpNoCase( "X10DeviceId" ) == 0 ) 
			{
				m_vectNetDevice[m_nDevice-1]->m_nUnitId = atoi( strVarValue );
			} else if( strVarName.CmpNoCase( "X10HouseId" ) == 0 ) 
			{
				m_vectNetDevice[m_nDevice-1]->m_nHouseId = atoi( strVarValue );
			} else if( strVarName.CmpNoCase( "DeviceCheckMethod" ) == 0 ) 
			{
				m_vectNetDevice[m_nDevice-1]->m_nCheckMethod = atoi( strVarValue );
			} else if( strVarName.CmpNoCase( "FailuresToAction" ) == 0 ) 
			{
				m_vectNetDevice[m_nDevice-1]->m_nFailuresToAction = atoi( strVarValue );
			} else if( strVarName.CmpNoCase( "ActionLogic" ) == 0 ) 
			{
				m_vectNetDevice[m_nDevice-1]->m_nActionLogic = atoi( strVarValue );
			} else if( strVarName.CmpNoCase( "ActionFunction" ) == 0 ) 
			{
				m_vectNetDevice[m_nDevice-1]->m_nActionFunction = atoi( strVarValue );
			} else if( strVarName.CmpNoCase( "ActionNotify" ) == 0 ) 
			{
				m_vectNetDevice[m_nDevice-1]->m_nActionNotify = atoi( strVarValue );
			} else if( strVarName.CmpNoCase( "RebootTime" ) == 0 ) 
			{
				m_vectNetDevice[m_nDevice-1]->m_nRebootTime = atoi( strVarValue );
			} else if( strVarName.CmpNoCase( "CheckTimeFrame" ) == 0 ) 
			{
				m_vectNetDevice[m_nDevice-1]->m_nCheckTimeFrame = atoi( strVarValue );
			} else if( strVarName.CmpNoCase( "CheckStartTime" ) == 0 ) 
			{
				if( reTime.Matches( strVarValue ) )
				{
					wxString strHour = reTime.GetMatch(strVarValue, 1 );
					wxString strMinute = reTime.GetMatch(strVarValue, 2 );
					m_vectNetDevice[m_nDevice-1]->m_nCheckStartTimeH = atoi( strHour );
					m_vectNetDevice[m_nDevice-1]->m_nCheckStartTimeM = atoi( strMinute );
				} else
				{
					m_vectNetDevice[m_nDevice-1]->m_nCheckStartTimeH = 0;
					m_vectNetDevice[m_nDevice-1]->m_nCheckStartTimeM = 0;
				}
			} else if( strVarName.CmpNoCase( "CheckEndTime" ) == 0 ) 
			{
				if( reTime.Matches( strVarValue ) )
				{
					wxString strHour = reTime.GetMatch(strVarValue, 1 );
					wxString strMinute = reTime.GetMatch(strVarValue, 2 );
					m_vectNetDevice[m_nDevice-1]->m_nCheckEndTimeH = atoi( strHour );
					m_vectNetDevice[m_nDevice-1]->m_nCheckEndTimeM = atoi( strMinute );
				} else
				{
					m_vectNetDevice[m_nDevice-1]->m_nCheckEndTimeH = 0;
					m_vectNetDevice[m_nDevice-1]->m_nCheckEndTimeM = 0;
				}
			}

		}
	}

	//if( bFound == 1 ) 
	DisplayGuiDeviceList( );

	// close file
	fclose( pFile );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DisplayGuiDeviceList
// Class:	CX10PingWorker
// Purpose:	display device list in gui
// Input:	nothing
// Output:	nothing	
////////////////////////////////////////////////////////////////////
void CX10PingWorker::DisplayGuiDeviceList( )
{
	int i = 0;

	// now tell gui to add this to the list as well
	for( i=0; i<m_nDevice; i++ )
	{
		m_pFrame->SetDevice( i, m_vectNetDevice[i]->m_strName,
							m_vectNetDevice[i]->m_nHouseId,
							m_vectNetDevice[i]->m_nUnitId, 
							m_vectNetDevice[i]->m_nUnitState );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	ClearGuiDeviceList
// Class:	CX10PingWorker
// Purpose:	clear display device list in gui
// Input:	nothing
// Output:	nothing	
////////////////////////////////////////////////////////////////////
void CX10PingWorker::ClearGuiDeviceList( )
{
	int i = 0;

	// now tell gui to add this to the list as well
	for( i=0; i<MAX_DEV_NUMBER; i++ )
	{
		m_pFrame->ClearDevice( i );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	UpdateDeviceState
// Class:	CX10PingWorker
// Purpose:	update my device state here and in gui if no change flag
// Input:	device list id, state
// Output:	nothing	
////////////////////////////////////////////////////////////////////
int CX10PingWorker::UpdateDeviceState( int nDeviceIndex, int nDeviceState )
{
	if( m_bChanged == 1 || m_bIsExit == 1 ) return( 0 );

	if( nDeviceState != DEVICE_STATE_UP && nDeviceState != DEVICE_STATE_DOWN )
	{
		m_vectNetDevice[nDeviceIndex]->m_nUnitState = nDeviceState;
		m_pFrame->SetDeviceState( nDeviceIndex, nDeviceState );
	} else
	{
			switch( m_vectNetDevice[nDeviceIndex]->m_nActionLogic )
			{
				case DEVICE_CHECK_LOGIC_OFF:
				{
					m_vectNetDevice[nDeviceIndex]->SetState( nDeviceState );
					m_pFrame->SetDeviceState( nDeviceIndex, nDeviceState );
					break;
				}

				case DEVICE_CHECK_LOGIC_ON:
				{
					if( nDeviceState == DEVICE_STATE_DOWN )
					{
						m_vectNetDevice[nDeviceIndex]->SetState( DEVICE_STATE_UP );
						m_pFrame->SetDeviceState( nDeviceIndex, DEVICE_STATE_UP );
						
					} else if( nDeviceState == DEVICE_STATE_UP )
					{
						m_vectNetDevice[nDeviceIndex]->SetState( DEVICE_STATE_DOWN );
						m_pFrame->SetDeviceState( nDeviceIndex, DEVICE_STATE_DOWN );
					}
					break;
				}

				case DEVICE_CHECK_LOGIC_SLOW:
				{
					m_vectNetDevice[nDeviceIndex]->SetState( nDeviceState );
					m_pFrame->SetDeviceState( nDeviceIndex, nDeviceState );
					break;
				}

				default: break;
			}

	}

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	AddDevice
// Class:	CX10PingWorker
// Purpose:	add device to the list to monitor
// Input:	device name, device id
// Output:	status	
////////////////////////////////////////////////////////////////////
int CX10PingWorker::AddDevice( const char *strName,unsigned int nHouseId, 
							  unsigned int nUnitId, 
							  int nCheckMethod, int nFailuresToAction, 
							   int nActionLogic, int nActionFunction, 
							   int nActionNotify,
							   int nRebootTime, int bCheckTimeFrame, 
							   int nCheckStartTimeH, int nCheckStartTimeM, 
							   int nCheckEndTimeH, int nCheckEndTimeM )
{
	int nStatus = 0;

	if( m_nDevice >= MAX_DEV_NUMBER )  return( -1 );

	// create new device
	m_vectNetDevice[m_nDevice] = new CNetDevice( this );

	// edit device
	m_vectNetDevice[m_nDevice]->Edit( strName, nHouseId, nUnitId, 
							   nCheckMethod, nFailuresToAction, 
							   nActionLogic, nActionFunction, 
							   nActionNotify, 
							   nRebootTime, bCheckTimeFrame, 
							   nCheckStartTimeH, nCheckStartTimeM, 
							   nCheckEndTimeH, nCheckEndTimeM );

	// set state and counter
	m_vectNetDevice[m_nDevice]->SetState( DEVICE_STATE_DOWN );
	m_vectNetDevice[m_nDevice]->SetCounter( 0 );
	
	m_nDevice++;

	// re-display device list in gui
	DisplayGuiDeviceList( );
	// do quick save devices to file
//	SaveNetDevices( "" );

	return( nStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	GetDeviceData
// Class:	CX10PingWorker
// Purpose:	get data about a device list id
// Input:	device list id, reference to name, device id
// Output:	status	
////////////////////////////////////////////////////////////////////
int CX10PingWorker::GetDeviceData( unsigned int nListId, char* strDevName,
									int& nHousId, int& nDevId, int& nCheckMethod,
									int& nFailuresToAction, int& nActionLogic,
									int& nActionFunction, int& nActionNotify, int& nRebootTime, 
									int& bCheckTimeFrame, int& nCheckStartTimeH,
									int& nCheckStartTimeM, int& nCheckEndTimeH, 
									int& nCheckEndTimeM )
{
	if( nListId >= m_nDevice )  return( 0 );

	strcpy( strDevName, m_vectNetDevice[nListId]->m_strName );
	nDevId = m_vectNetDevice[nListId]->m_nUnitId;
	nHousId = m_vectNetDevice[nListId]->m_nHouseId;

	nCheckMethod = m_vectNetDevice[nListId]->m_nCheckMethod;
	nFailuresToAction = m_vectNetDevice[nListId]->m_nFailuresToAction;
	nActionLogic = m_vectNetDevice[nListId]->m_nActionLogic;
	nActionFunction = m_vectNetDevice[nListId]->m_nActionFunction;
	nActionNotify = m_vectNetDevice[nListId]->m_nActionNotify;
	nRebootTime = m_vectNetDevice[nListId]->m_nRebootTime;

	bCheckTimeFrame = m_vectNetDevice[nListId]->m_nCheckTimeFrame;
	nCheckStartTimeH = m_vectNetDevice[nListId]->m_nCheckStartTimeH;
	nCheckStartTimeM = m_vectNetDevice[nListId]->m_nCheckStartTimeM;
	nCheckEndTimeH = m_vectNetDevice[nListId]->m_nCheckEndTimeH;
	nCheckEndTimeM = m_vectNetDevice[nListId]->m_nCheckEndTimeM;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	EditDevice
// Class:	CX10PingWorker
// Purpose:	edit and existing device id
// Input:	device list id, pointer to name, device id
// Output:	status	
////////////////////////////////////////////////////////////////////
int CX10PingWorker::EditDevice( unsigned int nListId, const char *strDevName, 
							   unsigned int nHouseId, unsigned int nUnitId, 
							   int nCheckMethod, int nFailuresToAction, 
							   int nActionLogic, int nActionFunction, 
							   int nActionNotify,
							   int nRebootTime, int bCheckTimeFrame, 
							   int nCheckStartTimeH, int nCheckStartTimeM, 
							   int nCheckEndTimeH, int nCheckEndTimeM )
{
	if( nListId >= m_nDevice )  return( 0 );
	char strMsg[255];

	// edit device
	m_vectNetDevice[nListId]->Edit( strDevName, nHouseId, nUnitId, 
							   nCheckMethod, nFailuresToAction, 
							   nActionLogic, nActionFunction, 
							   nActionNotify, 
							   nRebootTime, bCheckTimeFrame, 
							   nCheckStartTimeH, nCheckStartTimeM, 
							   nCheckEndTimeH, nCheckEndTimeM );

	if( m_bIsPaused == 0 ) IsChanged( 1 );

	// re-display list
	DisplayGuiDeviceList( );

	// do quick save devices to file
//	SaveNetDevices( "" );

	sprintf( strMsg, "INFO :worker: edited device name=%s", strDevName );
	m_pLogger->Log( strMsg );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	DeleteDevice
// Class:	CX10PingWorker
// Purpose:	delete device from the monitoring monitor
// Input:	device name, device id
// Output:	status	
////////////////////////////////////////////////////////////////////
int CX10PingWorker::DeleteDevice( unsigned int nListId )
{
	int i = 0;
	char strMsg[255];

	if( nListId < 0 || nListId >= m_nDevice )
	{
		sprintf( strMsg, "INFO :worker: could not delete device=%d out of limit( %d, %d )", 
							nListId, 0, m_nDevice );
		m_pLogger->Log( strMsg );
		return( 0 );
	}

	int nCurrentDevice = nListId;

	// shift up list from current position
	for( i=nCurrentDevice+1; i<m_nDevice; i++ )
	{
		// edit device
		m_vectNetDevice[i-1]->Edit( m_vectNetDevice[i]->m_strName, 
								m_vectNetDevice[i]->m_nHouseId, 
								m_vectNetDevice[i]->m_nUnitId, 
							   m_vectNetDevice[i]->m_nCheckMethod, 
							   m_vectNetDevice[i]->m_nFailuresToAction, 
							   m_vectNetDevice[i]->m_nActionLogic, 
							   m_vectNetDevice[i]->m_nActionFunction, 
							   m_vectNetDevice[i]->m_nActionNotify,
							   m_vectNetDevice[i]->m_nRebootTime, 
							   m_vectNetDevice[i]->m_nCheckTimeFrame, 
							   m_vectNetDevice[i]->m_nCheckStartTimeH, 
							   m_vectNetDevice[i]->m_nCheckStartTimeM, 
							   m_vectNetDevice[i]->m_nCheckEndTimeH, 
							   m_vectNetDevice[i]->m_nCheckEndTimeM );
	
	
		m_vectNetDevice[i-1]->SetState( m_vectNetDevice[i]->m_nUnitState );
		m_vectNetDevice[i-1]->SetCounter( m_vectNetDevice[i]->m_nCounter );

	}

	m_nDevice--;
	// delete and set pervious last device to null
	delete( m_vectNetDevice[m_nDevice] );
	m_vectNetDevice[m_nDevice] = NULL;
	// mark as changed
	IsChanged( 1 );

	// re-display list
	DisplayGuiDeviceList( );

	// do quick save devices to file
//	SaveNetDevices( "" );

	sprintf( strMsg, "INFO :worker: deleteted device name=%s", 
						m_vectNetDevice[nListId]->m_strName );
	m_pLogger->Log( strMsg );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		DoPause
// Class:		CX10PingWorker
// Purpose:		set pause loop flag and wait to confirm
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CX10PingWorker::DoPause( )
{
	m_bPauseLoop = 1;

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		DoStart
// Class:		CX10PingWorker
// Purpose:		set pause loop flag and wait to 0 start
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CX10PingWorker::DoStart( )
{
	m_bPauseLoop = 0;
	m_bIsPaused = 0;
	for( int i=0; i<m_nDevice; i++ )
	{
		m_vectNetDevice[i]->SetCounter( 0 );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		IsChanged
// Class:		CX10PingWorker
// Purpose:		if 0 change over if 1 changed
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CX10PingWorker::IsChanged( int nState )
{
	m_pMutex->Lock();
	m_bChanged = nState;
	// reset counters to zero when change done = 0
	m_pMutex->Unlock( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		TurnDeviceOn
// Class:		CX10PingWorker
// Purpose:		turn a device on and set state to on
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CX10PingWorker::TurnDeviceOn( unsigned int nListId )
{
	char strMsg[255];

	// check if device list out of range
	if( nListId >= m_nDevice ) return( 0 );

	// check if x10 sent off command ok, else return 0
	if( m_pX10->TurnUnitOn( m_vectNetDevice[nListId]->m_nHouseId, 
							m_vectNetDevice[nListId]->m_nUnitId ) == 0 ) return( 0 );
	// update satus to off
	if( UpdateDeviceState( nListId, DEVICE_STATE_SWITCHED_ON ) == 0 ) return( 0 );

	sprintf( strMsg, "INFO :worker: device name=%s is now switched ON", 
						m_vectNetDevice[nListId]->m_strName );
	m_pLogger->Log( strMsg );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		TurnDeviceOff
// Class:		CX10PingWorker
// Purpose:		turn a device on and set state to on
// Input:		nothing
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CX10PingWorker::TurnDeviceOff( unsigned int nListId )
{
	char strMsg[255];

	// check if device list out of range
	if( nListId >= m_nDevice ) return( 0 );

	// check if x10 sent off command ok, else return 0
	if( m_pX10->TurnUnitOff( m_vectNetDevice[nListId]->m_nHouseId, 
							m_vectNetDevice[nListId]->m_nUnitId ) == 0 ) return( 0 );
	// update satus to off
	if( UpdateDeviceState( nListId, DEVICE_STATE_SWITCHED_OFF ) == 0 ) return( 0 );

	sprintf( strMsg, "INFO :worker: device name=%s is now switched OFF", 
						m_vectNetDevice[nListId]->m_strName );
	m_pLogger->Log( strMsg );

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:		CheckDevice
// Class:		CX10PingWorker
// Purpose:		check a device by method selected
// Input:		device id
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CX10PingWorker::CheckDevice( unsigned int nDeviceId )
{
	if( m_bIsExit == 1 ) return( 0 );

	int nStatus = 0;

	// by checking method type
	switch( m_vectNetDevice[nDeviceId]->m_nCheckMethod )
	{
		case DEVICE_CHECK_METHOD_PING:
		{
			int nRet = m_pPing->Ping( m_vectNetDevice[nDeviceId]->m_strName );
			// check return by logic
			switch( m_vectNetDevice[nDeviceId]->m_nActionLogic )
			{
				case DEVICE_CHECK_LOGIC_OFF:
				{
					if( nRet == 0 )
						nStatus = 0;
					else if( nRet != 0 )
						nStatus = 1;
					break;
				}

				case DEVICE_CHECK_LOGIC_ON:
				{
					if( nRet == 0 )
						nStatus = 1;
					else if( nRet != 0 )
						nStatus = 0;
					break;
				}

				case DEVICE_CHECK_LOGIC_SLOW:
				{
					if( nRet == 0 )
						nStatus = 0;
					else if( nRet != 0 )
						nStatus = 1;
					break;
				}

				default: break;
			}

			break;
		}

		case DEVICE_CHECK_METHOD_SOCKET:
		{
			break;
		}

		case DEVICE_CHECK_METHOD_EXT_PROG:
		{
			break;
		}

		default: break;
	}

	return( nStatus );
}

////////////////////////////////////////////////////////////////////
// Method:		IsInTimeFrame
// Class:		CX10PingWorker
// Purpose:		check if a device is in time frame selected
// Input:		device id
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CX10PingWorker::IsInTimeFrame( unsigned int nDeviceId )
{
	char strMsg[255];

	// if no time frame selected return ok
	if( m_vectNetDevice[nDeviceId]->m_nCheckTimeFrame == 0 ) return( 1 );

	long nTimeStart = m_vectNetDevice[nDeviceId]->m_nCheckStartTimeH*60 + m_vectNetDevice[nDeviceId]->m_nCheckStartTimeM;
	long nTimeEnd = m_vectNetDevice[nDeviceId]->m_nCheckEndTimeH*60 + m_vectNetDevice[nDeviceId]->m_nCheckEndTimeM;
	long nCurrentTime;
	// get time 
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	nCurrentTime = timeinfo->tm_hour*60 + timeinfo->tm_min;

	if( nCurrentTime >= nTimeStart && nCurrentTime <= nTimeEnd )
	{
//		sprintf( strMsg, "DEBUG :worker: device=%d in time frame", nDeviceId );
//		m_pLogger->Log( strMsg );
		return( 1 );
	} else
	{
//		sprintf( strMsg, "DEBUG :worker: device=%d not in time frame", nDeviceId );
//		m_pLogger->Log( strMsg );
		return( 0 );
	}
}

////////////////////////////////////////////////////////////////////
// Method:		ActionNotify
// Class:		CX10PingWorker
// Purpose:		notify on action
// Input:		device id
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CX10PingWorker::ActionNotify( unsigned int nDeviceId )
{
	if( m_bIsExit == 1 ) return( 0 );

	int nStatus = 0;
	char strMsg[255];

	// get time label
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	// by action device type
	switch( m_vectNetDevice[nDeviceId]->m_nActionNotify )
	{
		case DEVICE_ACTION_NOT_NOTIFY:
		{
			break;
		}

		case DEVICE_ACTION_VISUAL_NOTIFY:
		{
			if( m_pFrame->m_bIconized == 1 )
			{
				m_pFrame->Show(TRUE);
				m_pFrame->Iconize( false );
			//	m_pFrame->Show(TRUE);
				m_pFrame->m_pFrameTaskBarIcon->RemoveIcon( );
				m_pFrame->m_bIconized = 0;
			}
			m_pFrame->Raise();

			// then popup alert
			sprintf( strMsg, "The device=%s was found down!", m_vectNetDevice[nDeviceId]->m_strName );
			wxMessageBox( strMsg, _( "x10ping notification" ), wxICON_EXCLAMATION | wxOK );
			break;
		}

		case DEVICE_ACTION_AUDIO_NOTIFY:
		{
			wxSound* pAudioNotify = new wxSound( m_strNotifyAudioFile );

			if( pAudioNotify->IsOk( ) )
			{
				pAudioNotify->Play( );
				Sleep( 500 );

			} else
			{
				m_pLogger->Log( "ERROR :: could not play audio file" );
			}

			delete( pAudioNotify );
			break;
		}

		case DEVICE_ACTION_EMAIL_NOTIFY:
		{
			CSmtp mail;
			CSmtpMessage msg;
			CSmtpAddress cc;
			CSmtpMessageBody body;
			CSmtpAttachment attach;
			
			// Initialize winsock
			WSADATA wsa;
			WSAStartup(MAKEWORD(2,0),&wsa);
			// set subject
			sprintf( strMsg, "x10ping monitor :: the device=%s was found down!", m_vectNetDevice[nDeviceId]->m_strName );
			msg.Subject = _T( strMsg );
			// Who the message is from
			msg.Sender.Name = _T( "x10ping monitor" );
			msg.Sender.Address = _T( m_strEmailFrom );

			// Primary recipient
			msg.Recipient.Name = _T("");
			msg.Recipient.Address = _T( m_strEmailTo );

			// copy content of remote string in local buffer
			sprintf( strMsg, "\n\n The device=%s was found down at time=%.2d:%.2d:%.2d %.2d/%.2d/%d\n",
							m_vectNetDevice[nDeviceId]->m_strName,
							timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, 
							timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900 );

			// Assign a value to the message body
			body = _T( strMsg );

			// Add the message body to the message
			msg.Message.Add( body );


			// Here are some samples of adding attachments to the message
			//  attach = _T("d:\\temp\\ntregmon.zip");
			//  msg.Attachments.Add(attach);

			// Assign these values only if your SMTP server requires authentication
			if( m_bEmailAuth == 1 )
			{
				mail.m_strUser = _T( m_strEmailAuthUser );
				mail.m_strPass = _T( m_strEmailAuthPasswd );
			}

			// Attempt to connect to the mailserver
			if ( mail.Connect( m_strEmailServer ) )
			{
				// Send the message and close the connection afterwards
				mail.SendMessage(msg);

				// Alternate one-shot method of sending a message
				// mail.SendMessage(_T("foo@bar.com"),
				//					_T("someone@else.com"),
				//					_T("My Subject"),
				//					_T("This is the msg body"));
				mail.Close();
			}

			WSACleanup();

			
			break;
		}

		default: break;
	}

	return( nStatus );
}

////////////////////////////////////////////////////////////////////
// Method:		ActionDevice
// Class:		CX10PingWorker
// Purpose:		action a device by method selected
// Input:		device id
// Output:		nothing
////////////////////////////////////////////////////////////////////
int CX10PingWorker::ActionDevice( unsigned int nDeviceId )
{
	if( m_bIsExit == 1 ) return( 0 );

	int nStatus = 0;
	// by action device type
	switch( m_vectNetDevice[nDeviceId]->m_nActionFunction )
	{
		case DEVICE_ACTION_POWER_CYCLE:
		{
			// check if x10 sent off command ok, else break
			nStatus = TurnDeviceOff( nDeviceId );
			if( nStatus == 0 )
			{
				m_pLogger->Log( "ERROR :: power on failure" );
				break;
			}

			// sleep after OFF
			wxThread::Sleep( m_nX10DelayAfterOFF );

			// check if x10 sent on command ok, else break
			nStatus = TurnDeviceOn( nDeviceId );
			if( nStatus == 0 )
			{
				m_pLogger->Log( "ERROR :: power off failure" );
				break;
			}

			wxThread::Sleep( m_vectNetDevice[nDeviceId]->m_nRebootTime*100 );

			nStatus = 1;

			break;
		}

		case DEVICE_ACTION_POWER_IMPULSE:
		{
			// check if x10 sent on command ok, else break
			nStatus = TurnDeviceOn( nDeviceId );
			if( nStatus == 0 )
			{
				m_pLogger->Log( "ERROR :: power off failure" );
				break;
			}

			// sleep after ON
			wxThread::Sleep( m_nX10DelayAfterON );
			// sleep reboot time
			wxThread::Sleep( m_vectNetDevice[nDeviceId]->m_nRebootTime*100 );

			// check if x10 sent off command ok, else break
			nStatus = TurnDeviceOff( nDeviceId );
			if( nStatus == 0 )
			{
				m_pLogger->Log( "ERROR :: power on failure" );
				break;
			}

			// sleep after OFF
			wxThread::Sleep( m_nX10DelayAfterOFF );

			nStatus = 1;

			break;
		}

		case DEVICE_ACTION_POWER_OFF:
		{
			nStatus = TurnDeviceOff( nDeviceId );
			if( nStatus == 0 )
			{
				m_pLogger->Log( "ERROR :: power off failure" );
				break;
			}

			// sleep after OFF
			wxThread::Sleep( m_nX10DelayAfterOFF );

			nStatus = 1;

			break;
		}

		case DEVICE_ACTION_POWER_ON:
		{
			nStatus = TurnDeviceOn( nDeviceId );
			if( nStatus == 0 )
			{
				m_pLogger->Log( "ERROR :: power on failure" );
				break;
			}

			// sleep after OFF
			wxThread::Sleep( m_nX10DelayAfterON );

			nStatus = 1;

			break;
		}

		case DEVICE_ACTION_DO_NOTHING:
		{
			nStatus = 1;
			// just do nothing 
			break;
		}

		default: break;
	}

	return( nStatus );
}

////////////////////////////////////////////////////////////////////
// Method:		Entry
// Class:		CX10PingWorker
// Purpose:		the main executable body of my thread
// Input:		nothing
// Output:		void pointer
////////////////////////////////////////////////////////////////////
void *CX10PingWorker::Entry( )
{
	char strMsg[255];
	int i = 0;
	m_bLife = 1;

	// Update configuration
	UpdateConfig( );
	// load devices from file if they exist
	LoadNetDevices( "" );

	////////////////////////////////////////////////////////////////
	// Start Life Cycle
	////////////////////////////////////////////////////////////////
	m_pLogger->Log( "INFO :worker: start life cycle" );
	// loop as long as flag m_bLife = 1
	while( m_bLife )
	{
		m_bIsProcessing = 0;

		if( TestDestroy( ) == 1 ) 
			break;

		if( m_bIsPaused == 1 )
		{
			if( m_bChanged == 1 ) IsChanged( 0 );
			wxThread::Sleep( 200 );
			continue;
		}
		// for every device in my list 
		for( i=0; i<m_nDevice; i++ )
		{	
			// check if on pause then brake
			if( m_bPauseLoop == 1 )
			{	
				m_bIsPaused = 1;
				m_pFrame->m_bWorkerIsPaused = 1;
				break;
			}

			// set log by logic
			wxString strCheckLogic = "down";
			switch( m_vectNetDevice[i]->m_nActionLogic )
			{
				case DEVICE_CHECK_LOGIC_OFF:
				{
					strCheckLogic = "down";
					break;
				}

				case DEVICE_CHECK_LOGIC_ON:
				{
					strCheckLogic = "up";
					break;
				}

				case DEVICE_CHECK_LOGIC_SLOW:
				{
					strCheckLogic = "slow";
					break;
				}

				default: break;
			}

			// check if time frame
			if( IsInTimeFrame( i ) == 0 ) continue;

			if( UpdateDeviceState( i, DEVICE_STATE_CHECKING ) == 0 ) break;
	
			m_bIsProcessing = 1;
			// the big loop to check if host is alive
			if( CheckDevice( i ) == 0 )
			{
				if( TestDestroy( ) == 1 ) 
					break;

				if( UpdateDeviceState( i, DEVICE_STATE_DOWN ) == 0 ) break;
				// check if we are at the limit with trying
				if( m_vectNetDevice[i]->m_nCounter >= m_vectNetDevice[i]->m_nFailuresToAction-1 )
				{
					// the host is down so we need to reset
					sprintf( strMsg, "INFO :: device =%s found %s after we tried %d times",
							m_vectNetDevice[i]->m_strName, strCheckLogic, m_vectNetDevice[i]->m_nFailuresToAction );
							m_pLogger->Log( strMsg, GUI_LOGGER_ATTENTION_STYLE );

					// check if x10 action was done command ok, else break
					if( ActionDevice( i ) == 0 ) break;
					// now also notify
					ActionNotify( i );
					
					m_vectNetDevice[i]->SetCounter( 0 );

				} else
				{
					sprintf( strMsg, "INFO :: device =%s found %s after %d try",
									m_vectNetDevice[i]->m_strName, strCheckLogic, m_vectNetDevice[i]->m_nCounter+1 );
					m_pLogger->Log( strMsg, GUI_LOGGER_ATTENTION_STYLE );
					m_vectNetDevice[i]->m_nCounter++;
				}

			// if ping ok sleep x amount of time and reset counter to zero
			} else
			{
				if( m_vectNetDevice[i]->m_nUnitState != DEVICE_STATE_UP )
				{
					if( UpdateDeviceState( i, DEVICE_STATE_UP ) == 0 ) break;
				}
				m_vectNetDevice[i]->SetCounter( 0 );
			}
			//m_bIsProcessing = 0;
			Sleep( 100 );
		}

		if( TestDestroy( ) == 1 ) 
			break;

		// Thread sleep
		if( m_bPauseLoop == 0 )	wxThread::Sleep( m_nLoopCheckDelay );

		if( m_bChanged == 1 ) IsChanged( 0 );
	}
	// end of my life time
//	m_pLogger->Log( "INFO :worker: finished my life span" );

    return( NULL );
}
