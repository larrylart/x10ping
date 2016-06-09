# Microsoft Developer Studio Project File - Name="x10ping" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=x10ping - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "x10ping.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "x10ping.mak" CFG="x10ping - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "x10ping - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "x10ping - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "x10ping - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "C:\wxWidgets-2.6.3\lib\vc_lib\msw" /I "C:\wxWidgets-2.6.3\include" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WXMSW__" /D "NOPCH" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1809 /d "NDEBUG"
# ADD RSC /l 0x1809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wxmsw26_adv.lib wxmsw26_core.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /libpath:"C:\wxWidgets-2.6.3\lib\vc_lib"

!ELSEIF  "$(CFG)" == "x10ping - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /GR /GX /Zi /Od /I "C:\wxWidgets-2.6.3\lib\vc_lib\mswd" /I "C:\wxWidgets-2.6.3\include" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WXMSW__" /D "__WXDEBUG__" /D "NOPCH" /FR"" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1809 /d "_DEBUG"
# ADD RSC /l 0x1809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxmsw26d_adv.lib wxmsw26d_core.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /nodefaultlib:"LIBCD" /nodefaultlib:"libc.lib" /pdbtype:sept /libpath:"C:\wxWidgets-2.6.3\lib\vc_lib"

!ENDIF 

# Begin Target

# Name "x10ping - Win32 Release"
# Name "x10ping - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\gui\about.cpp
# End Source File
# Begin Source File

SOURCE=.\util\Base64.cpp
# End Source File
# Begin Source File

SOURCE=.\config\config.cpp
# End Source File
# Begin Source File

SOURCE=.\gui\configview.cpp
# End Source File
# Begin Source File

SOURCE=.\gui\frame.cpp
# End Source File
# Begin Source File

SOURCE=.\util\func.cpp
# End Source File
# Begin Source File

SOURCE=.\logger\logger.cpp
# End Source File
# Begin Source File

SOURCE=.\network\netdevice.cpp
# End Source File
# Begin Source File

SOURCE=.\ping.cpp
# End Source File
# Begin Source File

SOURCE=.\util\Smtp.cpp
# End Source File
# Begin Source File

SOURCE=.\dataport\winserial.cpp
# End Source File
# Begin Source File

SOURCE=.\util\winstartup.cpp
# End Source File
# Begin Source File

SOURCE=.\worker.cpp
# End Source File
# Begin Source File

SOURCE=.\x10.cpp
# End Source File
# Begin Source File

SOURCE=.\x10ping.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\gui\about.h
# End Source File
# Begin Source File

SOURCE=.\util\Base64.h
# End Source File
# Begin Source File

SOURCE=.\config\config.h
# End Source File
# Begin Source File

SOURCE=.\gui\configview.h
# End Source File
# Begin Source File

SOURCE=.\gui\frame.h
# End Source File
# Begin Source File

SOURCE=.\util\func.h
# End Source File
# Begin Source File

SOURCE=.\logger\logger.h
# End Source File
# Begin Source File

SOURCE=.\network\netdevice.h
# End Source File
# Begin Source File

SOURCE=.\ping.h
# End Source File
# Begin Source File

SOURCE=.\util\Smtp.h
# End Source File
# Begin Source File

SOURCE=.\dataport\winserial.h
# End Source File
# Begin Source File

SOURCE=.\util\winstartup.h
# End Source File
# Begin Source File

SOURCE=.\worker.h
# End Source File
# Begin Source File

SOURCE=.\x10.h
# End Source File
# Begin Source File

SOURCE=.\x10ping.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Icon0069.ico
# End Source File
# Begin Source File

SOURCE=.\x10ping.rc
# End Source File
# Begin Source File

SOURCE=.\x10pingabout.bmp
# End Source File
# End Group
# End Target
# End Project
