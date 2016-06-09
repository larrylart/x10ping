////////////////////////////////////////////////////////////////////
// Package:		functions declaration
// File:		func.h
// Purpose:		build a library of global functions
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:  
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _FUNC_H
#define _FUCN_H

// define fubction bzero for windows
#ifdef WIN32 
int bzero( char* pString, unsigned int nString );
#endif

// define sleep
#ifndef WIN32 
#include <unistd.h>
void Sleep( unsigned long nTimeout );
#endif

#endif
