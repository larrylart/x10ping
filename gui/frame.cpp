////////////////////////////////////////////////////////////////////
// Package:		main frame implementation
// File:		frame.h
// Purpose:		define my main frame structure
//
// Created by:	Larry Lart on 22-Apr-2006
// Updated by:
//
// Copyright:	(c) 2006 Larry Lart
// Licence:		Digital Entity
////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include "wx/wxprec.h"
#include "wx/statline.h"
#include "wx/minifram.h"
#include <wx/string.h>
//#include "wx/thread.h"
//#include <wx/listbox.h>
//#include <wx/taskbar.h>

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "../x10ping.xpm"

// load local headers
#include "../logger/logger.h"
#include "../worker.h"
#include "configview.h"
#include "../config/config.h"
#include "../x10.h"
#include "about.h"
#include "../util/winstartup.h""
#include "../x10ping.h"
#include "../network/netdevice.h"

// maine header
#include "frame.h"

//		***** CLASS CNetDeviceList *****
////////////////////////////////////////////////////////////////////
// CNetDeviceList EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CNetDeviceList, wxListCtrl)
		// list events
//	EVT_LISTBOX_DCLICK( wxID_LIST_EVENT, CNetDeviceList::OnDClick )
	// right click event (to apply to all lists
	EVT_RIGHT_DOWN( CNetDeviceList::OnRightCick )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CNetDeviceList
// Purpose:	Initialize my device control list
// Input:	pnothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CNetDeviceList::CNetDeviceList( wxWindow* parent, wxWindowID id,
							   const wxPoint& pos, const wxSize& size,
							   long style ) : wxListCtrl( parent, id, pos, size, style )
{
	// build my right click menus
	// :: menu popup when you right click in the decice list
	m_pDeviceRCLickMenu = new wxMenu( "", 0 );
	m_pDeviceRCLickMenu->Append( wxID_RCLICK_X10_ON_KEY, "Switch On");
	m_pDeviceRCLickMenu->Append( wxID_RCLICK_X10_OFF_KEY, "Switch Off");
	m_pDeviceRCLickMenu->Append( wxID_RCLICK_X10_CHECK_KEY, "Check");
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CNetDeviceList
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CNetDeviceList::~CNetDeviceList( )
{
	delete( m_pDeviceRCLickMenu );
	m_pX10PingWorker = NULL;
}

////////////////////////////////////////////////////////////////////
// Method:		OnRightCick
// Class:		CNetDeviceList
// Purpose:		when right click is pressed show popup menu
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CNetDeviceList::OnRightCick( wxMouseEvent& pEvent )
{
	long nItem = -1;
	// check if anything is selected
	if( GetSelectedItemCount() == 0 ) return;
	// check f anything is selected out of range
	nItem = GetNextItem( nItem,
                             wxLIST_NEXT_ALL,
                             wxLIST_STATE_SELECTED );

	if ( nItem == -1  || nItem >= m_pX10PingWorker->m_nDevice ) return;

	// show right click menu
	PopupMenu( m_pDeviceRCLickMenu );
}

//		***** CLASS CFrameTaskBarIcon *****
////////////////////////////////////////////////////////////////////
// CFrameTaskBarIcon EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CFrameTaskBarIcon, wxTaskBarIcon)
	EVT_TASKBAR_LEFT_DCLICK(CFrameTaskBarIcon::OnShowFrame)
	EVT_MENU( wxID_RCLICK_ICON_EXIT, CFrameTaskBarIcon::OnExit )
	EVT_MENU( wxID_RCLICK_ICON_START, CFrameTaskBarIcon::OnStart )
	EVT_MENU( wxID_RCLICK_ICON_STOP, CFrameTaskBarIcon::OnStop )
//	EVT_TASKBAR_RIGHT_DOWN(
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CFrameTaskBarIcon
// Purpose:	Initialize task bar icon
// Input:	pnothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CFrameTaskBarIcon::CFrameTaskBarIcon( ) : wxTaskBarIcon( )
{
//	SetIcon( wxICON( MYICON ), "x10ping Monitor" );

}

CFrameTaskBarIcon::~CFrameTaskBarIcon( )
{
//	delete( m_pMenu );
}

////////////////////////////////////////////////////////////////////
// Method:	OnShowFrame
// Class:	CFrameTaskBarIcon
// Purpose:	show frame on left double click
// Input:	reference to event
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CFrameTaskBarIcon::OnShowFrame( wxTaskBarIconEvent& WXUNUSED(pEvent) )
{

	m_pFrame->Show(TRUE);
	m_pFrame->Iconize( false );
//	m_pFrame->Show(TRUE);
	m_pFrame->Raise();
	RemoveIcon( );
	m_pFrame->m_bIconized = 0;

	return;
}

wxMenu* CFrameTaskBarIcon::CreatePopupMenu( )
{
	wxMenu* pMenu = new wxMenu( "", 0 );
	pMenu->Append( wxID_RCLICK_ICON_EXIT, "Exit");
	pMenu->Append( wxID_RCLICK_ICON_START, "Start");
	pMenu->Append( wxID_RCLICK_ICON_STOP, "Stop");

	if( m_pFrame->m_pStartMonButton->IsEnabled( ) )
		pMenu->Enable( wxID_RCLICK_ICON_START, true );
	else
		pMenu->Enable( wxID_RCLICK_ICON_START, false );

	if( m_pFrame->m_pStopMonButton->IsEnabled( ) )
		pMenu->Enable( wxID_RCLICK_ICON_STOP, true );
	else
		pMenu->Enable( wxID_RCLICK_ICON_STOP, false );

	return( pMenu );
}

////////////////////////////////////////////////////////////////////
// Method:		On Exit
// Class:		CFrameTaskBarIcon
// Purpose:		exit x10ping program
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CFrameTaskBarIcon::OnExit( wxCommandEvent& WXUNUSED(pEvent) )
{
//    this->Destroy();
	m_pFrame->Close( );
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		On Start
// Class:		CFrameTaskBarIcon
// Purpose:		start loop
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CFrameTaskBarIcon::OnStart( wxCommandEvent& WXUNUSED(pEvent) )
{
	m_pFrame->m_pX10PingWorker->DoStart( );
	m_pFrame->m_pStartMonButton->Enable( 0 );
	m_pFrame->m_pStopMonButton->Enable( 1 );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		On Stop
// Class:		CFrameTaskBarIcon
// Purpose:		stop loop
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CFrameTaskBarIcon::OnStop( wxCommandEvent& WXUNUSED(pEvent) )
{
	m_pFrame->m_pX10PingWorker->DoPause( );
	m_pFrame->m_pStartMonButton->Enable( 1 );
	m_pFrame->m_pStopMonButton->Enable( 0 );

	return;
}


//		***** CLASS CChangeDeviceDialog *****
////////////////////////////////////////////////////////////////////
// CChangeDeviceDialog EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CChangeDeviceDialog, wxDialog)
	EVT_CHECKBOX( wxID_CHECKBOX_TIMEFRAME, CChangeDeviceDialog::OnCheckBox )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CChangeDeviceDialog
// Purpose:	Initialize add devicedialog
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
CChangeDeviceDialog::CChangeDeviceDialog( wxWindow *parent, const wxString& title,
				 char* strDeviceName, int nHouseId, int nDeviceId ) : wxDialog(parent, -1,
                          title,
                          wxDefaultPosition,
                          wxDefaultSize,
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL)
{
	wxString vectStrX10Dev[16];
	wxString vectStrX10ActionLogic[3];
	wxString vectStrX10ActionFunction[5];
	wxString vectStrX10ActionNotification[4];
	wxString vectMethodCheckHost[1];

	// populate action logic options
	vectStrX10ActionLogic[0] = "when host found down";
	vectStrX10ActionLogic[1] = "when host found up";
	vectStrX10ActionLogic[2] = "when host found slow";

	// populate action function options
	vectStrX10ActionFunction[0] = "power cycle host (OFF-WAIT-ON)";
	vectStrX10ActionFunction[1]	= "power impulse host (ON-WAIT-OFF)";
	vectStrX10ActionFunction[2] = "power OFF host";
	vectStrX10ActionFunction[3] = "power ON host";
	vectStrX10ActionFunction[4] = "do nothing";

	// populate notifaction options
	vectStrX10ActionNotification[0] = "no notification";
	vectStrX10ActionNotification[1] = "visual notification";
	vectStrX10ActionNotification[2] = "audio notification";
	vectStrX10ActionNotification[3] = "email notification";

	// methods to check the host
	vectMethodCheckHost[0] = "use ping";
//	vectMethodCheckHost[1] = "use tcp/udp port check";
//	vectMethodCheckHost[2] = "use external program";

	int i = 0;

	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
//	wxGridSizer *sizerPanel = new wxGridSizer(2,2,10,10);

	// building configure as a note book
	wxNotebook *pNotebook = new wxNotebook( this, -1 ); //, wxPoint( 0, 0 ),
									//wxSize( 450, 540 ) );
	// add basic config panel
	wxPanel* pPanelBasic = new wxPanel( pNotebook );
	// add schedule config panel
	wxPanel* pPanelSchedule = new wxPanel( pNotebook );
	// add advanced panel
	wxPanel* pPanelAdvanced = new wxPanel( pNotebook );

	/////////////////////////////
	// basic panel sizer - layout of elements
	wxGridSizer *sizerBasicPanel = new wxGridSizer( 2, 2, 10, 10 );
	// layout of group of elements in the panel
	wxGridSizer* sizerBasicPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerBasicPage->Add( sizerBasicPanel, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL| wxALL, 10  );

	/////////////////////////////
	// schedule panel sizer - layout of elements
	wxGridSizer *sizerSchedulePanel = new wxGridSizer( 2, 2, 10, 10 );
	// layout of group of elements in the panel
	wxGridSizer* sizerSchedulePage = new wxGridSizer( 1, 1, 10, 10 );
	sizerSchedulePage->Add( sizerSchedulePanel, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////
	// advanced panel sizer - layout of elements
	wxFlexGridSizer *sizerAdvancedPanel = new wxFlexGridSizer( 2, 2, 10, 10 );
	// layout of group of elements in the panel
	wxGridSizer* sizerAdvancedPage = new wxGridSizer( 1, 1, 10, 10 );
	sizerAdvancedPage->Add( sizerAdvancedPanel, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	m_pDeviceHostNameEntry = new wxTextCtrl( pPanelBasic, -1, _T(""),
									wxDefaultPosition, wxSize(170,-1) );

	m_pDeviceHostRebootTimeEntry = new wxTextCtrl( pPanelAdvanced, -1, _T(""),
									wxDefaultPosition, wxSize(50,-1) );

	m_pDeviceHostFailuresToAction = new wxTextCtrl( pPanelAdvanced, -1, _T(""),
									wxDefaultPosition, wxSize(50,-1) );


	m_pDeviceCheckStartTimeH = new wxTextCtrl( pPanelSchedule, -1, _T(""),
									wxDefaultPosition, wxSize(24,-1) );
	m_pDeviceCheckStartTimeH->SetMaxLength( 2 );


	m_pDeviceCheckStartTimeM = new wxTextCtrl( pPanelSchedule, -1, _T(""),
									wxDefaultPosition, wxSize(24,-1) );
	m_pDeviceCheckStartTimeM->SetMaxLength( 2 );

	m_pDeviceCheckEndTimeH = new wxTextCtrl( pPanelSchedule, -1, _T(""),
									wxDefaultPosition, wxSize(24,-1) );
	m_pDeviceCheckEndTimeH->SetMaxLength( 2 );


	m_pDeviceCheckEndTimeM = new wxTextCtrl( pPanelSchedule, -1, _T(""),
									wxDefaultPosition, wxSize(24,-1) );
	m_pDeviceCheckEndTimeM->SetMaxLength( 2 );

	m_pTimeFrameCheck = new wxCheckBox( pPanelSchedule, wxID_CHECKBOX_TIMEFRAME, _T("") );

	// device id select option
	for( i=0; i<16; i++ )
	{
		vectStrX10Dev[i].sprintf( "%.2d", i+1 );
	}

	m_pX10DeviceIDSelect = new wxChoice( pPanelBasic, -1,
        wxDefaultPosition,
        wxSize(40,-1), 16, vectStrX10Dev, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// house id select option
	m_pX10HouseIDSelect = new wxChoice( pPanelBasic, -1,
        wxDefaultPosition,
        wxSize(40,-1), 16, m_vectStrX10House, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// method to check the host
	m_pX10MethodHostCheck = new wxChoice( pPanelBasic, -1,
        wxDefaultPosition,
        wxSize(-1,-1), 1, vectMethodCheckHost, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// action logic choice
	m_pX10ActionLogic = new wxChoice( pPanelAdvanced, -1,
        wxDefaultPosition,
        wxSize(-1,-1), 3, vectStrX10ActionLogic, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// action function choice
	m_pX10ActionFunction = new wxChoice( pPanelAdvanced, -1,
        wxDefaultPosition,
        wxSize(-1,-1), 5, vectStrX10ActionFunction, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// action notification choice
	m_pX10ActionNotify = new wxChoice( pPanelBasic, -1,
        wxDefaultPosition,
        wxSize(-1,-1), 4, vectStrX10ActionNotification, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );


	///////////////////////////////////
	// device host name entry
	sizerBasicPanel->Add(new wxStaticText(pPanelBasic, -1, _T("Device IP/Name to monitor:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerBasicPanel->Add( m_pDeviceHostNameEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	if( strDeviceName != NULL ) m_pDeviceHostNameEntry->SetValue( strDeviceName );

	///////////////////////////////////
	// x10 id for the house
	sizerBasicPanel->Add( new wxStaticText(pPanelBasic, -1, _T("X10 House ID to control:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerBasicPanel->Add( m_pX10HouseIDSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	if( nHouseId != -1 )
	{
		m_pX10HouseIDSelect->SetSelection( nHouseId-1 );
	} else
	{
		// set eeg to current selection
		m_pX10HouseIDSelect->SetSelection( 0 );

	}

	//////////////////////////////////
	// x10 id for the device
	sizerBasicPanel->Add( new wxStaticText(pPanelBasic, -1, _T("X10 Device ID to control:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerBasicPanel->Add( m_pX10DeviceIDSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	if( nDeviceId != -1 )
	{
		m_pX10DeviceIDSelect->SetSelection( nDeviceId-1 );
	} else
	{
		// set eeg to current selection
		m_pX10DeviceIDSelect->SetSelection( 0 );

	}

	//////////////////////////////////////
	// Method to check the host
	sizerBasicPanel->Add( new wxStaticText(pPanelBasic, -1, _T("How to check the host:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerBasicPanel->Add( m_pX10MethodHostCheck, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// set eeg to current selection
	m_pX10MethodHostCheck->SetSelection( 0 );

	////////////////////////////////////
	// x10 action notification
	sizerBasicPanel->Add( new wxStaticText(pPanelBasic, -1, _T("Notify on action:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerBasicPanel->Add( m_pX10ActionNotify, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// set eeg to current selection
	m_pX10ActionNotify->SetSelection( 0 );

	//////////////////////////////////////
	// logic do if up or down
	sizerAdvancedPanel->Add( new wxStaticText(pPanelAdvanced, -1, _T("When to take action:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerAdvancedPanel->Add( m_pX10ActionLogic, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// set eeg to current selection
	m_pX10ActionLogic->SetSelection( 0 );

	////////////////////////////////////
	// x10 function if down
	sizerAdvancedPanel->Add( new wxStaticText(pPanelAdvanced, -1, _T("What action to take:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerAdvancedPanel->Add( m_pX10ActionFunction, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// set eeg to current selection
	m_pX10ActionFunction->SetSelection( 0 );


	///////////////////////////////////
	// host reboot time entry
	sizerAdvancedPanel->Add(new wxStaticText(pPanelAdvanced, -1, _T("Host reboot time(s/10):")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerAdvancedPanel->Add( m_pDeviceHostRebootTimeEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	m_pDeviceHostRebootTimeEntry->SetValue( "1500" );

	////////////////////////////////////
	// consecutive failures to action
	sizerAdvancedPanel->Add(new wxStaticText(pPanelAdvanced, -1, _T("Consecutive failures to action:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerAdvancedPanel->Add( m_pDeviceHostFailuresToAction, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	m_pDeviceHostFailuresToAction->SetValue( "2" );
//	if( strFailuresToAction != NULL ) m_pDeviceHostFailuresToAction->SetValue( strFailuresToAction );

	///////////////////////////////////
	// time frame check
	sizerSchedulePanel->Add(new wxStaticText( pPanelSchedule, -1, _T("Only check in this time frame")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerSchedulePanel->Add( m_pTimeFrameCheck, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// start time
	m_pDeviceCheckStartTimeLabel = new wxStaticText(pPanelSchedule, -1, _T("Start time:"));
	sizerSchedulePanel->Add( m_pDeviceCheckStartTimeLabel, 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

	wxBoxSizer* sizerCheckStartTime = new wxBoxSizer( wxHORIZONTAL );
	m_pDeviceCheckStartTimeSep = new wxStaticText(pPanelSchedule, -1, _T(":"));
	sizerCheckStartTime->Add( m_pDeviceCheckStartTimeH, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerCheckStartTime->Add( m_pDeviceCheckStartTimeSep, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	sizerCheckStartTime->Add( m_pDeviceCheckStartTimeM, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerSchedulePanel->Add( sizerCheckStartTime, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

//	if( strCheckStartTimeH != NULL ) m_pDeviceCheckStartTimeH->SetValue( strCheckStartTimeH );
//	if( strCheckStartTimeM != NULL ) m_pDeviceCheckStartTimeM->SetValue( strCheckStartTimeM );

	// end time
	m_pDeviceCheckEndTimeLabel = new wxStaticText(pPanelSchedule, -1, _T("End time:"));
	sizerSchedulePanel->Add( m_pDeviceCheckEndTimeLabel, 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

	wxBoxSizer* sizerCheckEndTime = new wxBoxSizer( wxHORIZONTAL );
	m_pDeviceCheckEndTimeSep = new wxStaticText(pPanelSchedule, -1, _T(":"));
	sizerCheckEndTime->Add( m_pDeviceCheckEndTimeH, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerCheckEndTime->Add( m_pDeviceCheckEndTimeSep, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	sizerCheckEndTime->Add( m_pDeviceCheckEndTimeM, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	sizerSchedulePanel->Add( sizerCheckEndTime, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	// by default disable time frame entry
	DisableTimeFrame( );

//	if( strCheckEndTimeH != NULL ) m_pDeviceCheckEndTimeH->SetValue( strCheckEndTimeH );


	// add panel to the main sizer
//	topsizer->Add( sizerPanel, 1, wxGROW | wxALL, 10 );
//	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 10 );

	// calculate sizers
	topsizer->Add( pNotebook, 1, wxGROW | wxALL, 10 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 10 );

	pPanelBasic->SetSizer( sizerBasicPage );
	pNotebook->AddPage( pPanelBasic, _("Basic") );
	pPanelSchedule->SetSizer( sizerSchedulePage );
	pNotebook->AddPage( pPanelSchedule, _("Schedule") );
	pPanelAdvanced->SetSizer( sizerAdvancedPage );
	pNotebook->AddPage( pPanelAdvanced, _("Advanced") );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	DisableTimeFrame
// Class:	CChangeDeviceDialog
// Purpose:	disable time frame entry
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CChangeDeviceDialog::DisableTimeFrame( )
{
	m_pDeviceCheckStartTimeLabel->Disable( );
	m_pDeviceCheckStartTimeH->Disable( );
	m_pDeviceCheckStartTimeSep->Disable( );
	m_pDeviceCheckStartTimeM->Disable( );

	m_pDeviceCheckEndTimeLabel->Disable( );
	m_pDeviceCheckEndTimeH->Disable( );
	m_pDeviceCheckEndTimeSep->Disable( );
	m_pDeviceCheckEndTimeM->Disable( );

}

////////////////////////////////////////////////////////////////////
// Method:	OnCheckBox
// Class:	CChangeDeviceDialog
// Purpose:	use when check box is un/ticked
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CChangeDeviceDialog::OnCheckBox( wxCommandEvent& pEvent )
{
	if( m_pTimeFrameCheck->IsChecked( ) )
	{
		EnableTimeFrame( );
	} else
	{
		DisableTimeFrame( );
	}
}

////////////////////////////////////////////////////////////////////
// Method:	EnableTimeFrame
// Class:	CChangeDeviceDialog
// Purpose:	disable time frame entry
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CChangeDeviceDialog::EnableTimeFrame( )
{
	m_pDeviceCheckStartTimeLabel->Enable( );
	m_pDeviceCheckStartTimeH->Enable( );
	m_pDeviceCheckStartTimeSep->Enable( );
	m_pDeviceCheckStartTimeM->Enable( );

	m_pDeviceCheckEndTimeLabel->Enable( );
	m_pDeviceCheckEndTimeH->Enable( );
	m_pDeviceCheckEndTimeSep->Enable( );
	m_pDeviceCheckEndTimeM->Enable( );

}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CChangeDeviceDialog
// Purpose:	update config from worker
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CChangeDeviceDialog::SetConfig( int nCheckMethod,
									int nFailuresToAction, int nActionLogic,
									int nActionFunction, int nActionNotify, int nRebootTime,
									int bCheckTimeFrame, int nCheckStartTimeH,
									int nCheckStartTimeM, int nCheckEndTimeH,
									int nCheckEndTimeM )
{
	char strTmp[255];

	if( nCheckMethod != -1 )
	{
		m_pX10MethodHostCheck->SetSelection( nCheckMethod-1 );

	} else
	{
		// set eeg to current selection
		m_pX10MethodHostCheck->SetSelection( 0 );

	}

	if( nActionLogic != -1 )
	{
		m_pX10ActionLogic->SetSelection( nActionLogic-1 );
	} else
	{
		// set eeg to current selection
		m_pX10ActionLogic->SetSelection( 0 );

	}

	if( nActionFunction != -1 )
	{
		m_pX10ActionFunction->SetSelection( nActionFunction-1 );
	} else
	{
		// set eeg to current selection
		m_pX10ActionFunction->SetSelection( 0 );

	}

	if( nActionNotify != -1 )
	{
		m_pX10ActionNotify->SetSelection( nActionNotify-1 );
	} else
	{
		// set eeg to current selection
		m_pX10ActionNotify->SetSelection( 0 );

	}

	if( nRebootTime != -1 )
	{
		sprintf( strTmp, "%d", nRebootTime );
		m_pDeviceHostRebootTimeEntry->SetValue( strTmp );
	} else
		m_pDeviceHostRebootTimeEntry->SetValue( "1000" );

	if( nFailuresToAction != -1 )
	{
		sprintf( strTmp, "%d", nFailuresToAction );
		m_pDeviceHostFailuresToAction->SetValue( strTmp );
	} else
		m_pDeviceHostFailuresToAction->SetValue( "2" );


	if( bCheckTimeFrame != -1 )
	{
		m_pTimeFrameCheck->SetValue( bCheckTimeFrame );
		if( bCheckTimeFrame ==  1 )
		{
			EnableTimeFrame( );

			sprintf( strTmp, "%d", nCheckStartTimeH );
			m_pDeviceCheckStartTimeH->SetValue( strTmp );
			sprintf( strTmp, "%.2d", nCheckStartTimeM );
			m_pDeviceCheckStartTimeM->SetValue( strTmp );

			sprintf( strTmp, "%d", nCheckEndTimeH );
			m_pDeviceCheckEndTimeH->SetValue( strTmp );
			sprintf( strTmp, "%.2d", nCheckEndTimeM );
			m_pDeviceCheckEndTimeM->SetValue( strTmp );

		}
	} else
		m_pTimeFrameCheck->SetValue( "0" );

}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CChangeDeviceDialog
// Purpose:	Delete my object
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CChangeDeviceDialog::~CChangeDeviceDialog( )
{
    delete( m_pDeviceHostNameEntry );
	delete( m_pX10DeviceIDSelect );
	delete( m_pX10HouseIDSelect );
	delete( m_pX10MethodHostCheck );
	delete( m_pX10ActionLogic );
	delete( m_pX10ActionFunction );
	delete( m_pX10ActionNotify );
	delete( m_pDeviceHostRebootTimeEntry );
	delete( m_pDeviceHostFailuresToAction );
	delete( m_pDeviceCheckStartTimeH );
	delete( m_pDeviceCheckStartTimeM );
	delete( m_pDeviceCheckEndTimeH );
	delete( m_pDeviceCheckEndTimeM );
	delete( m_pTimeFrameCheck );
	delete( m_pDeviceCheckStartTimeLabel );
	delete( m_pDeviceCheckEndTimeLabel );
	delete( m_pDeviceCheckStartTimeSep );
	delete( m_pDeviceCheckEndTimeSep );

}

//		***** CLASS CGUIFRAME *****
////////////////////////////////////////////////////////////////////
// CGUIFrame EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CGUIFrame, wxFrame )
	// menu->file events
	EVT_MENU( wxID_DEVSAVE, CGUIFrame::OnSave )
	EVT_MENU( wxID_DEVLOAD, CGUIFrame::OnLoad )
	EVT_MENU( wxID_CONFIGOPT, CGUIFrame::OnConfig )
	EVT_MENU( wxID_ABOUTX10PING, CGUIFrame::OnAbout )
	// exit event
	EVT_MENU( wxID_EXIT, CGUIFrame::OnExit )
//	EVT_MENU( wxID_RCLICK_ICON_EXIT, CGUIFrame::OnExit )
	// right click popup menu functions
	EVT_MENU( wxID_RCLICK_X10_ON_KEY, CGUIFrame::OnX10ON )
	EVT_MENU( wxID_RCLICK_X10_OFF_KEY, CGUIFrame::OnX10OFF )
	EVT_MENU( wxID_RCLICK_X10_CHECK_KEY, CGUIFrame::OnX10DeviceCheck )
	// panel buttons and control list events
	EVT_BUTTON( wxID_BUTTON_STARTMON, CGUIFrame::OnButton )
	EVT_BUTTON( wxID_BUTTON_STOPMON, CGUIFrame::OnButton )
	EVT_BUTTON( wxID_BUTTON_ADDDEV, CGUIFrame::OnButton )
	EVT_BUTTON( wxID_BUTTON_CLEARDEV,CGUIFrame::OnButton )
	EVT_BUTTON( wxID_BUTTON_EDITDEV, CGUIFrame::OnButton )
	// close window event
	EVT_CLOSE( CGUIFrame::OnCloseWindow )
	// on minimize
	EVT_ICONIZE( CGUIFrame::OnMinimize )
END_EVENT_TABLE()

////////////////////////////////////////////////////////////////////
// Method:		Map GUI frame constructor
// Class:		CGUIFrame
// Purpose:		Buils my frame object
// Input:		pointer to frame, string tile, number position/size
// Output:		nothing
////////////////////////////////////////////////////////////////////
CGUIFrame::CGUIFrame(wxWindow* parent, const wxString& title,
			const wxPoint& pos, const wxSize& size):
			wxFrame(parent, -1, title, pos, size, wxSYSTEM_MENU | wxMINIMIZE_BOX |wxCLOSE_BOX| wxCAPTION )
{
	int i = 0;
	m_pConfig = NULL;
	m_pLogger = NULL;
	m_bWorkerIsPaused = 0;
	m_bIconized = 0;

	wxIcon icon( x10ping_xpm );
	SetIcon( icon );
//	SetIcon( wxICON( X10PING ) );

	// set bg color for my frame
//	SetBackgroundColour( *wxLIGHT_GREY );

	// create taskbar icon
	m_pFrameTaskBarIcon = new CFrameTaskBarIcon();
	m_pFrameTaskBarIcon->m_pFrame = this;

	// make file menu bar
	wxMenu *pSessionMenu = new wxMenu( "", wxMENU_TEAROFF );
	pSessionMenu->Append(wxID_DEVSAVE, "Save");
	pSessionMenu->Append(wxID_DEVLOAD, "Load");
	pSessionMenu->AppendSeparator();
	pSessionMenu->Append(wxID_CONFIGOPT, "Config");
	pSessionMenu->AppendSeparator();
	pSessionMenu->Append(wxID_ABOUTX10PING, "About");
	pSessionMenu->AppendSeparator();
	pSessionMenu->Append(wxID_EXIT, "Exit");


	// make menu toolbar
	menuBar = new wxMenuBar;
	menuBar->Append(pSessionMenu, "File");
	this->SetMenuBar(menuBar);

	int width, height;
	this->GetClientSize(&width, &height);

	int nControlPanelY = height-100-50;

	wxPanel *panel1 = new wxPanel(this, -1, wxPoint(0,0), wxSize(width,height), 0 );

	// options box
	wxStaticBox *pMonitorBox = new wxStaticBox( panel1, -1, "Monitoring List",
									wxPoint(0,0), wxSize(width,height-100-50-2) );

	// panel box
	wxStaticBox *pPanelBox = new wxStaticBox( panel1, -1, "Control Panel",
									wxPoint( 0, nControlPanelY ), wxSize(width,48) );

	/////////////////////////////////////
	// start/stop/rec/pause buttons
	////////////////////////////////////
	// Start monitor button
	int nButtonWidth = 90;
	int nStartMonButtonX = (int)((width/6)-(nButtonWidth/2));
	m_pStartMonButton = new wxButton( panel1, wxID_BUTTON_STARTMON, "Start Monitoring",
										wxPoint( nStartMonButtonX , nControlPanelY+18 ),
										wxSize(nButtonWidth,20) );

	// Start monitor button
	int nStopMonButtonX = (int)(2*(width/6)-(nButtonWidth/2));
	m_pStopMonButton = new wxButton( panel1, wxID_BUTTON_STOPMON, "Stop Monitoring",
										wxPoint( nStopMonButtonX, nControlPanelY+18 ),
										wxSize(nButtonWidth,20) );

	// set default not started
	m_pStartMonButton->Enable( 1 );
	m_pStopMonButton->Enable( 0 );

	// Add device button
	int nAddDevButtonX = (int)(3*(width/6)-(nButtonWidth/2));
	m_pAddDevButton = new wxButton( panel1, wxID_BUTTON_ADDDEV, "Add Device",
										wxPoint( nAddDevButtonX, nControlPanelY+18 ),
										wxSize(nButtonWidth,20) );
	// Clear device button
	int nClearDevButtonX = (int)(4*(width/6)-(nButtonWidth/2));
	m_pClearDevButton = new wxButton( panel1, wxID_BUTTON_CLEARDEV, "Clear Device",
										wxPoint( nClearDevButtonX, nControlPanelY+18 ),
										wxSize(nButtonWidth,20) );
	// Clear device button
	int nEditDevButtonX = (int)(5*(width/6)-(nButtonWidth/2));
	m_pEditDevButton = new wxButton( panel1, wxID_BUTTON_EDITDEV, "Edit Device",
										wxPoint( nEditDevButtonX, nControlPanelY+18 ),
										wxSize(nButtonWidth,20) );

	// my view control list
	m_pListBox = new CNetDeviceList(panel1, wxID_LIST_EVENT,
			wxPoint(3,15), wxSize(width-7,height-100-50-20), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES );
	m_pListBox->InsertColumn( 1, "hId", wxLIST_FORMAT_CENTRE, 30 );
	m_pListBox->InsertColumn( 2, "dId", wxLIST_FORMAT_CENTRE, 30 );
	m_pListBox->InsertColumn( 3, "Net Device Address", wxLIST_FORMAT_CENTRE, 2*(width/3) );
	m_pListBox->InsertColumn( 4, "Net Device Status", wxLIST_FORMAT_CENTRE, width/3-60-28 );
	// insert maxnumber of items to list
	for( i=0; i<20; i++ )
	{
		long nIndex = m_pListBox->InsertItem( 55551+i, "" );
		m_pListBox->SetItem( nIndex, 0, "" );
		m_pListBox->SetItem( nIndex, 1, "" );
		m_pListBox->SetItem( nIndex, 2, "" );
		m_pListBox->SetItem( nIndex, 3, "" );
	}

	// here is where I log my messages
	m_pTextLogger = new wxTextCtrl(panel1, -1, _T(""),
			wxPoint(0, height-100), wxSize(width,100), wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH );


	wxBoxSizer *sizer1 = new wxBoxSizer(wxVERTICAL);
	sizer1->Add( panel1, 1, wxGROW | wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	//sizer1->Add( panel2, 1, wxGROW );
	SetSizer( sizer1 );
    panel1->SetAutoLayout( FALSE );
//	sizer1->Fit( this );

	SetAutoLayout( FALSE );
}

////////////////////////////////////////////////////////////////////
// Method:		Destructor
// Class:		CGUIFrame
// Purpose:		destroy my object
// Input:		number score
// Output:		nothing
////////////////////////////////////////////////////////////////////
CGUIFrame::~CGUIFrame( )
{
	m_pX10PingWorker->SaveNetDevices( "" );
	m_pX10PingWorker->m_bIsExit = 1;
	m_pX10PingWorker->DoPause( );
	m_pX10PingWorker->Delete( );

//	while( m_pX10PingWorker->m_bIsProcessing == 1 )
//	{
//		wxThread::Sleep( 400 );
//	}

//	m_pX10PingWorker->m_bLife = 0;


//	while( m_pX10PingWorker->IsRunning() == 1 )
//	{
//		Sleep( 300 );
//	}

	// get my last setting before exiting
	delete( m_pFrameTaskBarIcon );
	delete( m_pTextLogger );
	delete( m_pListBox );
	delete( m_pStartMonButton );
	delete( m_pStopMonButton );
	delete( m_pAddDevButton );
	delete( m_pClearDevButton );
	delete( m_pEditDevButton );

	m_pCX10PingApp->m_pX10PingWorker = NULL;
	m_pFrameTaskBarIcon = NULL;
	m_pX10 = NULL;
	m_pX10PingWorker = NULL;
	m_pCX10PingApp = NULL;
	m_pLogger = NULL;
	m_pConfig = NULL;

//	menuBar->Close();
//	delete( menuBar );
}

////////////////////////////////////////////////////////////////////
// Method:		ClearDevice
// Class:		CGUIFrame
// Purpose:		clear device data 
// Input:		line index-1
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::ClearDevice( unsigned int nIndex )
{
	m_pListBox->SetItem( nIndex, 0, "" );
	m_pListBox->SetItem( nIndex, 1, "" );
	m_pListBox->SetItem( nIndex, 2, "" );
	m_pListBox->SetItem( nIndex, 3, "" );
	m_pListBox->SetItemBackgroundColour( nIndex, *wxWHITE );
}

////////////////////////////////////////////////////////////////////
// Method:		SetDevice
// Class:		CGUIFrame
// Purpose:		set device info in my report
// Input:		line index-1, dev host name, unit id and state 0/1
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetDevice( unsigned int nIndex, char *strName,
						  unsigned int nHouseId, unsigned int nUnitId, int nState )
{
	char strMsg[255];

	m_pListBox->SetItem( nIndex, 0, m_vectStrX10House[nHouseId-1] );
	sprintf( strMsg, "%d", nUnitId );
	m_pListBox->SetItem( nIndex, 1, strMsg );
	m_pListBox->SetItem( nIndex, 2, strName );
	SetDeviceState( nIndex, nState );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		SetDeviceState
// Class:		CGUIFrame
// Purpose:		set device state as string info
// Input:		index, state
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetDeviceState( unsigned int nIndex, int nState )
{
	// set state as ON/OFF and color
	if( nState == DEVICE_STATE_DOWN )
	{
		m_pListBox->SetItemBackgroundColour( nIndex, *wxRED );
		m_pListBox->SetItem( nIndex, 3, "DOWN" );
	} else if( nState == DEVICE_STATE_UP )
	{
		m_pListBox->SetItemBackgroundColour( nIndex, *wxGREEN );
		m_pListBox->SetItem( nIndex, 3, "UP" );
	} else if( nState == DEVICE_STATE_CHECKING )
	{
		m_pListBox->SetItemBackgroundColour( nIndex, *wxLIGHT_GREY );
		m_pListBox->SetItem( nIndex, 3, "Checking..." );
	} else if( nState == DEVICE_STATE_SWITCHED_OFF )
	{
		m_pListBox->SetItemBackgroundColour( nIndex, *wxCYAN );
		m_pListBox->SetItem( nIndex, 3, "Switched OFF" );
	} else if( nState == DEVICE_STATE_SWITCHED_ON )
	{
		m_pListBox->SetItemBackgroundColour( nIndex, *wxCYAN );
		m_pListBox->SetItem( nIndex, 3, "Switched ON" );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		Set Engine
// Class:		CGUIFrame
// Purpose:		set my focuser engine
// Input:		pointer to focuser object
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::SetEngine( CX10PingWorker *pX10PingWorker )
{
	m_pX10PingWorker = pX10PingWorker;
	m_pListBox->m_pX10PingWorker = m_pX10PingWorker;
	return;
}


////////////////////////////////////////////////////////////////////
// Method:		On Config
// Class:		CGUIFrame
// Purpose:		call new objects by type/event
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnConfig( wxCommandEvent& WXUNUSED(pEvent) )
{
	if( m_pConfig == NULL ) return;

	CConfigView *pConfigView = new CConfigView( this );
	pConfigView->SetConfig( m_pConfig );

	wxString strWxTmp;
	char strTmp[255];

	if( pConfigView->ShowModal() == wxID_OK )
	{
		long nVar = 0;

		// serial port config
		unsigned int nSerialPort = pConfigView->m_pSerialPortSelect->GetSelection()+1;
		m_pConfig->SetIntVar( SECTION_ID_X10, CONF_X10_SERIALPORT, nSerialPort );

		// controller type config
		unsigned int nControllerType = pConfigView->m_pX10ControllerType->GetSelection()+1;
		m_pConfig->SetIntVar( SECTION_ID_X10, CONF_X10_CONTROLLER_MODEL, nControllerType );

		// need to init the conection again
		m_pX10->Init( );

		// net net loop check delay config
		wxString strNetLoopDelay = pConfigView->m_pNetLoopCheckDelayEntry->GetLineText(0);
		if( strNetLoopDelay.IsNumber() && strNetLoopDelay.ToLong( &nVar ) )
		{
			m_pConfig->SetIntVarUpdate( SECTION_ID_NETWORK, CONF_NETWORK_LOOP_CHECK_DELAY, (int) nVar );
		}

		// net device ping retry config
		wxString strDevicePingRetry = pConfigView->m_pDevicePingRetryEntry->GetLineText(0);
		if( strDevicePingRetry.IsNumber() && strDevicePingRetry.ToLong( &nVar ) )
		{
			m_pConfig->SetIntVarUpdate( SECTION_ID_NETWORK, CONF_DEVICE_PING_RETRY, (int) nVar );
		}

		// x 10 delay after off
		wxString strX10DelayAfterOFF = pConfigView->m_pX10DelayAfterOFFEntry->GetLineText(0);
		if( strX10DelayAfterOFF.IsNumber() && strX10DelayAfterOFF.ToLong( &nVar ) )
		{
			m_pConfig->SetIntVarUpdate( SECTION_ID_NETWORK, CONF_NETWORK_X10_DELAY_AFTER_OFF, (int) nVar );
		}

		// x 10 delay after on
		wxString strX10DelayAfterON = pConfigView->m_pX10DelayAfterONEntry->GetLineText(0);
		if( strX10DelayAfterON.IsNumber() && strX10DelayAfterON.ToLong( &nVar ) )
		{
			m_pConfig->SetIntVarUpdate( SECTION_ID_NETWORK, CONF_NETWORK_X10_DELAY_AFTER_ON, (int) nVar );
		}

		// controller type config
		int nTemp = -1;
		m_pConfig->GetIntVar( SECTION_ID_RUNTIME, CONF_RUN_ON_STARTUP, nTemp );

		// runtime config
		unsigned int nRuntimeStartup = pConfigView->m_pRuntimeStartup->GetValue( );
		m_pConfig->SetIntVar( SECTION_ID_RUNTIME, CONF_RUN_ON_STARTUP, nRuntimeStartup );

		// now check if there is a difference
		if( nTemp >= 0 && nTemp != nRuntimeStartup )
		{
			StartupUser user = AllUsers;
			char strTmp[255];
			sprintf( strTmp, "%s%s%s%s%s%s", wxGetCwd().GetData(), "\\", "x10ping.exe",
								" -s \"", wxGetCwd().GetData(), "\"" );
			if( nRuntimeStartup == 1 )
			{
				CWinStartup::AddApp(_T("x10ping"), strTmp, user);
				m_pConfig->SetIntVar( SECTION_ID_RUNTIME, CONF_MONITOR_ON_STARTUP, 1 );
			} else if( nRuntimeStartup == 0 )
			{
				CWinStartup::RemoveApp(_T("x10ping"), user);
				m_pConfig->SetIntVar( SECTION_ID_RUNTIME, CONF_MONITOR_ON_STARTUP, 0 );
			}
		}

		// notificationa panel - email server settings
		strWxTmp = pConfigView->m_pNotifyEmailServer->GetLineText(0);
		sprintf( strTmp, "%s", strWxTmp.GetData() );
		m_pConfig->SetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_SERVER, strTmp );

		strWxTmp = pConfigView->m_pNotifyEmailFrom->GetLineText(0);
		sprintf( strTmp, "%s", strWxTmp.GetData() );
		m_pConfig->SetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_FROM, strTmp );

		strWxTmp = pConfigView->m_pNotifyEmailTo->GetLineText(0);
		sprintf( strTmp, "%s", strWxTmp.GetData() );
		m_pConfig->SetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_TO, strTmp );

		// email server auth
		unsigned int nNotifyAuth = pConfigView->m_pNotifyEmailAuth->GetValue( );
		m_pConfig->SetIntVar( SECTION_ID_NOTIFY, CONF_EMAIL_AUTH, nNotifyAuth );

		if( nNotifyAuth == 1 )
		{
			strWxTmp = pConfigView->m_pNotifyEmailAuthUser->GetLineText(0);
			sprintf( strTmp, "%s", strWxTmp.GetData() );
			m_pConfig->SetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_AUTH_USER, strTmp );

			strWxTmp = pConfigView->m_pNotifyEmailAuthPasswd->GetLineText(0);
			sprintf( strTmp, "%s", strWxTmp.GetData() );
			m_pConfig->SetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_AUTH_PASSWD, strTmp );
		}

		// audio notification
		strWxTmp = pConfigView->m_pNotifyAudioFile->GetLineText(0);
		sprintf( strTmp, "%s", strWxTmp.GetData() );
		m_pConfig->SetStrVar( SECTION_ID_NOTIFY, CONF_AUDIO_NOTIFY_FILE, strTmp );

		// now save my config
		m_pConfig->Save( );

	}

	delete( pConfigView );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		On About
// Class:		CGUIFrame
// Purpose:		display about dialog
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnAbout( wxCommandEvent& WXUNUSED(pEvent) )
{
	CX10PingAbout dialog(this);
    dialog.ShowModal();

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		On Button
// Class:		CGUIFrame
// Purpose:		handle the panel button press
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnButton( wxCommandEvent& pEvent )
{
	char strMsg[255];
	long item = -1;

	// get the id of the button in action
	switch( pEvent.GetId( ) )
	{
		// on add event button
		case wxID_BUTTON_STARTMON:
		{
			m_pX10PingWorker->DoStart( );
			m_pStartMonButton->Enable( 0 );
			m_pStopMonButton->Enable( 1 );

			break;
		}
		// on double click list event
		case wxID_LIST_EVENT:
		{

			break;
		}
		// on button start press
		case wxID_BUTTON_STOPMON:
		{
			m_pX10PingWorker->DoPause( );
			m_pStartMonButton->Enable( 1 );
			m_pStopMonButton->Enable( 0 );

			break;
		}
		// on button stop press
		case wxID_BUTTON_ADDDEV:
		{
			CChangeDeviceDialog  *pAddDeviceDialog = new CChangeDeviceDialog( this, _("Add Device Setup") );
			// if ok
			if( pAddDeviceDialog->ShowModal() == wxID_OK )
			{
				wxString strTemp;
				int nTmp;
				int nCheckMethod = -1;
				int nFailuresToAction = -1;
				int nActionLogic = -1;
				int nActionFunction = -1;
				int nActionNotify = -1;
				int nRebootTime = -1;
				int bCheckTimeFrame = -1;
				int nCheckStartTimeH = -1;
				int nCheckStartTimeM = -1;
				int nCheckEndTimeH = -1;
				int nCheckEndTimeM = -1;

				unsigned int nHouseId = pAddDeviceDialog->m_pX10HouseIDSelect->GetSelection()+1;
				unsigned int nDeviceId = pAddDeviceDialog->m_pX10DeviceIDSelect->GetSelection()+1;
				wxString strDeviceName = pAddDeviceDialog->m_pDeviceHostNameEntry->GetLineText(0);

				nCheckMethod = pAddDeviceDialog->m_pX10MethodHostCheck->GetSelection()+1;
				nActionLogic = pAddDeviceDialog->m_pX10ActionLogic->GetSelection()+1;
				nActionFunction = pAddDeviceDialog->m_pX10ActionFunction->GetSelection()+1;
				nActionNotify = pAddDeviceDialog->m_pX10ActionNotify->GetSelection()+1;

				strTemp = pAddDeviceDialog->m_pDeviceHostFailuresToAction->GetLineText(0);
				if( strTemp.IsNumber( ) )
				{
					nFailuresToAction = atoi( strTemp );
				}
				strTemp = pAddDeviceDialog->m_pDeviceHostRebootTimeEntry->GetLineText(0);
				if( strTemp.IsNumber( ) )
				{
						nRebootTime = atoi( strTemp );
					}

					// check if set time frame
					nTmp = pAddDeviceDialog->m_pTimeFrameCheck->GetValue( );
					if( nTmp == 1 )
					{
						bCheckTimeFrame = 1;
						wxString strHour = pAddDeviceDialog->m_pDeviceCheckStartTimeH->GetLineText(0);
						wxString strMinute = pAddDeviceDialog->m_pDeviceCheckStartTimeM->GetLineText(0);
						if( strHour.IsNumber( ) && strMinute.IsNumber( ) )
						{
							nCheckStartTimeH = atoi( strHour );
							nCheckStartTimeM = atoi( strMinute );
						} else
						{
							nCheckStartTimeH = 0;
							nCheckStartTimeM = 0;
						}

						strHour = pAddDeviceDialog->m_pDeviceCheckEndTimeH->GetLineText(0);
						strMinute = pAddDeviceDialog->m_pDeviceCheckEndTimeM->GetLineText(0);

						if( strHour.IsNumber( ) && strMinute.IsNumber( ) )
						{
							nCheckEndTimeH = atoi( strHour );
							nCheckEndTimeM = atoi( strMinute );
						} else
						{
							nCheckEndTimeH = 0;
							nCheckEndTimeM = 0;
						}

					} else
					{
						bCheckTimeFrame = 0;
					}

				m_pX10PingWorker->AddDevice( strDeviceName.GetData(), nHouseId, nDeviceId,
									nCheckMethod, nFailuresToAction, nActionLogic, nActionFunction,
										nActionNotify,
										nRebootTime, bCheckTimeFrame, nCheckStartTimeH, nCheckStartTimeM,
										nCheckEndTimeH, nCheckEndTimeM );

			}

			delete( pAddDeviceDialog );
			break;
		}
		// on button rec press
		case wxID_BUTTON_CLEARDEV:
		{
			item = -1;
			for ( ;; )
			{
				item = m_pListBox->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
				if ( item == -1 )
				break;

				if( m_pX10PingWorker->DeleteDevice( item ) == 1 )
				{
					m_pListBox->SetItem( m_pX10PingWorker->m_nDevice, 0, "" );
					m_pListBox->SetItem( m_pX10PingWorker->m_nDevice, 1, "" );
					m_pListBox->SetItem( m_pX10PingWorker->m_nDevice, 2, "" );
					m_pListBox->SetItem( m_pX10PingWorker->m_nDevice, 3, "" );
					m_pListBox->SetItemBackgroundColour( m_pX10PingWorker->m_nDevice, *wxWHITE );
				}

				// log item for deletion
//				sprintf( strMsg, "DEBUG :frame: item=%d marked for deletion", item );
//				m_pLogger->Log( strMsg );
			}

			break;
		}
		// on button rec press
		case wxID_BUTTON_EDITDEV:
		{
			//first find which device was selected for edit
			item = -1;
			for ( ;; )
			{
				item = m_pListBox->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
				if ( item == -1 ) break;

				// log item for deletion
				sprintf( strMsg, "DEBUG :frame: item=%d marked to be edited", item );
				m_pLogger->Log( strMsg );

				// get setting for the device
				char strDevName[255];
				int nDevId = 0;
				int nHousId = 0;
				int nCheckMethod = -1;
				int nFailuresToAction = -1;
				int nActionLogic = -1;
				int nActionFunction = -1;
				int nActionNotify = -1;
				int nRebootTime = -1;
				int bCheckTimeFrame = -1;
				int nCheckStartTimeH = -1;
				int nCheckStartTimeM = -1;
				int nCheckEndTimeH = -1;
				int nCheckEndTimeM = -1;
				if( m_pX10PingWorker->GetDeviceData( item, strDevName, nHousId, nDevId,
						nCheckMethod, nFailuresToAction, nActionLogic, nActionFunction,
						nActionNotify,
						nRebootTime, bCheckTimeFrame, nCheckStartTimeH, nCheckStartTimeM,
						nCheckEndTimeH, nCheckEndTimeM ) == 0 ) break;

				// create dialog
				CChangeDeviceDialog  *pEditDeviceDialog = new CChangeDeviceDialog( this, _("Edit Device Setup"),
															strDevName, nHousId, nDevId );

				pEditDeviceDialog->SetConfig( nCheckMethod, nFailuresToAction, nActionLogic, nActionFunction,
						nActionNotify,
						nRebootTime, bCheckTimeFrame, nCheckStartTimeH, nCheckStartTimeM,
						nCheckEndTimeH, nCheckEndTimeM );

				// if ok
				if( pEditDeviceDialog->ShowModal() == wxID_OK )
				{
					wxString strTemp;
					int nTmp;
					unsigned int nHouseId = pEditDeviceDialog->m_pX10HouseIDSelect->GetSelection()+1;
					unsigned int nDeviceId = pEditDeviceDialog->m_pX10DeviceIDSelect->GetSelection()+1;
					wxString strDeviceName = pEditDeviceDialog->m_pDeviceHostNameEntry->GetLineText(0);

					nCheckMethod = pEditDeviceDialog->m_pX10MethodHostCheck->GetSelection()+1;
					nActionLogic = pEditDeviceDialog->m_pX10ActionLogic->GetSelection()+1;
					nActionFunction = pEditDeviceDialog->m_pX10ActionFunction->GetSelection()+1;
					nActionNotify = pEditDeviceDialog->m_pX10ActionNotify->GetSelection()+1;

					strTemp = pEditDeviceDialog->m_pDeviceHostFailuresToAction->GetLineText(0);
					if( strTemp.IsNumber( ) )
					{
						nFailuresToAction = atoi( strTemp );
					}
					strTemp = pEditDeviceDialog->m_pDeviceHostRebootTimeEntry->GetLineText(0);
					if( strTemp.IsNumber( ) )
					{
						nRebootTime = atoi( strTemp );
					}

					// check if set time frame
					nTmp = pEditDeviceDialog->m_pTimeFrameCheck->GetValue( );
					if( nTmp == 1 )
					{
						bCheckTimeFrame = 1;
						wxString strHour = pEditDeviceDialog->m_pDeviceCheckStartTimeH->GetLineText(0);
						wxString strMinute = pEditDeviceDialog->m_pDeviceCheckStartTimeM->GetLineText(0);
						if( strHour.IsNumber( ) && strMinute.IsNumber( ) )
						{
							nCheckStartTimeH = atoi( strHour );
							nCheckStartTimeM = atoi( strMinute );
						} else
						{
							nCheckStartTimeH = 0;
							nCheckStartTimeM = 0;
						}

						strHour = pEditDeviceDialog->m_pDeviceCheckEndTimeH->GetLineText(0);
						strMinute = pEditDeviceDialog->m_pDeviceCheckEndTimeM->GetLineText(0);

						if( strHour.IsNumber( ) && strMinute.IsNumber( ) )
						{
							nCheckEndTimeH = atoi( strHour );
							nCheckEndTimeM = atoi( strMinute );
						} else
						{
							nCheckEndTimeH = 0;
							nCheckEndTimeM = 0;
						}

					} else
					{
						bCheckTimeFrame = 0;
					}

					m_pX10PingWorker->EditDevice( item, strDeviceName.GetData(), nHouseId,
										nDeviceId, nCheckMethod, nFailuresToAction, nActionLogic, nActionFunction,
										nActionNotify,
										nRebootTime, bCheckTimeFrame, nCheckStartTimeH, nCheckStartTimeM,
										nCheckEndTimeH, nCheckEndTimeM );

				}

				delete( pEditDeviceDialog );

				// now break - only first selection is used
				break;
			}

			break;
		}
		// on default
		default: break;
	}
}

////////////////////////////////////////////////////////////////////
// Method:		On Exit
// Class:		CGUIFrame
// Purpose:		exit nnro program
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnExit( wxCommandEvent& WXUNUSED(pEvent) )
{
//    this->Destroy();
	Close( );
	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnMinimize
// Class:	CGUIFrame
// Purose:	when minimize
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnMinimize( wxIconizeEvent& pEvent )
{
	if( pEvent.Iconized() )
	{
		wxIcon icon( x10ping_xpm );
		m_pFrameTaskBarIcon->SetIcon( icon, "x10ping Monitor" );
//		Iconize(TRUE);
		this->Show(FALSE);
		m_bIconized = 1;
	} else
	{
//		this->Show(TRUE);
//		this->Raise();
	}
//	pEvent.Skip( 0 );

	return;
}

////////////////////////////////////////////////////////////////////
// Method:		On Close Window
// Class:		CGUIFrame
// Purpose:		close window/nnro program
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnCloseWindow( wxCloseEvent& WXUNUSED(pEvent) )
{
    static bool destroyed = FALSE;
    if( destroyed ) return;

    this->Destroy();

    destroyed = TRUE;
}

////////////////////////////////////////////////////////////////////
// Method:		OnX10ON
// Class:		CGUIFrame
// Purpose:		if option from right click menu was to turn x10
//				devices on the go through list and do so
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnX10ON( wxCommandEvent& WXUNUSED(pEvent) )
{
	long nItem = -1;
	// for every item in the list
	for ( ;; )
	{
		nItem = m_pListBox->GetNextItem( nItem,
                             wxLIST_NEXT_ALL,
                             wxLIST_STATE_SELECTED );
		if ( nItem == -1 )
		break;

		m_pX10PingWorker->TurnDeviceOn( nItem );
	}
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnX10OFF
// Class:		CGUIFrame
// Purpose:		if option from right click menu was to turn x10
//				devices off the go through list and do so
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnX10OFF( wxCommandEvent& WXUNUSED(pEvent) )
{
	long nItem = -1;
	// for every item in the list
	for ( ;; )
	{
		nItem = m_pListBox->GetNextItem( nItem,
                             wxLIST_NEXT_ALL,
                             wxLIST_STATE_SELECTED );
		if ( nItem == -1 )
		break;

		m_pX10PingWorker->TurnDeviceOff( nItem );
	}
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnX10DeviceCheck
// Class:		CGUIFrame
// Purpose:		if option from right click menu was to check
//				devices off the go through list and do so
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnX10DeviceCheck( wxCommandEvent& WXUNUSED(pEvent) )
{
	long nItem = -1;
	// for every item in the list
	for ( ;; )
	{
		nItem = m_pListBox->GetNextItem( nItem,
                             wxLIST_NEXT_ALL,
                             wxLIST_STATE_SELECTED );
		if ( nItem == -1 )
		break;

		m_pX10PingWorker->UpdateDeviceState( nItem, DEVICE_STATE_CHECKING );
		if( m_pX10PingWorker->CheckDevice( nItem ) == 0 )
		{
			m_pX10PingWorker->UpdateDeviceState( nItem, DEVICE_STATE_DOWN );
		} else
		{
			m_pX10PingWorker->UpdateDeviceState( nItem, DEVICE_STATE_UP );
		}
	}
	return;
}


////////////////////////////////////////////////////////////////////
// Method:		On Save
// Class:		CGUIFrame
// Purpose:		save my list
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnSave( wxCommandEvent& WXUNUSED(pEvent) )
{
	wxFileDialog fileDlg( this, DIALOG_SAVE_DEVICE_FILE_TITLE, _T("./"),
					m_pX10PingWorker->m_strNetDevConfigFile, 
					DIALOG_DEVICE_FILE_TYPE, wxSAVE );

	if( fileDlg.ShowModal() == wxID_OK )
	{
		m_pX10PingWorker->SaveNetDevices( fileDlg.GetPath().GetData() );
	}
}

////////////////////////////////////////////////////////////////////
// Method:		On Load
// Class:		CGUIFrame
// Purpose:		save my list
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CGUIFrame::OnLoad( wxCommandEvent& WXUNUSED(pEvent) )
{
	wxFileDialog fileDlg( this, DIALOG_LOAD_DEVICE_FILE_TITLE, _T("./"),
					m_pX10PingWorker->m_strNetDevConfigFile, 
					DIALOG_DEVICE_FILE_TYPE, wxOPEN );

	if( fileDlg.ShowModal() == wxID_OK )
	{
		
		m_pX10PingWorker->LoadNetDevices( fileDlg.GetPath().GetData() );
	}
}
