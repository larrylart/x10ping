////////////////////////////////////////////////////////////////////
// Package:		functions implementation
// File:		func.cpp
// Purpose:		build a library of global functions
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:  
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#include "func.h"

// define function bzero for windows
#ifdef _WINDOWS
int bzero( char* pString, unsigned int nString )
{
	unsigned long no;

	for( no=0; no< nString; no++ ) { pString[no] = 0; }

	return( 0 );
}
#endif

// define function Sleep if not windows
#ifndef _WINDOWS
void Sleep( unsigned long nTimeout )
{ 
	usleep( nTimeout );
}
#endif
