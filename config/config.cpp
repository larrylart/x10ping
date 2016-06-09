////////////////////////////////////////////////////////////////////
// Package:		Config implementation
// File:		config.cpp
// Purpose:		manage software configuration from to file/argv
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:	Larry Lart on 31-Jul-2006
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include "wx/defs.h"

// custom
#ifdef _WINDOWS
#include "../util/func.h"
#endif

// wxwindows includes
#include "wx/wxprec.h"
#include <wx/string.h>
#include <wx/regex.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// custom headers
#include "../x10.h"
#include "../worker.h"
#include "../gui/configview.h"

// main header
#include "config.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CConfig
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CConfig::CConfig( char* strPath )
{
	m_pX10 = NULL;
	m_pX10PingWorker = NULL;

	if( strPath != NULL ) 
	{
		sprintf( m_strConfigFile, "%s%s%s", strPath, "\\", CONFIG_FILE_NAME );
	} else
	{
		sprintf( m_strConfigFile, "%s", CONFIG_FILE_NAME );
	}

	// set my default labels( hard coded for now)
	m_vectSectionDef.Insert( "X10", SECTION_ID_X10 );
	m_vectSectionDef.Insert( "Network", SECTION_ID_NETWORK );
	m_vectSectionDef.Insert( "Runtime", SECTION_ID_RUNTIME );
	m_vectSectionDef.Insert( "Notify", SECTION_ID_NOTIFY );

	// set my default vars for visual cortex
	// TODO :: how to have index starting zero by section ?
	m_arrConfigVar[SECTION_ID_X10].Insert( "SerialPort", CONF_X10_SERIALPORT );
	m_arrConfigVar[SECTION_ID_X10].Insert( "X10ControllerModel", CONF_X10_CONTROLLER_MODEL );

	// runtime section define
	m_arrConfigVar[SECTION_ID_RUNTIME].Insert( "RunOnStartup", CONF_RUN_ON_STARTUP );
	m_arrConfigVar[SECTION_ID_RUNTIME].Insert( "MonitorOnStartup", CONF_MONITOR_ON_STARTUP );

	// network section define
	m_arrConfigVar[SECTION_ID_NETWORK].Insert( "LoopCheckDelay", CONF_NETWORK_LOOP_CHECK_DELAY );
	m_arrConfigVar[SECTION_ID_NETWORK].Insert( "DevicePingRetry", CONF_DEVICE_PING_RETRY );
	m_arrConfigVar[SECTION_ID_NETWORK].Insert( "X10DelayAfterOFF", CONF_NETWORK_X10_DELAY_AFTER_OFF );
	m_arrConfigVar[SECTION_ID_NETWORK].Insert( "X10DelayAfterON", CONF_NETWORK_X10_DELAY_AFTER_ON );

	// notify section define
	m_arrConfigVar[SECTION_ID_NOTIFY].Insert( "EmailServer", CONF_EMAIL_SERVER );
	m_arrConfigVar[SECTION_ID_NOTIFY].Insert( "EmailFrom", CONF_EMAIL_FROM );
	m_arrConfigVar[SECTION_ID_NOTIFY].Insert( "EmailTo", CONF_EMAIL_TO );
	m_arrConfigVar[SECTION_ID_NOTIFY].Insert( "EmailAuth", CONF_EMAIL_AUTH );
	m_arrConfigVar[SECTION_ID_NOTIFY].Insert( "EmailAuthUser", CONF_EMAIL_AUTH_USER );
	m_arrConfigVar[SECTION_ID_NOTIFY].Insert( "EmailAuthPasswd", CONF_EMAIL_AUTH_PASSWD );
	m_arrConfigVar[SECTION_ID_NOTIFY].Insert( "AudioFile", CONF_AUDIO_NOTIFY_FILE );

	// call init
	Init( );
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CConfig
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CConfig::~CConfig( )
{
	//  save my configuration on exit
	this->Save( );
	// clear stuff left around
	for( int i=0; i<m_vectSectionDef.GetCount() ; i++ )
	{
		m_arrConfigVar[i].Clear( );
	}
	m_vectSectionDef.Clear( );

	m_pX10PingWorker = NULL;
	m_pX10 = NULL;

	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	Init
// Class:	CConfig
// Purpose:	inittialize my  object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CConfig::Init( )
{
	int i =0, j = 0;
	// set the init vector
	for( i=0; i<MAX_NO_CONF_SECTION; i++ )
	{
		for( j=0; j<MAX_NO_CONF_VARS; j++ )
		{
			m_vectConfigVarInit[i][j] = 0;
		}
	}

	// initialize my configuration
	if( !Load() )
	{
		// TODO: default initialization ?
	}
	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	Set Int Var
// Class:	CConfig
// Purpose:	set integer var of section/var id to a value 
// Input:	section id, var id, var value
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfig::SetIntVar( int nSection, int nVarId, int nVarValue )
{
	// check range valid for sections and variables
	if( ( nVarId < 0 && nVarId >= MAX_NO_CONF_VARS )  || 
		( nSection < 0 && nSection >= MAX_NO_CONF_SECTION ) ) return ( - 1 );
	// set my variable
	m_vectConfigVar[nSection][nVarId] = nVarValue;
	m_vectConfigVarInit[nSection][nVarId] = 1;
	m_vectConfigVarType[nSection][nVarId] = VAR_TYPE_NUMBER;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Set Str Var
// Class:	CConfig
// Purpose:	set string var of section/var id to a value 
// Input:	section id, var id, var value
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfig::SetStrVar( int nSection, int nVarId, char* strVarValue )
{
	// check range valid for sections and variables
	if( ( nVarId < 0 && nVarId >= MAX_NO_CONF_VARS )  || 
		( nSection < 0 && nSection >= MAX_NO_CONF_SECTION ) ) return ( - 1 );
	// set my variable
	sprintf( m_vectConfigStrVar[nSection][nVarId] , "%s", strVarValue );
	m_vectConfigVarInit[nSection][nVarId] = 1;
	m_vectConfigVarType[nSection][nVarId] = VAR_TYPE_STRING;

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Set Int Var Update
// Class:	CConfig
// Purpose:	set integer var of section/var id to a value and call remote 
//			objects to update
// Input:	section id, var id, var value
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfig::SetIntVarUpdate( int nSection, int nVarId, int nVarValue )
{

	if( SetIntVar( nSection, nVarId, nVarValue ) == -1 ) return( -1 );

	// check by section to update affected objects ??? feedback call ?!!
	switch( nSection )
	{
		// x10 section
		case SECTION_ID_X10:
		{
			// chek if pointer not set
			if( m_pX10 == NULL ) return( -1 );
			// else call remote update
			m_pX10->UpdateConfig();

			break;
		}
		// network section
		case SECTION_ID_NETWORK:
		{
			// chek if pointer not set
			if( m_pX10PingWorker == NULL ) return( -1 );
			// else call remote update
			m_pX10PingWorker->UpdateConfig( );

			break;
		}

		default: break;
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Set Str Var Update
// Class:	CConfig
// Purpose:	set integer var of section/var id to a value and call remote 
//			objects to update
// Input:	section id, var id, var value
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfig::SetStrVarUpdate( int nSection, int nVarId, char* strVarValue )
{

	if( SetStrVar( nSection, nVarId, strVarValue ) == -1 ) return( -1 );

	// check by section to update affected objects ??? feedback call ?!!
	switch( nSection )
	{
		// x10 section
		case SECTION_ID_X10:
		{
			// chek if pointer not set
			if( m_pX10 == NULL ) return( -1 );
			// else call remote update
			m_pX10->UpdateConfig();

			break;
		}
		// network section
		case SECTION_ID_NETWORK:
		{
			// chek if pointer not set
			if( m_pX10PingWorker == NULL ) return( -1 );
			// else call remote update
			m_pX10PingWorker->UpdateConfig( );

			break;
		}

		default: break;
	}

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Get Integer Var
// Class:	CConfig
// Purpose:	return variable by section and var id if number
// Input:	section id, var id, reference to variable
// Output:	var value
/////////////////////////////////////////////////////////////////////
int CConfig::GetIntVar( int nSection, int nVarId, int& nVarValue )
{
	// check range valid for sections and variables
	if( ( nVarId < 0 && nVarId >= MAX_NO_CONF_VARS )  || 
		( nSection < 0 && nSection >= MAX_NO_CONF_SECTION ) || 
		m_vectConfigVarInit[nSection][nVarId] == 0 ) return ( 0 );
	
	// set my var 
	nVarValue = m_vectConfigVar[nSection][nVarId];

	// return success = 1
	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Get String Var
// Class:	CConfig
// Purpose:	return variable by section and var id if number
// Input:	section id, var id, reference to variable
// Output:	var value
/////////////////////////////////////////////////////////////////////
int CConfig::GetStrVar( int nSection, int nVarId, char* strVarValue )
{
	// check range valid for sections and variables
	if( ( nVarId < 0 && nVarId >= MAX_NO_CONF_VARS )  || 
		( nSection < 0 && nSection >= MAX_NO_CONF_SECTION ) || 
		m_vectConfigVarInit[nSection][nVarId] == 0 ) return ( 0 );
	
	// set my var 
	sprintf( strVarValue, "%s", m_vectConfigStrVar[nSection][nVarId] );

	// return success = 1
	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Load 
// Class:	CConfig
// Purpose:	load my software configuration from file
// Input:	nothing
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfig::Load( )
{
	FILE* pFile = NULL;
	char strLine[2000];
	char strVarName[256];
	char strVarValue[256];
	int nSection = -1;
	// define my regex 
	wxRegEx reSection( "\\[([a-zA-Z0-9_\\-]+)\\]" );
	wxRegEx reVariable = wxT( "([a-zA-Z0-9_\\-]+)\\ *=\\ *([^\n^\r^\t]+)\\ *" );

	// Read the index
	pFile = fopen( m_strConfigFile, "r" );

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
			wxString strSection = reSection.GetMatch(strLine, 1 );
			int nTmp = m_vectSectionDef.Index( strSection );
			if( nTmp >= 0 ) nSection = nTmp;

		} else if( nSection >=0 && reVariable.Matches( strLine ) )
		{
			wxString strVarName = reVariable.GetMatch(strLine, 1 );
			wxString strVarValue = reVariable.GetMatch(strLine, 2 );
			
			int nVarId = m_arrConfigVar[nSection].Index( strVarName );
			// check if variable is number or string
			if( strVarValue.IsNumber() )
			{
				int nVarValue = atoi( strVarValue );
				// set my variables
				SetIntVarUpdate( nSection, nVarId, nVarValue );
			} else
			{
				char strTmp[255];
				sprintf( strTmp, "%s", strVarValue.GetData() );
				SetStrVarUpdate( nSection, nVarId, strTmp );

			}
		}
	}

	// close file
	fclose( pFile );

	return( 1 );
}

/////////////////////////////////////////////////////////////////////
// Method:	Save 
// Class:	CConfig
// Purpose:	save my configuration to a file/db ?
// Input:	nothing
// Output:	status
/////////////////////////////////////////////////////////////////////
int CConfig::Save( )
{
	FILE* pFile = NULL;
	unsigned int i = 0, j = 0;
	int nVarValue = 0;
	char strVarValue[255];


	// open config file to write
	pFile = fopen( m_strConfigFile, "w" );
	// check if there is any configuration to load
	if( !pFile ) return( 0 );

	// for each section
	for( i=0; i<m_vectSectionDef.GetCount() ; i++ )
	{
		// check if defined
		//??if( !m_vectSectionDef.Item( i ) ) continue;
		// print header
		fprintf( pFile, "\n\n\[%s\]\n", m_vectSectionDef[i] );
		// for each possibile var id
		for( j=0; j<m_arrConfigVar[i].GetCount(); j++ )
		{
			if( m_vectConfigVarType[i][j] == VAR_TYPE_NUMBER )
			{
				if( GetIntVar( i, j, nVarValue ) != 0 )
				{
					fprintf( pFile, "%s=%d\n", m_arrConfigVar[i].Item( j ),  
							 nVarValue );
				}
			} else if( m_vectConfigVarType[i][j] == VAR_TYPE_STRING )
			{
				if( GetStrVar( i, j, strVarValue ) != 0 )
				{
					fprintf( pFile, "%s=%s\n", m_arrConfigVar[i].Item( j ),  
							 strVarValue );
				}
			}
		}
	}
	// close my file handler
	fclose( pFile );

	return( 1 );
}

