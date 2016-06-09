////////////////////////////////////////////////////////////////////
// Package:		x10 definition
// File:		x10.h
// Purpose:		manage x10 controler
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:	
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _X10_H
#define _X10_H

// house codes
//#define HOUSE_CODE_A	06
//#define HOUSE_CODE_B	016
//#define HOUSE_CODE_C	02
//#define HOUSE_CODE_D	012
//#define HOUSE_CODE_E	01
//#define HOUSE_CODE_F	011
//#define HOUSE_CODE_G	05
//#define HOUSE_CODE_H	015
//#define HOUSE_CODE_I	07
//#define HOUSE_CODE_J	017
//#define HOUSE_CODE_K	03
//#define HOUSE_CODE_L	013
//#define HOUSE_CODE_M	0
//#define HOUSE_CODE_N	010
//#define HOUSE_CODE_O	04
//#define HOUSE_CODE_P	014

// define x10 function codes
#define X10_FUNC_UNIT_ON	0x02
#define X10_FUNC_UNIT_OFF	0x03

// config defaults
#define X10_DEFAULT_SERIAL_PORT			1
// x10 interface type
#define X10_CONTROLLER_TYPE_CM12U		1
#define X10_CONTROLLER_TYPE_CM15A_PLC	2
#define X10_CONTROLLER_TYPE_CM15A_RF	3
#define X10_CONTROLLER_TYPE_CM19A		4
#define X10_CONTROLLER_TYPE_STARGATE	5

// other defines
#define X10_MAX_NO_RETRY_COMMAND	5

// external classes
class CSerial;
class CLogger;
class CConfig;

/////////////////////////
// class :	cx10
class CX10
{

// public methods
public:
	CX10();
	~CX10();
	int Init( );
//	void SetHouseCode( unsigned int nHouseCode );
	void UpdateConfig( );
	int CM11A_CheckSum( unsigned char *vectBuf, int nSize );
	int CM11A_SetClock( );
	int TurnUnitOn( unsigned char nHouse, unsigned char nUnit );
	int TurnUnitOff( unsigned char nHouse, unsigned char nUnit );
	int CM11A_ExecFunc( unsigned char nHouse, unsigned char nUnit, int nFunction );
	int ActiveHome_ExecFunc( unsigned char nHouse, unsigned char nUnit, int nFunction, int nSendType );
	int Stargate_ExecFunc( unsigned char nHouse, unsigned char nUnit, int nFunction );

// public data
public:
	CSerial *m_pSerialPort;
	CLogger *m_pLogger;
	CConfig *m_pConfig;
	int m_nSerialPort;
	int m_nControllerTyle;

//	unsigned int m_nHouseCode;

	// vector for mapping units (1-16) or house codes (a-p) to 4 bit 
	// x10(cm12u/cm11a) codes element 0 = unit 1 or code a
	unsigned char m_vectCM11ToUnit[16];

	// vector for mapping an x10 format bitmap to unit number
	unsigned char m_vectUnitToCM11[16];

	// cm11a/cm12u house mapping
	unsigned char m_vectHouseToCM11[16];

	// stargate jds x10 module mappings
	unsigned char m_vectStargateToUnit[16];
	unsigned char m_vectUnitToStargate[16];

	// stargate house mapping
	unsigned char m_vectHouseToStargate[16];
};

#endif
