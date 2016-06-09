////////////////////////////////////////////////////////////////////
// Package:		Cping implementation
// File:		ping.cpp
// Purpose:		doest net ping
//
// Created by:	
// Updated by:	Larry Lart on 22-Apr-2006
//
////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// custom headers
#include "logger/logger.h"

// main header
#include "ping.h"

/////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CPing
// Purpose:	create my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CPing::CPing( )
{
	m_pLogger = NULL;
}

/////////////////////////////////////////////////////////////////////
// Method:	Init
// Class:	CPing
// Purpose:	intialize my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CPing::Init( )
{
	bValid = FALSE;
	WSADATA wsaData;			// WSADATA
	int nRet;					// General use return code

	// Dynamically load the ICMP.DLL
	hndlIcmp = LoadLibrary("ICMP.DLL");
	if (hndlIcmp == NULL)
	{
		m_pLogger->Log( "ERROR :ping: could not load ICMP.DLL" );
		return;
	}
	// Retrieve ICMP function pointers
	pIcmpCreateFile  = (HANDLE (WINAPI *)(void))
		GetProcAddress((HMODULE)hndlIcmp,"IcmpCreateFile");
	pIcmpCloseHandle = (BOOL (WINAPI *)(HANDLE))
		GetProcAddress((HMODULE)hndlIcmp,"IcmpCloseHandle");
	pIcmpSendEcho = (DWORD (WINAPI *)
		(HANDLE,DWORD,LPVOID,WORD,PIPINFO,LPVOID,DWORD,DWORD))
		GetProcAddress((HMODULE)hndlIcmp,"IcmpSendEcho");
	// Check all the function pointers
	if (pIcmpCreateFile == NULL		|| 
		pIcmpCloseHandle == NULL	||
		pIcmpSendEcho == NULL)
	{
		m_pLogger->Log( "ERROR :: failed loading ICMP.DLL" );
		FreeLibrary((HMODULE)hndlIcmp);
		return;
	}

	// Init WinSock
	nRet = WSAStartup(0x0101, &wsaData );
    if( nRet )
    {
		m_pLogger->Log( "ERROR :: WSAStartup() failed" );
        WSACleanup();
		FreeLibrary((HMODULE)hndlIcmp);
        return;
    }
    // Check WinSock version
    if (0x0101 != wsaData.wVersion)
    {
		m_pLogger->Log( "ERROR :: no WinSock version 1.1 support found" );
        WSACleanup();
		FreeLibrary((HMODULE)hndlIcmp);
        return;
    }
	bValid = TRUE;
}

/////////////////////////////////////////////////////////////////////
// Method:	Set Logger
// Class:	CPing
// Purpose:	set my logger
// Input:	pointer to logger
// Output:	nothing
/////////////////////////////////////////////////////////////////////
void CPing::SetLogger( CLogger* pLogger )
{
	m_pLogger = pLogger;
	return;
}

/////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CPing
// Purpose:	destroy/delete my object
// Input:	nothing
// Output:	nothing
/////////////////////////////////////////////////////////////////////
CPing::~CPing()
{
    WSACleanup();
	FreeLibrary((HMODULE)hndlIcmp);
}

/////////////////////////////////////////////////////////////////////
// Method:	Ping
// Class:	CPing
// Purpose:	does ping on the net to host/ip
// Input:	pointer to string ip/host name
// Output:	0 = failed, 1=ok
/////////////////////////////////////////////////////////////////////
int CPing::Ping(char* strHost)
{
	struct in_addr iaDest;		// Internet address structure
    LPHOSTENT pHost;			// Pointer to host entry structure
	DWORD *dwAddress;			// IP Address
	IPINFO ipInfo;				// IP Options structure
	ICMPECHO icmpEcho;			// ICMP Echo reply buffer
	HANDLE hndlFile;			// Handle for IcmpCreateFile()

    if(!bValid)
	{
		return FALSE;
	}

	// Lookup destination
    // Use inet_addr() to determine if we're dealing with a name
    // or an address
	iaDest.s_addr = inet_addr(strHost); 
	if (iaDest.s_addr == INADDR_NONE) 
	{ 
		pHost = gethostbyname(strHost); 
		if (pHost == NULL) 
		{ 
//			char szText[255] ; 
//			DWORD dw = WSAGetLastError() ; 
//			sprintf( szText , "ERROR :: WSA Error = %d" , dw ) ;
//			m_pLogger->Log( szText ); 
			return FALSE; 
		}
		
		// Copy the IP address 
		dwAddress = (DWORD *)(*pHost->h_addr_list); 
	} else 
	{ 
		pHost = gethostbyaddr((const char *)&iaDest, 
		sizeof(struct in_addr), AF_INET); 
		if (pHost == NULL) 
		{ 
			// Copy the IP address 
			dwAddress = &iaDest.s_addr ; 
		} else 
			dwAddress = (DWORD *)(*pHost->h_addr_list); 
	} 


	// Get an ICMP echo request handle        
	hndlFile = pIcmpCreateFile();

	// Set some reasonable default values
	ipInfo.Ttl = 255;
	ipInfo.Tos = 0;
	ipInfo.IPFlags = 0;
	ipInfo.OptSize = 0;
	ipInfo.Options = NULL;
	icmpEcho.Status = 0;
	// Reqest an ICMP echo
	pIcmpSendEcho(
		hndlFile,		// Handle from IcmpCreateFile()
		*dwAddress,		// Destination IP address
		NULL,			// Pointer to buffer to send
		0,				// Size of buffer in bytes
		&ipInfo,		// Request options
		&icmpEcho,		// Reply buffer
		sizeof(struct tagICMPECHO),
		1000);			// Time to wait in milliseconds
	// Print the results
	iaDest.s_addr = icmpEcho.Source;
	if (icmpEcho.Status)
	{
		return FALSE;
	}

	// Close the echo request file handle
	pIcmpCloseHandle(hndlFile);
	return TRUE;
}
