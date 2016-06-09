////////////////////////////////////////////////////////////////////
// Package:		main frame definition
// File:		frame.h
// Purpose:		define my main frame structure
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:  
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifndef _FRAME_H
#define _FRAME_H

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers
#include "wx/minifram.h"
#include <wx/listctrl.h>
#include <wx/taskbar.h>
#include <wx/string.h>

// defines
#define DIALOG_DEVICE_FILE_TYPE		"Config files (*.cfg)|*.cfg|All files (*.*)|*.*"
#define DIALOG_SAVE_DEVICE_FILE_TITLE	"Save list of devices"
#define DIALOG_LOAD_DEVICE_FILE_TITLE	"Load list of devices"

class CLogger;
class CConfig;
class CFocuser;
class CX10PingApp;
class CX10PingWorker;
class CFrameTaskBarIcon;
class CGUIFrame;
class CX10;

//// IDs for the controls and the menu commands
enum
{
	// file ids
	wxID_CONFIGOPT		= 1001,
	wxID_ABOUTX10PING	= 1002,
	wxID_ADDOPT			= 1003,
	wxID_DEVSAVE		= 1004,
	wxID_DEVLOAD		= 1005,
	// popuup menu ids
	wxID_RCLICK_X10_ON_KEY	= 1500,
	wxID_RCLICK_X10_OFF_KEY	= 1501,
	wxID_RCLICK_X10_CHECK_KEY = 1502,
	// buttons
	wxID_BUTTON_STARTMON	= 2001,
	wxID_BUTTON_STOPMON		= 2002,
	wxID_BUTTON_ADDDEV		= 2003,
	wxID_BUTTON_CLEARDEV	= 2004,
	wxID_BUTTON_EDITDEV		= 2005,
	wxID_LIST_EVENT			= 2008,
	wxID_LIST_EVENT_RCLICK	= 2009,
	wxID_CHECKBOX_TIMEFRAME	= 2010,
	// icon menu
	wxID_RCLICK_ICON_EXIT	= 3001,
	wxID_RCLICK_ICON_START	= 3002,
	wxID_RCLICK_ICON_STOP	= 3003

};

static wxMenuBar *menuBar = NULL;

// class:	CFrameTaskBarIcon
///////////////////////////////////////////////////////
class CNetDeviceList : public wxListCtrl
{
// public methods
public:
	CNetDeviceList( wxWindow* parent, wxWindowID id, 
					const wxPoint& pos = wxDefaultPosition, 
					const wxSize& size = wxDefaultSize, long style = wxLC_ICON ); 

	virtual ~CNetDeviceList( );
	void OnRightCick( wxMouseEvent& pEvent );

// public data
	// right click popup menu
	wxMenu*			m_pDeviceRCLickMenu;
	CX10PingWorker*	m_pX10PingWorker;

	DECLARE_EVENT_TABLE()
};

// class:	CFrameTaskBarIcon
///////////////////////////////////////////////////////
class CFrameTaskBarIcon : public wxTaskBarIcon
{
// methods
public:
	CFrameTaskBarIcon( );
	~CFrameTaskBarIcon( );
	void OnShowFrame( wxTaskBarIconEvent& pEvent );
	virtual wxMenu* CreatePopupMenu( );
	void OnExit( wxCommandEvent& pEvent );
	void OnStart( wxCommandEvent& pEvent );
	void OnStop( wxCommandEvent& pEvent );
// public data
public:
	CGUIFrame *m_pFrame;
//	wxMenu* m_pMenu;
	
	DECLARE_EVENT_TABLE()
};

// class:	CAddDeviceDialog
///////////////////////////////////////////////////////
class CChangeDeviceDialog : public wxDialog
{
// methods
public:
    CChangeDeviceDialog( wxWindow *parent, const wxString& title, 
						char* strDeviceName = NULL, 
						int nHouseId = -1, int nDeviceId = -1 );
	wxTextCtrl*		m_pDeviceHostNameEntry;
	wxChoice*		m_pX10DeviceIDSelect;
	wxChoice*		m_pX10HouseIDSelect;
	wxChoice*		m_pX10MethodHostCheck;
	wxChoice*		m_pX10ActionLogic;
	wxChoice*		m_pX10ActionFunction;
	wxChoice*		m_pX10ActionNotify;
	wxTextCtrl*		m_pDeviceHostRebootTimeEntry;
	wxTextCtrl*		m_pDeviceHostFailuresToAction;
	wxTextCtrl*		m_pDeviceCheckStartTimeH;
	wxTextCtrl*		m_pDeviceCheckStartTimeM;
	wxTextCtrl*		m_pDeviceCheckEndTimeH;
	wxTextCtrl*		m_pDeviceCheckEndTimeM;
	wxCheckBox*		m_pTimeFrameCheck;
	wxStaticText*	m_pDeviceCheckStartTimeSep;
	wxStaticText*	m_pDeviceCheckEndTimeSep;
	wxStaticText*	m_pDeviceCheckStartTimeLabel;
	wxStaticText*	m_pDeviceCheckEndTimeLabel;

	~CChangeDeviceDialog();

	void SetConfig( int nCheckMethod,
									int nFailuresToAction, int nActionLogic,
									int nActionFunction, int nActionNotify, int nRebootTime, 
									int bCheckTimeFrame, int nCheckStartTimeH,
									int nCheckStartTimeM, int nCheckEndTimeH, 
									int nCheckEndTimeM );
	void DisableTimeFrame( );
	void EnableTimeFrame( );
	void OnCheckBox( wxCommandEvent& pEvent );

	DECLARE_EVENT_TABLE()
};

// class:	CGUIFrame
///////////////////////////////////////////////////////
class CGUIFrame: public wxFrame
{
// methods
public:
	CGUIFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, 
			const wxSize& size);

	virtual ~CGUIFrame();

	// general menu handlers
	void OnSave( wxCommandEvent& WXUNUSED(pEvent) );
	void OnLoad( wxCommandEvent& WXUNUSED(pEvent) );
	void OnConfig( wxCommandEvent& WXUNUSED(pEvent) );

	void OnAdd( wxCommandEvent& pEvent );
	void OnAbout( wxCommandEvent& pEvent );
	void OnRightCick( wxMouseEvent& pEvent );
	// x10 right click menu functions
	void OnX10ON( wxCommandEvent& pEevent );
	void OnX10OFF( wxCommandEvent& pEvent );
	void OnX10DeviceCheck( wxCommandEvent& pEvent );
	// button handler
	void OnButton( wxCommandEvent& pEvent );

	void ClearDevice( unsigned int nIndex );
	// panel methods
	void SetDevice( unsigned int nIndex, char *strName, 
						  unsigned int nHouseId, unsigned int nUnitId, int nState );
	void SetDeviceState( unsigned int nIndex, int nState );

	void OnSlider( wxScrollEvent& pEvent );
	// exit/close handlers
	void OnCloseWindow( wxCloseEvent& pEvent );
	void OnMinimize( wxIconizeEvent& pEvent );
    void OnExit( wxCommandEvent& pEvent );


	void SetEngine( CX10PingWorker *pX10PingWorker );

// data
public:
	// my  buttons
	wxButton			*m_pStartMonButton;
	wxButton			*m_pStopMonButton;
	wxButton			*m_pAddDevButton;
	wxButton			*m_pClearDevButton;
	wxButton			*m_pEditDevButton;

	CFrameTaskBarIcon	*m_pFrameTaskBarIcon;
	CX10PingApp			*m_pCX10PingApp;
	CX10PingWorker		*m_pX10PingWorker;
	CX10				*m_pX10;

	CNetDeviceList*		m_pListBox;
	wxTextCtrl			*m_pTextLogger;
//	wxStaticText		*m_pScoreText;
//	wxStaticText		*m_pUnitsText;
//	wxTextCtrl			*m_pStepText;
//	wxTextCtrl			*m_pMirrorSlopText;
//	wxTextCtrl			*m_pFocuserSlopText;
//	wxTextCtrl			*m_pEventText;

	// logger and config
	CLogger				*m_pLogger;	
	CConfig				*m_pConfig;

	// some flags
	int m_bWorkerIsPaused;
	int m_bIconized;

	DECLARE_EVENT_TABLE()
};

#endif
