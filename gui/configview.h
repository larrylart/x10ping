////////////////////////////////////////////////////////////////////
// Package:		Chorg config definition
// File:		configview.h
// Purpose:		define a config view window
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:	
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _CONFIGVIEW_H
#define _CONFIGVIEW_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#define FILE_LOAD_AUDIO_TITLE		"Choose an audio file to open"
#define FILE_LOAD_AUDIO_TYPE		"Audio files (*.wav)|*.wav|All files (*.*)|*.*"

// controls ids
enum
{
	wxID_CHECK_EMAIL_AUTH	= 4000,
	wxID_BUTTON_FILEBROWSE	= 4001,
	wxID_X10_CTRL_SELECT	= 4002
};

// external classes
class CConfig;

// class:	CConfigView
///////////////////////////////////////////////////////
class CConfigView : public wxDialog
{
// public methods
public:
    CConfigView( wxWindow *parent );
	virtual ~CConfigView( );
	void SetConfig( CConfig* pConfig );
	void StoreConfig( );

	void OnEmailAuth( wxCommandEvent& pEvent );
	void OnAudioFileSelect( wxCommandEvent& pEvent );
	void OnSelectCtrlType( wxCommandEvent& event );
	void DisableEmailAuth( );
	void EnableEmailAuth( );

// public data
public:
	CConfig* m_pConfig;
	wxChoice*	m_pSerialPortSelect;
	wxChoice*	m_pX10ControllerType;
	wxTextCtrl*	m_pDevicePingRetryEntry;
	wxTextCtrl* m_pNetLoopCheckDelayEntry;
	wxTextCtrl*	m_pX10DelayAfterOFFEntry;
	wxTextCtrl*	m_pX10DelayAfterONEntry;
	wxTextCtrl* m_pNotifyEmailServer;
	wxTextCtrl* m_pNotifyEmailFrom;
	wxTextCtrl* m_pNotifyEmailTo;
	wxCheckBox*	m_pNotifyEmailAuth;
	wxTextCtrl* m_pNotifyEmailAuthUser;
	wxTextCtrl* m_pNotifyEmailAuthPasswd;
	wxStaticText* m_pNotifyEmailAuthUserLabel;
	wxStaticText* m_pNotifyEmailAuthPasswdLabel;
	wxTextCtrl*		m_pNotifyAudioFile;
	wxCheckBox*	m_pRuntimeStartup;

	DECLARE_EVENT_TABLE()
};


#endif
