////////////////////////////////////////////////////////////////////
// Package:		net device implementation
// File:		netdevice.cpp
// Purpose:		manage network devices
//
// Created by:	Larry Lart on 20-Jun-2006
// Updated by:	
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

// other headers
#include "../worker.h"

// main header
#include "netdevice.h"

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CNetDevice
// Purose:	build my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CNetDevice::CNetDevice( CX10PingWorker* pX10PingWorker )
{
	m_pX10PingWorker = pX10PingWorker;

	m_nCounter = 0;
	m_nHouseId = -1;
	m_nUnitId = -1;

	m_nCheckMethod = -1;
	m_nFailuresToAction = -1;
	m_nActionLogic = -1;
	m_nActionFunction = -1;
	m_nActionNotify = -1;
	m_nRebootTime = -1;

	m_nCheckTimeFrame = -1;
	m_nCheckStartTimeH = -1;
	m_nCheckStartTimeM = -1;
	m_nCheckEndTimeH = -1;
	m_nCheckEndTimeM = -1;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CNetDevice
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CNetDevice::~CNetDevice( )
{
	m_pX10PingWorker = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	Init
// Class:	CNetDevice
// Purose:	initialize my x10 system
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
int CNetDevice::Init( )
{
	int nStatus = 0;
//	char strMsg[255];

	return( nStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	Edit
// Class:	CNetDevice
// Purpose:	edit and existing device id
// Input:	device list id, pointer to name, device id
// Output:	status	
////////////////////////////////////////////////////////////////////
int CNetDevice::Edit( const char *strDevName, unsigned int nHouseId, 
					unsigned int nUnitId, int nCheckMethod, int nFailuresToAction, 
					int nActionLogic, int nActionFunction, int nActionNotify,
					int nRebootTime, int bCheckTimeFrame, 
					int nCheckStartTimeH, int nCheckStartTimeM, 
					int nCheckEndTimeH, int nCheckEndTimeM )
{
	strcpy( m_strName, strDevName );
	m_nHouseId = nHouseId;
	m_nUnitId = nUnitId;

	m_nCheckMethod = nCheckMethod;
	// check for globals
	if( nFailuresToAction >= 0 )
		m_nFailuresToAction = nFailuresToAction;
	else
		m_nFailuresToAction = m_pX10PingWorker->m_nRetry;

	m_nActionLogic = nActionLogic;
	m_nActionFunction = nActionFunction;
	m_nActionNotify = nActionNotify;
	// check for globals
	if( nRebootTime >= 0 )
		m_nRebootTime = nRebootTime;
	else
		m_nRebootTime = m_pX10PingWorker->m_nX10DelayAfterON;

	m_nCheckTimeFrame = bCheckTimeFrame;
	m_nCheckStartTimeH = nCheckStartTimeH;
	m_nCheckStartTimeM = nCheckStartTimeM;
	m_nCheckEndTimeH = nCheckEndTimeH;
	m_nCheckEndTimeM = nCheckEndTimeM;

	// reset counter for this one to zero
	m_nCounter = 0;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	SetState
// Class:	CNetDevice
// Purpose:	edit and existing device id
// Input:	device list id, pointer to name, device id
// Output:	status	
////////////////////////////////////////////////////////////////////
int CNetDevice::SetState( int nUnitState )
{
	m_nUnitState = nUnitState;

	return( 1 );
}

////////////////////////////////////////////////////////////////////
// Method:	SetCounter
// Class:	CNetDevice
// Purpose:	edit and existing device id
// Input:	device list id, pointer to name, device id
// Output:	status	
////////////////////////////////////////////////////////////////////
int CNetDevice::SetCounter( int nCounter )
{
	m_nCounter = nCounter;

	return( 1 );
}
