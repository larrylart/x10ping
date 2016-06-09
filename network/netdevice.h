////////////////////////////////////////////////////////////////////
// Package:		net device definition
// File:		netdevice.h
// Purpose:		define and control a network device
//
// Created by:	Larry Lart on 20-Jun-2006
// Updated by:	
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _NET_DEVICE
#define _NET_DEVICE

// device states
#define DEVICE_STATE_DOWN			0
#define DEVICE_STATE_UP				1
#define DEVICE_STATE_CHECKING		2
#define DEVICE_STATE_SWITCHED_OFF	3
#define DEVICE_STATE_SWITCHED_ON	4
// define check methods
#define DEVICE_CHECK_METHOD_PING			1
#define DEVICE_CHECK_METHOD_SOCKET			2
#define	DEVICE_CHECK_METHOD_EXT_PROG		3
// define action types
#define DEVICE_ACTION_POWER_CYCLE			1
#define DEVICE_ACTION_POWER_IMPULSE			2
#define DEVICE_ACTION_POWER_OFF				3
#define DEVICE_ACTION_POWER_ON				4
#define DEVICE_ACTION_DO_NOTHING			5
// define action notification
#define DEVICE_ACTION_NOT_NOTIFY			1
#define DEVICE_ACTION_VISUAL_NOTIFY			2
#define DEVICE_ACTION_AUDIO_NOTIFY			3
#define DEVICE_ACTION_EMAIL_NOTIFY			4

// define check logic
#define DEVICE_CHECK_LOGIC_OFF				1
#define DEVICE_CHECK_LOGIC_ON				2
#define DEVICE_CHECK_LOGIC_SLOW				3
// device x10 default delays
#define X10_DELAY_AFTER_OFF					5000
#define X10_DELAY_AFTER_ON					2000

// externat classes
class CX10PingWorker;

////////////////////////
// class :	CNetDevice
class CNetDevice
{

// public methods
public:
	CNetDevice( CX10PingWorker* pX10PingWorker );
	~CNetDevice( );
	int Init( );
//	void UpdateConfig( );
	int Edit( const char *strDevName, unsigned int nHouseId, 
				unsigned int nUnitId, int nCheckMethod, int nFailuresToAction, 
				int nActionLogic, int nActionFunction, int nActionNotify,
				int nRebootTime, int bCheckTimeFrame, 
				int nCheckStartTimeH, int nCheckStartTimeM, 
				int nCheckEndTimeH, int nCheckEndTimeM );
	int SetState( int nUnitState );
	int SetCounter( int nCounter );

// public data
public:
	// device properties
	char m_strName[255];
	int m_nHouseId;
	unsigned int m_nUnitId;
	int m_nUnitState;
	int m_nCounter;
	
	int m_nCheckMethod;
	int m_nFailuresToAction;
	int m_nActionLogic;
	int m_nActionFunction;
	int m_nActionNotify;
	int m_nRebootTime;

	int m_nCheckTimeFrame;
	int m_nCheckStartTimeH;
	int m_nCheckStartTimeM;
	int m_nCheckEndTimeH;
	int m_nCheckEndTimeM;

	// other class references
	CX10PingWorker* m_pX10PingWorker;

	// other references
};

#endif