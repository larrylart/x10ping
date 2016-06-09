////////////////////////////////////////////////////////////////////
// Package:		Chorg Config View implementation
// File:		configview.cpp
// Purpose:		define a config view window
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
#include "wx/thread.h"
#include "wx/notebook.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

// local headers
#include "../config/config.h"
#include "../x10.h"
#include "../worker.h"

// main header
#include "configview.h"

////////////////////////////////////////////////////////////////////
// CChangeDeviceDialog EVENTS TABLE
////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( CConfigView, wxDialog )
	EVT_CHECKBOX( wxID_CHECK_EMAIL_AUTH, CConfigView::OnEmailAuth )
	EVT_BUTTON( wxID_BUTTON_FILEBROWSE, CConfigView::OnAudioFileSelect )
	EVT_CHOICE( wxID_X10_CTRL_SELECT, CConfigView::OnSelectCtrlType )
END_EVENT_TABLE()


////////////////////////////////////////////////////////////////////
// Method:	Constructor
// Class:	CConfigView
// Purpose:	Initialize my about dialog // 
// Input:	pointer to reference window 
// Output:	nothing
////////////////////////////////////////////////////////////////////
CConfigView::CConfigView(wxWindow *parent)
               : wxDialog(parent, -1,
                          _("x10ping configuration"),
                          wxDefaultPosition,
                          wxSize( 360, 440 ),
                          wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL )
{
	int i=0;
	// initialize local vars
	m_pConfig = NULL;
	wxString vectStrX10Dev[20];
	wxString vectStrX10Type[5];
	// init selection for x10 serial port
	for( i=0; i<20; i++ ) vectStrX10Dev[i].sprintf( "%.2d", i+1 );
	// init selection for x10 controler type
	vectStrX10Type[0] = _T("CM11A/CM12U");
	vectStrX10Type[1] = _T("ACTIVEHOME CM15A PLC");
	vectStrX10Type[2] = _T("ACTIVEHOME CM15A RF");
	vectStrX10Type[3] = _T("ACTIVEHOME CM19A");
	vectStrX10Type[4] = _T("JDS STARGATE");


	// main dialog sizer
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	// building configure as a note book
	wxNotebook *pNotebook = new wxNotebook( this, -1 ); //, wxPoint( 0, 0 ),
									//wxSize( 450, 540 ) );
	// add X10 config panel
	wxPanel* pPanelX10 = new wxPanel( pNotebook );
	// add Worker config panel
	wxPanel* pPanelWorker = new wxPanel( pNotebook );
	// add runtime panel
	wxPanel* pPanelRuntime = new wxPanel( pNotebook );
	// add notification panel
	wxPanel* pPanelNotify = new wxPanel( pNotebook );

	/////////////////////////////
	// x10 panel sizer - layout of elements
	wxGridSizer *sizerX10Panel = new wxGridSizer( 2, 2, 10, 10 );
	// layout of group of elements in the panel 
	wxGridSizer* sizerX10Page = new wxGridSizer( 1, 1, 10, 10 );
	sizerX10Page->Add( sizerX10Panel, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	/////////////////////////
	// worker panel sizer
	wxFlexGridSizer *sizerWorkerPanel = new wxFlexGridSizer( 4, 2, 1, 10 );
	// layout of group of elements in the panel 
	wxGridSizer* sizerWorkerPage = new wxGridSizer( 1, 1, 2, 2 );
	sizerWorkerPage->Add( sizerWorkerPanel, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );
	// set column 0 growable
	sizerWorkerPanel->AddGrowableCol( 0, 20 );

	//////////////////////////////
	// runtime panel sizer
	wxGridSizer *sizerRuntimePanel = new wxGridSizer( 1, 2, 10, 10 );
	// layout of group of elements in the panel 
	wxGridSizer* sizerRuntimePage = new wxGridSizer( 1, 1, 10, 10 );
	sizerRuntimePage->Add( sizerRuntimePanel, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	/////////////////////////////
	// notify panel sizer
	// main notify sizer
//	wxBoxSizer *topNotifySizer = new wxBoxSizer( wxVERTICAL );

	wxGridSizer *sizerNotifyPanelOne = new wxFlexGridSizer( 1, 2, 7, 7 );
	wxGridSizer *sizerNotifyPanelTwo = new wxFlexGridSizer( 1, 2, 7, 7 );

	// layout of group of elements in the panel 
	wxGridSizer* sizerNotifyPage = new wxFlexGridSizer( 2, 1, 10, 10 );

	wxStaticBox* pNotifyEmailBox = new wxStaticBox( pPanelNotify, -1, _( "Email" ) );
	wxStaticBoxSizer* pNotifyEmailBoxSizer = new wxStaticBoxSizer( pNotifyEmailBox, wxVERTICAL );

	sizerNotifyPage->Add( pNotifyEmailBoxSizer, 0,
                  wxALIGN_CENTER | wxEXPAND );

	pNotifyEmailBoxSizer->Add( sizerNotifyPanelOne, 0,
                  wxALIGN_CENTER | wxALIGN_CENTER_VERTICAL );

	wxStaticBox* pNotifyAudioBox = new wxStaticBox( pPanelNotify, -1, _( "Audio" ) );
	wxStaticBoxSizer* pNotifyAudioBoxSizer = new wxStaticBoxSizer( pNotifyAudioBox, wxVERTICAL );

	sizerNotifyPage->Add( pNotifyAudioBoxSizer, 0,
                  wxALIGN_CENTER | wxEXPAND );

	pNotifyAudioBoxSizer->Add( sizerNotifyPanelTwo, 0,
                  wxALIGN_CENTER | wxEXPAND );

//	pNotifyAudioBoxSizer->Fit( pPanelNotify );

	///////////////////////////////////////////
	// elements definition for x10 panel
	m_pSerialPortSelect = new wxChoice( pPanelX10, -1,
										wxDefaultPosition,
										wxSize(40,-1), 20, vectStrX10Dev, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );

	m_pX10ControllerType = new wxChoice( pPanelX10, wxID_X10_CTRL_SELECT,
										wxDefaultPosition,
										wxSize(140,-1), 5, vectStrX10Type, 
										wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL );
	m_pX10ControllerType->SetSelection( 0 );

 	m_pDevicePingRetryEntry = new wxTextCtrl( pPanelWorker, -1, _T(""),
									wxDefaultPosition, wxSize(60,-1) );

 	m_pNetLoopCheckDelayEntry = new wxTextCtrl( pPanelWorker, -1, _T(""),
									wxDefaultPosition, wxSize(60,-1) );

	m_pX10DelayAfterOFFEntry = new wxTextCtrl( pPanelWorker, -1, _T(""),
									wxDefaultPosition, wxSize(60,-1) );

	m_pX10DelayAfterONEntry = new wxTextCtrl( pPanelWorker, -1, _T(""),
									wxDefaultPosition, wxSize(60,-1) );

	m_pRuntimeStartup = new wxCheckBox( pPanelRuntime, -1, _T("") );

	// define entries for notification
	m_pNotifyEmailServer = new wxTextCtrl( pPanelNotify, -1, _T(""),
									wxDefaultPosition, wxSize( 180,-1) );

	m_pNotifyEmailFrom = new wxTextCtrl( pPanelNotify, -1, _T(""),
									wxDefaultPosition, wxSize( 180,-1) );

	m_pNotifyEmailTo = new wxTextCtrl( pPanelNotify, -1, _T(""),
									wxDefaultPosition, wxSize( 180,-1) );

	m_pNotifyEmailAuth = new wxCheckBox( pPanelNotify, wxID_CHECK_EMAIL_AUTH, _T("") );


	m_pNotifyEmailAuthUser = new wxTextCtrl( pPanelNotify, -1, _T(""),
									wxDefaultPosition, wxSize( 180,-1) );

	m_pNotifyEmailAuthPasswd = new wxTextCtrl( pPanelNotify, -1, _T(""),
									wxDefaultPosition, wxSize( 180,-1), wxTE_PASSWORD );

	m_pNotifyAudioFile = new wxTextCtrl( pPanelNotify, -1, _T(""),
									wxDefaultPosition, wxSize(250,-1) );

	m_pSerialPortSelect->SetSelection( 0 );

	////////////////
	// populate x10 panel
	// row for serial port select
	sizerX10Panel->Add(new wxStaticText( pPanelX10, -1, _T("X10 Serial Port Number:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerX10Panel->Add( m_pSerialPortSelect, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for controler type
	sizerX10Panel->Add(new wxStaticText( pPanelX10, -1, _T("X10 Controller Type:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerX10Panel->Add( m_pX10ControllerType, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	////////////////
	// populate worker panel	
	// row for net device pin retry
	sizerWorkerPanel->Add( new wxStaticText( pPanelWorker, -1, _T("Number of Ping Retries:")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerWorkerPanel->Add( m_pDevicePingRetryEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for network loop delay
	sizerWorkerPanel->Add( new wxStaticText( pPanelWorker, -1, _T("Net Loop Check Delay(ms):")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerWorkerPanel->Add( m_pNetLoopCheckDelayEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for x10 delay after off
	sizerWorkerPanel->Add( new wxStaticText( pPanelWorker, -1, _T("Delay After Switching OFF(ms):")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerWorkerPanel->Add( m_pX10DelayAfterOFFEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// row for x10 delay after on
	sizerWorkerPanel->Add( new wxStaticText( pPanelWorker, -1, _T("Delay After Switching ON(ms):")), 0,
						wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT  );
	sizerWorkerPanel->Add( m_pX10DelayAfterONEntry, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	/////////////////////////////
	// populate runtime panel
	sizerRuntimePanel->Add(new wxStaticText( pPanelRuntime, -1, _T("Run x10ping on startup:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerRuntimePanel->Add( m_pRuntimeStartup, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	/////////////////////////////
	// populate notify panel 
	sizerNotifyPanelOne->Add(new wxStaticText( pPanelNotify, -1, _T("Use SMTP email server:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerNotifyPanelOne->Add( m_pNotifyEmailServer, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	sizerNotifyPanelOne->Add(new wxStaticText( pPanelNotify, -1, _T("Sender's email:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerNotifyPanelOne->Add( m_pNotifyEmailFrom, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	sizerNotifyPanelOne->Add(new wxStaticText( pPanelNotify, -1, _T("Destination email:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerNotifyPanelOne->Add( m_pNotifyEmailTo, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	sizerNotifyPanelOne->Add(new wxStaticText( pPanelNotify, -1, _T("Use SMTP authentification:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerNotifyPanelOne->Add( m_pNotifyEmailAuth, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// smtp auth
	m_pNotifyEmailAuthUserLabel = new wxStaticText( pPanelNotify, -1, _T("SMTP auth user:") );
	sizerNotifyPanelOne->Add( m_pNotifyEmailAuthUserLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL );
	sizerNotifyPanelOne->Add( m_pNotifyEmailAuthUser, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	m_pNotifyEmailAuthPasswdLabel = new wxStaticText( pPanelNotify, -1, _T("SMTP auth password:") );
	sizerNotifyPanelOne->Add( m_pNotifyEmailAuthPasswdLabel, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerNotifyPanelOne->Add( m_pNotifyEmailAuthPasswd, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );
	// disable auth entry by default
	DisableEmailAuth( );

	// the audio notifier
	sizerNotifyPanelTwo->Add(new wxStaticText( pPanelNotify, -1, _T("Choose your audio notification file:")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
	sizerNotifyPanelTwo->Add(new wxStaticText( pPanelNotify, -1, _T("")), 0,
                  wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

	sizerNotifyPanelTwo->Add( m_pNotifyAudioFile, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	wxButton* pFileBrowseButton = new wxButton( pPanelNotify, wxID_BUTTON_FILEBROWSE, "Browse" ); 
//										wxPoint( nStopMonButtonX, nControlPanelY+18 ), 
//										wxSize(nButtonWidth,20) );

	sizerNotifyPanelTwo->Add( pFileBrowseButton, 1, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL  );

	// calculate sizers
	topsizer->Add( pNotebook, 1, wxGROW | wxALL, 10 );
	topsizer->Add( CreateButtonSizer(wxOK | wxCANCEL), 0, wxCENTRE | wxALL, 10 );


	pPanelX10->SetSizer( sizerX10Page );
	pNotebook->AddPage( pPanelX10, _("X10") );
	pPanelWorker->SetSizer( sizerWorkerPage );
	pNotebook->AddPage( pPanelWorker, _("Worker") );
	pPanelRuntime->SetSizer( sizerRuntimePage );
	pNotebook->AddPage( pPanelRuntime, _("Runtime") );
	pPanelNotify->SetSizer( sizerNotifyPage );
	pNotebook->AddPage( pPanelNotify, _( "Notify" ) );

//	sizerX10Page->FitInside( pPanelX10 );
//	pPanelX10->Centre( wxBOTH );

	// activate
	SetSizer(topsizer);
	SetAutoLayout(TRUE);
	topsizer->SetSizeHints(this);
//	sizer->Fit(this);
	Centre(wxBOTH | wxCENTRE_ON_SCREEN);
}

////////////////////////////////////////////////////////////////////
// Method:	Destructor
// Class:	CConfigView
// Purpose:	destroy my  dialog
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
CConfigView::~CConfigView( ) 
{
	m_pConfig = NULL;
	delete( m_pSerialPortSelect );
	delete( m_pX10ControllerType );
	delete( m_pDevicePingRetryEntry );
	delete( m_pNetLoopCheckDelayEntry );
	delete( m_pX10DelayAfterOFFEntry );
	delete( m_pX10DelayAfterONEntry );
	delete( m_pRuntimeStartup );
	delete( m_pNotifyEmailServer );
	delete( m_pNotifyEmailFrom );
	delete( m_pNotifyEmailTo );
	delete( m_pNotifyEmailAuth );
	delete( m_pNotifyEmailAuthUser );
	delete( m_pNotifyEmailAuthPasswd );
	delete( m_pNotifyEmailAuthUserLabel );
	delete( m_pNotifyEmailAuthPasswdLabel ); 
	delete( m_pNotifyAudioFile );
}

////////////////////////////////////////////////////////////////////
// Method:	OnEmailAuth
// Class:	CConfigView
// Purpose:	use when check box is un/ticked
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CConfigView::OnEmailAuth( wxCommandEvent& pEvent )
{
	if( m_pNotifyEmailAuth->IsChecked( ) )
	{ 
		EnableEmailAuth( );
	} else
	{
		DisableEmailAuth( );
	}
}

////////////////////////////////////////////////////////////////////
// Method:	DisableEmailAuth
// Class:	CConfigView
// Purpose:	disable email auth entry
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CConfigView::DisableEmailAuth( )
{
	m_pNotifyEmailAuthUserLabel->Disable();
	m_pNotifyEmailAuthUser->Disable();
	m_pNotifyEmailAuthPasswdLabel->Disable();
	m_pNotifyEmailAuthPasswd->Disable();
}

////////////////////////////////////////////////////////////////////
// Method:	EnableEmailAuth
// Class:	CConfigView
// Purpose:	enable email auth entry
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CConfigView::EnableEmailAuth( )
{
	m_pNotifyEmailAuthUserLabel->Enable();
	m_pNotifyEmailAuthUser->Enable();
	m_pNotifyEmailAuthPasswdLabel->Enable();
	m_pNotifyEmailAuthPasswd->Enable();
}

////////////////////////////////////////////////////////////////////
// Method:	StoreConfig
// Class:	CConfigView
// Purpose:	store my configuration in config object->file save
// Input:	nothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CConfigView::StoreConfig( ) 
{
//	m_pConfig->SetVar( SECTION_ID_VCORTEX, CONF_VCORTEX_CONTOUR, 
//						(int) m_pContourUse->GetValue() );
	return;
}

////////////////////////////////////////////////////////////////////
// Method:		OnAudioFileSelect
// Class:		CGUIFrame
// Purpose:		handle the panel button press
// Input:		pointer to event
// Output:		nothing
////////////////////////////////////////////////////////////////////
void CConfigView::OnAudioFileSelect( wxCommandEvent& pEvent )
{
	char strLoadPath[255];
	char strLoadMsg[255];
	char strLoadPattern[255];
	wxString strFileName;

	strcpy( strLoadPath, "./" );
	strcpy( strLoadMsg, FILE_LOAD_AUDIO_TITLE );
	strcpy( strLoadPattern, FILE_LOAD_AUDIO_TYPE );

	// create a file choser 
	wxFileDialog filedlg( this,
                         strLoadMsg,
                         _(""),
                         _(""),
                         strLoadPattern,
                         wxOPEN | wxFILE_MUST_EXIST );

	// set path to curret or + data dir (TODO)
	filedlg.SetPath( strLoadPath );
	// if ok
	if( filedlg.ShowModal() == wxID_OK )
	{
		// debug
		strFileName = filedlg.GetPath( );
//		sprintf( strTmp, "INFO :: going to load file %s\n", strFileName.GetData() );
//		m_pLogger->Log( strTmp );
		// check the envent's id

		// now do something here
		m_pNotifyAudioFile->SetValue( strFileName.GetData() );
	}

}

////////////////////////////////////////////////////////////////////
// Method:	SetConfig
// Class:	CConfigView
// Purpose:	Set and get my reference from the config object
// Input:	pointer to reference window
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CConfigView::SetConfig( CConfig* pConfig )
{
	m_pConfig = pConfig;
	int nVar = 0;
	char strMsg[255];
	char strVar[255];
	char strTmp[255];

	// get serial port number
	if( m_pConfig->GetIntVar( SECTION_ID_X10, CONF_X10_SERIALPORT, nVar ) == 0 )
	{
		m_pSerialPortSelect->SetSelection( 0 );

	} else
	{
		m_pSerialPortSelect->SetSelection( nVar-1 );
	}

	// get x10 controller model
	if( m_pConfig->GetIntVar( SECTION_ID_X10, CONF_X10_CONTROLLER_MODEL, nVar ) == 0 )
	{
		m_pX10ControllerType->SetSelection( 0 );

	} else
	{
		m_pX10ControllerType->SetSelection( nVar-1 );
	}

	// get serial port number
	if( m_pConfig->GetIntVar( SECTION_ID_NETWORK, CONF_NETWORK_LOOP_CHECK_DELAY, nVar ) == 0 )
	{

	} else 
	{
		sprintf( strMsg, "%d", nVar );
		m_pNetLoopCheckDelayEntry->SetValue( strMsg );
	}
	// get net device ping retry
	if( m_pConfig->GetIntVar( SECTION_ID_NETWORK, CONF_DEVICE_PING_RETRY, nVar ) == 0 )
	{

	} else
	{
		sprintf( strMsg, "%d", nVar );
		m_pDevicePingRetryEntry->SetValue( strMsg );
	}
	// x10 delay after off
	if( m_pConfig->GetIntVar( SECTION_ID_NETWORK, CONF_NETWORK_X10_DELAY_AFTER_OFF, nVar ) == 0 )
	{

	} else
	{
		sprintf( strMsg, "%d", nVar );
		m_pX10DelayAfterOFFEntry->SetValue( strMsg );
	}
	// x10 delay after on
	if( m_pConfig->GetIntVar( SECTION_ID_NETWORK, CONF_NETWORK_X10_DELAY_AFTER_ON, nVar ) == 0 )
	{

	} else
	{
		sprintf( strMsg, "%d", nVar );
		m_pX10DelayAfterONEntry->SetValue( strMsg );
	}

	// runtime setup
	if( m_pConfig->GetIntVar( SECTION_ID_RUNTIME, CONF_RUN_ON_STARTUP, nVar ) == 0 )
	{
		m_pRuntimeStartup->SetValue( 0 );

	} else
	{
		m_pRuntimeStartup->SetValue( nVar );
	}

	// notification setup
	// email server
	if( m_pConfig->GetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_SERVER, strVar ) == 0 )
	{
		sprintf( strTmp, "%s", "" );
		m_pNotifyEmailServer->SetValue( strTmp );
	} else
	{
		m_pNotifyEmailServer->SetValue( strVar );
	}
	// email from
	if( m_pConfig->GetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_FROM, strVar ) == 0 )
	{
		sprintf( strTmp, "%s", "" );
		m_pNotifyEmailFrom->SetValue( strTmp );
	} else
	{
		m_pNotifyEmailFrom->SetValue( strVar );
	}
	// email to
	if( m_pConfig->GetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_TO, strVar ) == 0 )
	{
		sprintf( strTmp, "%s", "" );
		m_pNotifyEmailTo->SetValue( strTmp );
	} else
	{
		m_pNotifyEmailTo->SetValue( strVar );
	}

	// mail notfiction auth
	if( m_pConfig->GetIntVar( SECTION_ID_NOTIFY, CONF_EMAIL_AUTH, nVar ) == 0 )
	{
		m_pNotifyEmailAuth->SetValue( 0 );
		DisableEmailAuth( );

	} else
	{
		m_pNotifyEmailAuth->SetValue( nVar );
		if( m_pNotifyEmailAuth->IsChecked( ) )
		{ 
			EnableEmailAuth( );
			// user
			if( m_pConfig->GetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_AUTH_USER, strVar ) == 0 )
			{
				sprintf( strTmp, "%s", "" );
				m_pNotifyEmailAuthUser->SetValue( strTmp );
			} else
			{
				m_pNotifyEmailAuthUser->SetValue( strVar );
			}
			// passwd
			if( m_pConfig->GetStrVar( SECTION_ID_NOTIFY, CONF_EMAIL_AUTH_PASSWD, strVar ) == 0 )
			{
				sprintf( strTmp, "%s", "" );
				m_pNotifyEmailAuthPasswd->SetValue( strTmp );
			} else
			{
				m_pNotifyEmailAuthPasswd->SetValue( strVar );
			}

		} else
		{
			DisableEmailAuth( );
		}

	}

	// audio notification
	if( m_pConfig->GetStrVar( SECTION_ID_NOTIFY, CONF_AUDIO_NOTIFY_FILE, strVar ) == 0 )
	{
		sprintf( strTmp, "%s", "" );
		m_pNotifyAudioFile->SetValue( strTmp );
	} else
	{
		m_pNotifyAudioFile->SetValue( strVar );
	}

	return;
}

////////////////////////////////////////////////////////////////////
// Method:	OnSelectCtrlType
// Class:	CConfigView
// Purose:	when control tyle is selected
// Input:	mothing
// Output:	nothing
////////////////////////////////////////////////////////////////////
void CConfigView::OnSelectCtrlType( wxCommandEvent& event )
{

	int nType = event.GetSelection() + 1;

	if( nType == X10_CONTROLLER_TYPE_CM15A_PLC ||
		nType == X10_CONTROLLER_TYPE_CM15A_RF ||
		nType == X10_CONTROLLER_TYPE_CM19A ) 
	{
		m_pSerialPortSelect->Disable();
	} else
	{
		m_pSerialPortSelect->Enable();
	}

}

