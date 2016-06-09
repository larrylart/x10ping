////////////////////////////////////////////////////////////////////
// Name:		windows serial port implementation
// File:		winserial.cpp
// Purpose:		to process serial port data
//
// Created by:	Unknown
// Updated by:  Larry Lart on 22-Apr-2006
//
////////////////////////////////////////////////////////////////////

#include "../StdAfx.h"
#include "winserial.h"

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CSerial
// Purose:	build my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CSerial::CSerial( )
{
	memset( &m_OverlappedRead, 0, sizeof( OVERLAPPED ) );
 	memset( &m_OverlappedWrite, 0, sizeof( OVERLAPPED ) );
	m_hIDComDev = NULL;
	m_bOpened = FALSE;
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CSerial
// Purose:	destroy my object
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CSerial::~CSerial( )
{
	Close( );
}

////////////////////////////////////////////////////////////////////
// Method:	Open
// Class:	CSerial
// Purose:	Open a serial port
// Input:	int port and boud rate
// Output:	if port was opened or not
////////////////////////////////////////////////////////////////////
BOOL CSerial::Open( int nPort, int nBaud )
{

	if( m_bOpened ) return( TRUE );

	char szPort[15];
	char szComParams[50];
	DCB dcb;

	wsprintf( szPort, "COM%d", nPort );
	m_hIDComDev = CreateFile( szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL );
	if( m_hIDComDev == NULL ) return( FALSE );

	memset( &m_OverlappedRead, 0, sizeof( OVERLAPPED ) );
 	memset( &m_OverlappedWrite, 0, sizeof( OVERLAPPED ) );

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts( m_hIDComDev, &CommTimeOuts );

	wsprintf( szComParams, "COM%d:%d,n,8,1", nPort, nBaud );

	m_OverlappedRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_OverlappedWrite.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	dcb.DCBlength = sizeof( DCB );
	GetCommState( m_hIDComDev, &dcb );
	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	dcb.Parity=NOPARITY;
	dcb.StopBits=ONESTOPBIT;

	unsigned char ucSet;
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_DTRDSR ) != 0 );
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_RTSCTS ) != 0 );
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_XONXOFF ) != 0 );

	SetCommState( m_hIDComDev, &dcb );

	if( !SetupComm( m_hIDComDev, 10000, 10000 ) ||
		m_OverlappedRead.hEvent == NULL ||
		m_OverlappedWrite.hEvent == NULL )
	{
		DWORD dwError = GetLastError();
		if( m_OverlappedRead.hEvent != NULL ) CloseHandle( m_OverlappedRead.hEvent );
		if( m_OverlappedWrite.hEvent != NULL ) CloseHandle( m_OverlappedWrite.hEvent );
		CloseHandle( m_hIDComDev );

		return( FALSE );
	}

	m_bOpened = TRUE;

	return( m_bOpened );

}

////////////////////////////////////////////////////////////////////
// Method:	Close
// Class:	CSerial
// Purose:	close my serial connection
// Input:	void
// Output:	boolean status
////////////////////////////////////////////////////////////////////
BOOL CSerial::Close( void )
{

	if( !m_bOpened || m_hIDComDev == NULL ) return( TRUE );

	if( m_OverlappedRead.hEvent != NULL ) CloseHandle( m_OverlappedRead.hEvent );
	if( m_OverlappedWrite.hEvent != NULL ) CloseHandle( m_OverlappedWrite.hEvent );
	CloseHandle( m_hIDComDev );
	m_bOpened = FALSE;
	m_hIDComDev = NULL;

	return( TRUE );

}

////////////////////////////////////////////////////////////////////
// Method:	Write Comm Byte
// Class:	CSerial
// Purose:	write a byte to the comm
// Input:	unsigned char byte
// Output:	boolean status
////////////////////////////////////////////////////////////////////
BOOL CSerial::WriteCommByte( unsigned char ucByte )
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat = WriteFile( m_hIDComDev, (LPSTR) &ucByte, 1, &dwBytesWritten, 
							&m_OverlappedWrite );
	if( !bWriteStat && ( GetLastError() == ERROR_IO_PENDING ) )
	{
		if( WaitForSingleObject( m_OverlappedWrite.hEvent, 1000 ) ) 
			dwBytesWritten = 0;
		else
		{
			GetOverlappedResult( m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE );
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}

	return( TRUE );

}

////////////////////////////////////////////////////////////////////
// Method:	Send Data
// Class:	CSerial
// Purose:	send data to serial port
// Input:	pointer to data and size of data
// Output:	number of bytes sent 
////////////////////////////////////////////////////////////////////
int CSerial::SendData( const unsigned char *buffer, int size )
{

	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );

	DWORD dwBytesWritten = 0;
	int i;
	for( i=0; i<size; i++ )
	{
		WriteCommByte( buffer[i] );
		dwBytesWritten++;
	}

	return( (int) dwBytesWritten );

}

////////////////////////////////////////////////////////////////////
// Method:	Read Data Waiting 
// Class:	CSerial
// Purose:	check to see how many bytes are waiting to be read
// Input:	void
// Output:	nunber of bytes waiting to be read
////////////////////////////////////////////////////////////////////
int CSerial::ReadDataWaiting( void )
{
	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );

	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );

	return( (int) ComStat.cbInQue );
}

////////////////////////////////////////////////////////////////////
// Method:	Read Data
// Class:	CSerial
// Purose:	read data from serial port
// Input:	pointer to buffer and number of bytes to be read
// Output:	number of bytes read
////////////////////////////////////////////////////////////////////
int CSerial::ReadData( void *buffer, int limit )
{
	if( !m_bOpened || m_hIDComDev == NULL ) return( 0 );

	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	if( !ComStat.cbInQue ) return( 0 );

	dwBytesRead = (DWORD) ComStat.cbInQue;
	if( limit < (int) dwBytesRead ) dwBytesRead = (DWORD) limit;

	bReadStatus = ReadFile( m_hIDComDev, buffer, dwBytesRead, &dwBytesRead, 
							&m_OverlappedRead );
	if( !bReadStatus )
	{
		if( GetLastError() == ERROR_IO_PENDING )
		{
			WaitForSingleObject( m_OverlappedRead.hEvent, 2000 );
			return( (int) dwBytesRead );
		}
		return( 0 );
	}

	return( (int) dwBytesRead );
}
