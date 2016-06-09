////////////////////////////////////////////////////////////////////
// Package:		config definition
// File:		config.h
// Purpose:		manage software/hardware configuration
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:	
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _CONFIG_H
#define _CONFIG_H

#define CONFIG_FILE_NAME		"./x10ping.cfg"
#define MAX_NO_CONF_SECTION		10
#define MAX_NO_CONF_VARS		20

// define sections ids
#define SECTION_ID_X10			0
#define SECTION_ID_NETWORK		1
#define SECTION_ID_RUNTIME		2
#define SECTION_ID_NOTIFY		3


// define Section :: X10 - var IDs
#define CONF_X10_SERIALPORT				0
#define	CONF_X10_CONTROLLER_MODEL		1
// define Section :: Network - var IDs
#define CONF_NETWORK_LOOP_CHECK_DELAY	0
#define CONF_DEVICE_PING_RETRY			1
#define CONF_NETWORK_X10_DELAY_AFTER_OFF		2
#define CONF_NETWORK_X10_DELAY_AFTER_ON			3
// define Section :: Runtime - var IDs
#define CONF_RUN_ON_STARTUP 0
#define CONF_MONITOR_ON_STARTUP 1
//define Section :: notify
#define CONF_EMAIL_SERVER				0
#define	CONF_EMAIL_FROM					1
#define CONF_EMAIL_TO					2
#define CONF_EMAIL_AUTH					3
#define CONF_EMAIL_AUTH_USER			4
#define CONF_EMAIL_AUTH_PASSWD			5
#define CONF_AUDIO_NOTIFY_FILE			6

// define var types;
#define VAR_TYPE_NUMBER					0
#define VAR_TYPE_STRING					1

#include <wx/arrstr.h>

// external classes
class CX10PingWorker;
class CX10;
class CConfigView;

// CLASS :: CCONFIG
////////////////////////////////////
class CConfig
{
// methods
public:
	CConfig( char* strPath );
	~CConfig( );
	void Init( );
	int Load( );
	int Save( );
	int SetIntVar( int nSection, int nVarId, int nVarValue );
	int SetIntVarUpdate( int nSection, int nVarId, int nVarValue );
	int GetIntVar( int nSection, int nVarId, int& nVarValue );
	int SetStrVar( int nSection, int nVarId, char* strVarValue );
	int SetStrVarUpdate( int nSection, int nVarId, char* strVarValue );
	int GetStrVar( int nSection, int nVarId, char* strVarValue );

// data
public:
	// my data vectors
	int m_vectConfigVar[MAX_NO_CONF_SECTION][MAX_NO_CONF_VARS];
	char m_vectConfigStrVar[MAX_NO_CONF_SECTION][MAX_NO_CONF_VARS][255];
	int m_vectConfigVarInit[MAX_NO_CONF_SECTION][MAX_NO_CONF_VARS];
	int m_vectConfigVarType[MAX_NO_CONF_SECTION][MAX_NO_CONF_VARS];

	// section/var label definition
	wxArrayString m_vectSectionDef;
	wxArrayString m_arrConfigVar[MAX_NO_CONF_SECTION];

	// remote object
	CX10PingWorker	*m_pX10PingWorker;
	CX10			*m_pX10;

	// config file with path
	char m_strConfigFile[255];
};

#endif
