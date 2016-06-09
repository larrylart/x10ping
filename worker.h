////////////////////////////////////////////////////////////////////
// Package:		worker definition
// File:		worker.h
// Purpose:		manage checking, switching tasks
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:	
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _WORKER_H
#define _WORKER_H

// local defines
#define MAX_DEV_NUMBER				20
// default values
#define NETWORK_DEFAULT_LOOP_CHECK_DELAY	20000
#define NETWORK_DEFAULT_NO_RETRY			5
// cofig file for net devices
#define NET_DEVICE_CONFIG_FILE_NAME		"./x10ping_dev.cfg"


#include "wx/wxprec.h"
#include "wx/thread.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

class CX10PingApp;
class CGUIFrame;
class CLogger;
class CConfig;
class CPing;
class CX10;
class CNetDevice;

//////////////////////////////////////////////////
// class:	CX10PingWorker
/////////////////////////////////////////////////
class CX10PingWorker : public wxThread
{
// methods
public:
	CX10PingWorker( CGUIFrame *pFrame, CLogger *pLogger, CConfig *pConfig, char* strPath );

	~CX10PingWorker();
	// thread entry point
	virtual void *Entry();
	virtual void OnExit();
	// save load methods
	int SaveNetDevices( const char* strNetDevConfigFile );
	int LoadNetDevices( const char* strNetDevConfigFile );
	// device list handling methods
	void DisplayGuiDeviceList( );
	void ClearGuiDeviceList( );
	int AddDevice( const char *strName,unsigned int nHouseId, 
							  unsigned int nUnitId, 
							  int nCheckMethod, int nFailuresToAction, 
							   int nActionLogic, int nActionFunction, 
							   int nActionNotify,
							   int nRebootTime, int bCheckTimeFrame, 
							   int nCheckStartTimeH, int nCheckStartTimeM, 
							   int nCheckEndTimeH, int nCheckEndTimeM );

	int DeleteDevice( unsigned int nListId );
	int EditDevice( unsigned int nListId, const char *strDevName, 
							   unsigned int nHouseId, unsigned int nUnitId, 
							   int nCheckMethod, int nFailuresToAction, 
							   int nActionLogic, int nActionFunction, 
							   int nActionNotify, 
							   int nRebootTime, int bCheckTimeFrame, 
							   int nCheckStartTimeH, int nCheckStartTimeM, 
							   int nCheckEndTimeH, int nCheckEndTimeM );
	int GetDeviceData( unsigned int nListId, char* strDevName,
						int& nHousId, int& nDevId, int& nCheckMethod,
						int& nFailuresToAction, int& nActionLogic,
						int& nActionFunction, int& nActionNotify, int& nRebootTime, 
						int& bCheckTimeFrame, int& nCheckStartTimeH,
						int& nCheckStartTimeM, int& nCheckEndTimeH, 
						int& nCheckEndTimeM );
	int UpdateDeviceState( int nDeviceIndex, int nDeviceState );
	int CheckDevice( unsigned int nDeviceId );
	int ActionDevice( unsigned int nDeviceId );
	int IsInTimeFrame( unsigned int nDeviceId );
	int TurnDeviceOn( unsigned int nListId );
	int TurnDeviceOff( unsigned int nListId );
	void UpdateConfig( );
	// action methods
	void DoStart( );
	void DoPause( );
	void IsChanged( int nState );
	int ActionNotify( unsigned int nDeviceId );

// data
public:
	CLogger			*m_pLogger;	
	CX10PingApp		*m_pX10PingApp;
    CGUIFrame		*m_pFrame;
	CConfig			*m_pConfig;
	CPing			*m_pPing;
	CX10			*m_pX10;

	// device list vars
	CNetDevice* m_vectNetDevice[MAX_DEV_NUMBER];

//	char m_vectDeviceNetName[MAX_DEV_NUMBER][255];
//	int m_vectDeviceHouseId[MAX_DEV_NUMBER];
//	unsigned int m_vectDeviceUnitId[MAX_DEV_NUMBER];
//	int m_vectDeviceUnitState[MAX_DEV_NUMBER];
//	int m_vectDeviceCounter[MAX_DEV_NUMBER];

	int m_nDevice;
	int m_nLoopCheckDelay;
	int m_nRetry;
	int m_nX10DelayAfterOFF;
	int m_nX10DelayAfterON;
	int m_bIsMonitorStarted;
	int m_bIsPaused;
	int m_bPauseLoop;
	int m_bChanged;
	int m_bIsJustStarted;
	int m_bEmailAuth;
	int m_bEmailServer;
	int m_bAudioNotification;

	char m_strEmailServer[255];
	char m_strEmailFrom[255];
	char m_strEmailTo[255];
	char m_strEmailAuthUser[255];
//	char m_strEmailPasswd[255];
	char m_strEmailAuthPasswd[255];
	char m_strNotifyAudioFile[255];

	unsigned char	m_bLife;
	int m_bIsProcessing;
	int m_bIsExit;

	// device properties
//	int m_vectCheckMethod[MAX_DEV_NUMBER];
//	int m_vectFailuresToAction[MAX_DEV_NUMBER];
//	int m_vectActionLogic[MAX_DEV_NUMBER];
//	int m_vectActionFunction[MAX_DEV_NUMBER];
//	int m_vectRebootTime[MAX_DEV_NUMBER];

//	int m_vectCheckTimeFrame[MAX_DEV_NUMBER];
//	int m_vectCheckStartTimeH[MAX_DEV_NUMBER];
//	int m_vectCheckStartTimeM[MAX_DEV_NUMBER];
//	int m_vectCheckEndTimeH[MAX_DEV_NUMBER];
//	int m_vectCheckEndTimeM[MAX_DEV_NUMBER];

	// path
	char m_strNetDevConfigFile[255];

// private data
private:
	// process syncronizer/locker
	wxMutex *m_pMutex;

};

#endif


