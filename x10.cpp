////////////////////////////////////////////////////////////////////
// Package:		x10 implementation
// File:		x10.cpp
// Purpose:		manage x10 controler
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:	
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400	// Change this to the appropriate value to target Windows 2000 or later.
#endif						

///////////////////////////////////////////////////////
// active home dll load
#if _MSC_VER > 1200
#import "progid:X10.ActiveHome" 
#else
#import "c:\\program files\\common files\\x10\\common\\ahscript.dll"
#endif
///////////////////////////////////////////////////////

// system headers
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
#include <math.h>
#include <objbase.h>

//#include <iostream>
//#include <tchar.h>

// custom headers
#include "config/config.h"
#include "logger/logger.h"
#include "dataport/winserial.h"
#include "x10ping.h"

// main header
#include "x10.h"

// ugly crap global here
ActiveHomeScriptLib::IActiveHome* m_pActiveHome;

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CX10
// Purose:	build my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CX10::CX10( )
{
	m_pActiveHome = NULL;

	// map codes from cm11a x10 to unit
	m_vectCM11ToUnit[0] = 06;
	m_vectCM11ToUnit[1] = 016;
	m_vectCM11ToUnit[2] = 02;
	m_vectCM11ToUnit[3] = 012;
	m_vectCM11ToUnit[4] = 01;
	m_vectCM11ToUnit[5] = 011;
	m_vectCM11ToUnit[6] = 05;
	m_vectCM11ToUnit[7] = 015;
	m_vectCM11ToUnit[8] = 07;
	m_vectCM11ToUnit[9] = 017;
	m_vectCM11ToUnit[10] = 03;
	m_vectCM11ToUnit[11] = 013;
	m_vectCM11ToUnit[12] = 0;
	m_vectCM11ToUnit[13] = 010;
	m_vectCM11ToUnit[14] = 04;
	m_vectCM11ToUnit[15] = 014;

	// map codes from unit to cm11a x10
	m_vectUnitToCM11[0] = 13;
	m_vectUnitToCM11[1] = 5;
	m_vectUnitToCM11[2] = 3;
	m_vectUnitToCM11[3] = 11;
	m_vectUnitToCM11[4] = 15;
	m_vectUnitToCM11[5] = 7;
	m_vectUnitToCM11[6] = 1;
	m_vectUnitToCM11[7] = 9;
	m_vectUnitToCM11[8] = 14;
	m_vectUnitToCM11[9] = 6;
	m_vectUnitToCM11[10] = 4;
	m_vectUnitToCM11[11] = 12;
	m_vectUnitToCM11[12] = 16;
	m_vectUnitToCM11[13] = 8;
	m_vectUnitToCM11[14] = 2;
	m_vectUnitToCM11[15] = 10;

	// cm11a/cm12u house codes
	m_vectHouseToCM11[0] = 06;
	m_vectHouseToCM11[1] = 016;
	m_vectHouseToCM11[2] = 02;
	m_vectHouseToCM11[3] = 012;
	m_vectHouseToCM11[4] = 01;
	m_vectHouseToCM11[5] = 011;
	m_vectHouseToCM11[6] = 05;
	m_vectHouseToCM11[7] = 015;
	m_vectHouseToCM11[8] = 07;
	m_vectHouseToCM11[9] = 017;
	m_vectHouseToCM11[10] = 03;
	m_vectHouseToCM11[11] = 013;
	m_vectHouseToCM11[12] = 0;
	m_vectHouseToCM11[13] = 010;
	m_vectHouseToCM11[14] = 04;
	m_vectHouseToCM11[15] = 014;

	////////////////////////////////
	// stargate defines
	m_vectStargateToUnit[0] = 0;

	m_vectUnitToStargate[0] = 6;
	m_vectUnitToStargate[1] = 7;
	m_vectUnitToStargate[2] = 4;
	m_vectUnitToStargate[3] = 5;
	m_vectUnitToStargate[4] = 8;
	m_vectUnitToStargate[5] = 9;
	m_vectUnitToStargate[6] = 10;
	m_vectUnitToStargate[7] = 11;
	m_vectUnitToStargate[8] = 14;
	m_vectUnitToStargate[9] = 15;
	m_vectUnitToStargate[10] = 12;
	m_vectUnitToStargate[11] = 13;
	m_vectUnitToStargate[12] = 0;
	m_vectUnitToStargate[13] = 1;
	m_vectUnitToStargate[14] = 2;
	m_vectUnitToStargate[15] = 3;

	// stargate house mapping
	m_vectHouseToStargate[0] = 6;
	m_vectHouseToStargate[1] = 7;
	m_vectHouseToStargate[2] = 4;
	m_vectHouseToStargate[3] = 5;
	m_vectHouseToStargate[4] = 8;
	m_vectHouseToStargate[5] = 9;
	m_vectHouseToStargate[6] = 10;
	m_vectHouseToStargate[7] = 11;
	m_vectHouseToStargate[8] = 14;
	m_vectHouseToStargate[9] = 15;
	m_vectHouseToStargate[10] = 12;
	m_vectHouseToStargate[11] = 13;
	m_vectHouseToStargate[12] = 0;
	m_vectHouseToStargate[13] = 1;
	m_vectHouseToStargate[14] = 2;
	m_vectHouseToStargate[15] = 3;

	// se default house code A
//	m_nHouseCode = HOUSE_CODE_A;

	// set serial port to defaul
	m_nSerialPort = X10_DEFAULT_SERIAL_PORT;

	m_pSerialPort = new CSerial( );
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CX10
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CX10::~CX10( )
{
	// reset/delete active home
	if( m_pActiveHome )
	{
		int nRet = m_pActiveHome->Release();
		m_pActiveHome = NULL;
		CoUninitialize();
	}
	// close serial port
	m_pSerialPort->Close( );
	Sleep( 200 );
	delete( m_pSerialPort );
	m_pSerialPort = NULL;
	m_pLogger = NULL;
	m_pConfig = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:	Init
// Class:	CX10
// Purose:	initialize my x10 system
// Input:	nothing
// Output:	status
////////////////////////////////////////////////////////////////////
int CX10::Init( )
{
	int nStatus = 0;
	char strMsg[255];

	// Update config from config object
	UpdateConfig( );
	
	// close serial port if open
	m_pSerialPort->Close( );
	// reset/delete active home
	if( m_pActiveHome )
	{
		int nRet = m_pActiveHome->Release();
		m_pActiveHome = NULL;
		CoUninitialize();
	}

	// if cm11a/cm12u
	if( m_nControllerTyle == X10_CONTROLLER_TYPE_CM12U )
	{
		// open serial port
		if( !m_pSerialPort->Open( m_nSerialPort, 4800 ) ) 
		{
			sprintf( strMsg, "ERROR :X10: unable to open serial port=%d", m_nSerialPort );
			m_pLogger->Log( strMsg );

			return( -1 );
		} else
		{
			sprintf( strMsg, "INFO :X10: serial port=%d succesfully opened", m_nSerialPort );
			m_pLogger->Log( strMsg );
			// now initialize disable ring? ##%1c\r
			unsigned char strCmd[10];
			// do a small loop looking for power failt problem on init
			int nCount = 0;
			while( nCount < 5 )
			{

				nCount++;
				m_pSerialPort->ReadData( strCmd, 1 );
				// set clock if to power fail
				if( strCmd[0] == 0xa5 ) CM11A_SetClock( );
			}
		}

	} else if( m_nControllerTyle == X10_CONTROLLER_TYPE_STARGATE )
	{
		// open serial port
		if( !m_pSerialPort->Open( m_nSerialPort, 9600 ) ) 
		{
			sprintf( strMsg, "ERROR :X10: unable to open serial port=%d", m_nSerialPort );
			m_pLogger->Log( strMsg );

			return( -1 );
		} else
		{
			sprintf( strMsg, "INFO :X10: serial port=%d succesfully opened", m_nSerialPort );
			m_pLogger->Log( strMsg );
			// now initialize the stargate for echo off? // ##%1c\r
			unsigned char strCmd[10];
			strCmd[0] = '#';
			strCmd[1] = '#';
			strCmd[2] = '%';
			strCmd[3] = '1';
			strCmd[4] = 'c';
			strCmd[5] = '\r';
			// send my init string to the controler
			m_pSerialPort->SendData( strCmd, 6 );
		}

	} else if( m_nControllerTyle == X10_CONTROLLER_TYPE_CM15A_PLC ||
				m_nControllerTyle == X10_CONTROLLER_TYPE_CM15A_RF ||
				m_nControllerTyle == X10_CONTROLLER_TYPE_CM19A )
	{

		CoInitializeEx(NULL, COINIT_MULTITHREADED);
//		ActiveHomeScriptLib::IActiveHome* pActiveHome;
		HRESULT hr = CoCreateInstance( __uuidof(ActiveHomeScriptLib::ActiveHome), 
				NULL, CLSCTX_INPROC, __uuidof(ActiveHomeScriptLib::IActiveHome), 
				(LPVOID *) &m_pActiveHome );

		// check connection
		if(! SUCCEEDED(hr) )
		{
			m_pLogger->Log( "ERROR :activehome: cannot load interface to ActiveHome." );
			m_pLogger->Log( "ERROR :activehome: perhaps, ActiveHome is not installed." );

			CoUninitialize();

			m_pActiveHome = NULL;
		} else
		{
			m_pLogger->Log( "INFO :activehome: interface to ActiveHome was succesfully opened" );
		}
	}

	return( nStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	SetHouseCode
// Class:	CX10
// Purose:	set house code local
// Input:	house code
// Output:	nothing
////////////////////////////////////////////////////////////////////
//void CX10::SetHouseCode( unsigned int nHouseCode )
//{
//	m_nHouseCode = nHouseCode;
//	return;
//}

////////////////////////////////////////////////////////////////////
// Method:	Update Config
// Class:	CX10
// Purose:	update configuration variables
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CX10::UpdateConfig( )
{
	// check if valid reference
	if( !m_pConfig ) return;

	int bChange = 0;

	// get serial port number
	if( m_pConfig->GetIntVar( SECTION_ID_X10, CONF_X10_SERIALPORT, m_nSerialPort ) == 0 )
	{
		m_nSerialPort = X10_DEFAULT_SERIAL_PORT;
		m_pConfig->SetIntVar( SECTION_ID_X10, CONF_X10_SERIALPORT, m_nSerialPort );
		bChange = 1;
	}
	// get controler type
	if( m_pConfig->GetIntVar( SECTION_ID_X10, CONF_X10_CONTROLLER_MODEL, m_nControllerTyle ) == 0 )
	{
		m_nControllerTyle = X10_CONTROLLER_TYPE_CM12U;
		if( m_pConfig->SetIntVar( SECTION_ID_X10, CONF_X10_CONTROLLER_MODEL, m_nControllerTyle ) > 0 )
		{
			bChange = 1;
		}
	}

	// check if default updates then save config
	if( bChange > 0 ) m_pConfig->Save( );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	CM11A_CheckSum
// Class:	CX10
// Purose:	check the CM11A's acknowledgment to the data we send it.
//			It's a simple addition of all bytes.
// Input:	data buffer and buffer length
// Output:	checksum
////////////////////////////////////////////////////////////////////
int CX10::CM11A_CheckSum( unsigned char *vectBuf, int nSize )
{
    register int i, sum = 0;

    for (i = 0, sum = 0; i < nSize ; i++ )
	sum += vectBuf[i];
    return (sum & 0xFF);
}

////////////////////////////////////////////////////////////////////
// Method:	TurnUnitOn
// Class:	CX10
// Purose:	turn a specific unit on
// Input:	unit number
// Output:	status 0 - failure, 1 - success
////////////////////////////////////////////////////////////////////
int CX10::TurnUnitOn( unsigned char nHouse, unsigned char nUnit )
{
	int nCounter = 0;
	int nStatus = 0;

	// send x10 command by controller type
	switch( m_nControllerTyle )
	{
		// if cm11a/cm12u
		case X10_CONTROLLER_TYPE_CM12U:
		{

			while( nStatus == 0 && nCounter < X10_MAX_NO_RETRY_COMMAND )
			{
				nStatus = CM11A_ExecFunc( nHouse, nUnit, X10_FUNC_UNIT_ON );
				Sleep( 100 );
				nCounter++;
			}
			break;
		}

		// if cm15a controller
		case X10_CONTROLLER_TYPE_CM15A_PLC:
		{
			nStatus = ActiveHome_ExecFunc( nHouse, nUnit, X10_FUNC_UNIT_ON, 0 );
			Sleep( 100 );
			nCounter++;
			nStatus = 1;

			break;
		}

		// if cm15a controller
		case X10_CONTROLLER_TYPE_CM15A_RF:
		{
			nStatus = ActiveHome_ExecFunc( nHouse, nUnit, X10_FUNC_UNIT_ON, 1 );
			Sleep( 100 );
			nCounter++;
			nStatus = 1;

			break;
		}

		// if cm19a controller
		case X10_CONTROLLER_TYPE_CM19A:
		{
			nStatus = ActiveHome_ExecFunc( nHouse, nUnit, X10_FUNC_UNIT_ON, 1 );
			Sleep( 100 );
			nCounter++;
			nStatus = 1;

			break;
		}

		// if JDS Technologies Stargate
		case X10_CONTROLLER_TYPE_STARGATE:
		{
			nStatus = Stargate_ExecFunc( nHouse, nUnit, X10_FUNC_UNIT_ON );
			Sleep( 100 );
			nCounter++;

			break;
		}

		// default do nothing
		default: break;
	}

	return( nStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	TurnUnitOff
// Class:	CX10
// Purose:	turn a specific unit off
// Input:	unit number
// Output:	status 0 - failure, 1 - success
////////////////////////////////////////////////////////////////////
int CX10::TurnUnitOff( unsigned char nHouse, unsigned char nUnit )
{
	int nCounter = 0;
	int nStatus = 0;

	// send x10 command by controller type
	switch( m_nControllerTyle )
	{
		// if cm11a/cm12u
		case X10_CONTROLLER_TYPE_CM12U:
		{
			while( nStatus == 0 && nCounter < X10_MAX_NO_RETRY_COMMAND )
			{
				nStatus = CM11A_ExecFunc( nHouse, nUnit, X10_FUNC_UNIT_OFF );
				Sleep( 100 );
				nCounter++;
			}
			break;
		}

		// if cm15a controller
		case X10_CONTROLLER_TYPE_CM15A_PLC:
		{
			nStatus = ActiveHome_ExecFunc( nHouse, nUnit, X10_FUNC_UNIT_OFF, 0 );
			Sleep( 100 );
			nCounter++;
			nStatus = 1;
			break;
		}

		// if cm15a controller
		case X10_CONTROLLER_TYPE_CM15A_RF:
		{
			nStatus = ActiveHome_ExecFunc( nHouse, nUnit, X10_FUNC_UNIT_OFF, 1 );
			Sleep( 100 );
			nCounter++;
			nStatus = 1;
			break;
		}

		// if cm15a controller
		case X10_CONTROLLER_TYPE_CM19A:
		{
			nStatus = ActiveHome_ExecFunc( nHouse, nUnit, X10_FUNC_UNIT_OFF, 1 );
			Sleep( 100 );
			nCounter++;
			nStatus = 1;
			break;
		}

		// if JDS Technologies Stargate
		case X10_CONTROLLER_TYPE_STARGATE:
		{
			nStatus = Stargate_ExecFunc( nHouse, nUnit, X10_FUNC_UNIT_OFF );
			Sleep( 100 );
			nCounter++;
			break;
		}

		// default do nothing
		default: break;
	}
 
	return( nStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	ActiveHome_ExecFunc
// Class:	CX10
// Purose:	execute a function and a specific unit for cm15a with
//			activehome scripting service
// Input:	house id, unit number, function id
// Output:	status 0 - failure, 1 - success
////////////////////////////////////////////////////////////////////
int CX10::ActiveHome_ExecFunc( unsigned char nHouse, unsigned char nUnit, 
							int nFunction, int nSendType )
{
    long nReturn = 0;
	char strMsg[255];
	wxString strSendType = "sendplc";

//	CoInitializeEx(NULL, COINIT_MULTITHREADED);
//	ActiveHomeScriptLib::IActiveHome* pActiveHome;
//	HRESULT hr = CoCreateInstance( __uuidof(ActiveHomeScriptLib::ActiveHome), NULL, CLSCTX_INPROC, __uuidof(ActiveHomeScriptLib::IActiveHome), (LPVOID *) &pActiveHome );
	if( m_pActiveHome != NULL )
	{
        _variant_t varReseved1;
        _variant_t varReseved2;
        _variant_t varReturn;
		char strParam[255];
		
		switch( nFunction )
		{
			case X10_FUNC_UNIT_ON:
			{
				sprintf( strParam, "%s%d on", m_vectStrX10House[nHouse-1].Lower().GetData(), nUnit );
				break;
			}

			case X10_FUNC_UNIT_OFF:
			{
				sprintf( strParam, "%s%d off", m_vectStrX10House[nHouse-1].Lower().GetData(), nUnit );
				break;
			}

			default: break;
		}
	
		// check by send type 
		if( nSendType == 0 )
			strSendType = "sendplc";
		else if( nSendType == 1 )
			strSendType = "sendrf";

		// debug
		sprintf( strMsg, "DEBUG :activehome: send command = %s %s", strSendType.GetData(), strParam );
		m_pLogger->Log( strMsg );

		varReturn = m_pActiveHome->SendAction( strSendType.GetData(), strParam, varReseved1, varReseved2);
		nReturn = varReturn;

//		int nRet = pActiveHome->Release();
//       pActiveHome = NULL;
	} else
	{
		m_pLogger->Log( "ERROR :activehome: ActiveHome interface wasn't initialized" );
//		m_pLogger->Log( "ERROR :activehome: perhaps, ActiveHome is not installed." );
	}

//	CoUninitialize();

	return( (int) nReturn );
}

////////////////////////////////////////////////////////////////////
// Method:	Stargate_ExecFunc
// Class:	CX10
// Purose:	execute a function and a specific unit for jds stargate
// Input:	house id, unit number, function id
// Output:	status 0 - failure, 1 - success
////////////////////////////////////////////////////////////////////
int CX10::Stargate_ExecFunc( unsigned char nHouse, unsigned char nUnit, 
							int nFunction )
{
	int nStatus = 0;
	unsigned char vectBuf[20];
	char vectTemp[10];
	unsigned char nTemp;
	char strMsg[255];

	unsigned char strOk[3];
	strOk[0] = '#';
	strOk[1] = '#';
	strOk[1] = '0';

	// map unit -1 
	unsigned int nUnitCode = m_vectUnitToStargate[nUnit-1];
	unsigned int nHouseCode = m_vectHouseToStargate[nHouse-1];

	// build address header first to select the units
	vectBuf[0] = '#';
	vectBuf[1] = '#';
	vectBuf[2] = '%';
	vectBuf[3] = '0';
	vectBuf[4] = '4';
	vectBuf[5] = '0';
	vectBuf[6] = '0';

	// build byte from code(4bits) and unit(4bits)
	nTemp = ( nUnitCode << 4 ) | nHouseCode;
	// convert to string unit/house code
	sprintf( vectTemp, "%.2x", nTemp );
	vectBuf[7] = vectTemp[0];
	vectBuf[8] = vectTemp[1];
	vectBuf[9] = '\r';
	vectBuf[10] = 0;

	// debug
	sprintf( strMsg, "DEBUG :jds: send hdr cmd = %s", vectBuf );
	m_pLogger->Log( strMsg );

	// send my header data to the controler
	m_pSerialPort->SendData( vectBuf, 10 );
	Sleep( 100 );

	// build address header first to select the units
	vectBuf[0] = '#';
	vectBuf[1] = '#';
	vectBuf[2] = '%';
	vectBuf[3] = '0';
	vectBuf[4] = '4';
	vectBuf[5] = '0';
	vectBuf[6] = '1';

	switch( nFunction )
	{
		case X10_FUNC_UNIT_ON:
		{
			nTemp = 64 | nHouseCode;
			break;
		}

		case X10_FUNC_UNIT_OFF:
		{
			nTemp = 192 | nHouseCode;
			break;
		}

		default: break;
	}
	// convert command housecode to hex string
	sprintf( vectTemp, "%.2x", nTemp );
	vectBuf[7] = vectTemp[0];
	vectBuf[8] = vectTemp[1];
	vectBuf[9] = '\r';
	vectBuf[10] = 0;

	// debug
	sprintf( strMsg, "DEBUG :jds: send cmd = %s", vectBuf );
	m_pLogger->Log( strMsg );

	// send my header data to the controler
	m_pSerialPort->SendData( vectBuf, 10 );

	nStatus = 1;

	return( nStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	CM11A_ExecFunc
// Class:	CX10
// Purose:	execute a function and a specific unit for cm11a/cm12u
// Input:	unit number, function id
// Output:	status 0 - failure, 1 - success
////////////////////////////////////////////////////////////////////
int CX10::CM11A_ExecFunc( unsigned char nHouse, unsigned char nUnit, int nFunction )
{
	int nStatus = 0;
	unsigned char vectBuf[20];
	unsigned int nSum;
	unsigned int nDim = 0;
//	unsigned int nBits = 0;
	int nExtendedFl = 0;
	unsigned char strOk[2];
	strOk[0]=0x00;
	strOk[1]=0x00;


	// map unit -1 
	unsigned int nUnitCode = m_vectCM11ToUnit[nUnit-1];
	unsigned int nHouseCode = m_vectHouseToCM11[nHouse-1];

	// build address header first to select the units
	vectBuf[0] = 0x04;			
	// build byte from code(4bits) and unit(4bits)
	vectBuf[1] = (nHouseCode << 4 ) | nUnitCode;
	
	// send my data to the controler
	m_pSerialPort->SendData( vectBuf, 2 );

	// calculate sum
	nSum = CM11A_CheckSum( vectBuf, 2 );

	int nCount = 0;
	int nFound = 0;

	Sleep( 200 );
	while( nCount < 5 && nFound == 0 )
	{
		Sleep( 100 );
		// read check sum from x10 controler
		m_pSerialPort->ReadData( vectBuf, 1 );

		// if checksum returned by x10 ok 
		if( nSum == vectBuf[0] ) 
		{
			Sleep( 100 );
			// tell x10 we are ready to send command
			m_pSerialPort->SendData( strOk , 1 );
			nFound = 1;

		} else
		{
			nCount++;
			// check if power failure
			if( vectBuf[0] == 0xa5 && nSum != 0xa5 ) CM11A_SetClock( );
		}
	}

	// if error return
	if( nFound == 0 ) 
	{
		m_pLogger->Log( "ERROR :X10: Failure sending address header" );
		return( 0 );
	}

	nCount = 0;
	nFound = 0;

	while( nCount < 10 && nFound == 0 )
	{

		Sleep( 100 );
		// read if interface is also ready
		m_pSerialPort->ReadData( vectBuf, 1 );
		// if interface ready
		if( vectBuf[0] == 0x55 ) 
		{
			nFound = 1;
		} else
			nCount ++;
	}
	// if interface wasn't found ready after x steps
	if( nFound == 0 ) 
	{
		m_pLogger->Log( "ERROR :X10: Could not find the interface ready" );
		return( 0 );
	}
		

	// build a function header with dimming info
	vectBuf[0] = 0x6 | nDim | nExtendedFl;	
    vectBuf[1] = (nHouseCode << 4) | nFunction;
 
	Sleep( 200 );	
	// send command to turn on
	m_pSerialPort->SendData( vectBuf, 2 );

	// calculate sum - again - TODO : have this in a method
	nSum = CM11A_CheckSum( vectBuf, 2 );


	nCount = 0;
	nFound = 0;

	Sleep( 200 );
	while( nCount < 5 && nFound == 0 )
	{

		// read check sum from x10 controler
		m_pSerialPort->ReadData( vectBuf, 1 );

		// if checksum returned by x10 ok 
		if( nSum == vectBuf[0]) 
		{
			Sleep( 100 );
			// tell x10 we are ready to send command
			m_pSerialPort->SendData( strOk , 1 );
			nStatus = 1;
			nFound = 1;

		} else
		{
			nCount++;
		}
	}

	// if error return
	if( nFound == 0 ) 
	{
		m_pLogger->Log( "ERROR :X10: Failure sending command function" );
		return( 0 );
	}


	return( nStatus );
}

////////////////////////////////////////////////////////////////////
// Method:	CM11A_SetClock
// Class:	CX10
// Purose:	set cm11a clock
// Input:	unit number, function id
// Output:	status 0 - failure, 1 - success
////////////////////////////////////////////////////////////////////
int CX10::CM11A_SetClock( )
{
	unsigned char strCmd[10];
	time_t t = time(NULL);
	struct tm* tp = localtime( &t );

	// set clock
    strCmd[0] = 0x9b;		// CM11A timer download code
    strCmd[1] = tp->tm_sec ;								// seconds		
    strCmd[2] = tp->tm_min + (((tp->tm_hour) %2) * 60 );	// minutes 0 - 119 
    strCmd[3] = tp->tm_hour / 2;							// hour / 2 - 0 - 11 
    strCmd[4] = tp->tm_yday  % 256;				// mantisa of julian date 
    strCmd[5] = ((tp->tm_yday / 256 ) << 7);	// radix of julian date 
    strCmd[5] |= (0x01 << (tp->tm_wday));		// bits 0-6 =  single bit mask day 
    											// of week ( smtwtfs ) 
//    strCmd[6] = (x10_housecode << 4);
//    strCmd[6] |= clear;
	// use this instead
	strCmd[6] = 0x03;
	
	// send data
	m_pSerialPort->SendData( strCmd, 1 );
	// receive but ignore
	m_pSerialPort->ReadData( strCmd, 1 );
	// send ok
	strCmd[0] = 0x00;
	m_pSerialPort->SendData( strCmd, 1 );

	return( 1 );
}
