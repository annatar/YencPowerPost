# Microsoft Developer Studio Project File - Name="PostMost" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PostMost - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PostMost.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PostMost.mak" CFG="PostMost - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PostMost - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PostMost - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PostMost - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /D "ASMENCODE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "PostMost - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /Ob1 /D "ASMENCODE" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/PostMostDbg.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PostMost - Win32 Release"
# Name "PostMost - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddGroupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoRetryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\Common.cpp
# End Source File
# Begin Source File

SOURCE=.\DelayedPostingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DeleteHistoryConfirmDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DeleteTaskDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EncoderThread.cpp
# End Source File
# Begin Source File

SOURCE=.\EncodingProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HistoryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\mt19937ar.cpp
# End Source File
# Begin Source File

SOURCE=.\par2format.cpp
# End Source File
# Begin Source File

SOURCE=.\PosterThread.cpp
# End Source File
# Begin Source File

SOURCE=.\PostingProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PostMost.cpp
# End Source File
# Begin Source File

SOURCE=.\PostMost.rc
# End Source File
# Begin Source File

SOURCE=.\PostMostDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\PostMostPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\PostMostView.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgramSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\proppage_add_checksums.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPage_Add_FileOrder.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPage_Add_Groups.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPage_Add_Recovery.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPage_Add_Subject.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPage_Add_TextPrefix.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPage_Groups.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPage_Headers.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPage_ProgramSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\PropPage_Server.cpp
# End Source File
# Begin Source File

SOURCE=.\SetMaxLinesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SplashWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskObject.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskProp_FileAndParts.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskProp_Groups.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskProp_Log.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskProp_PrefixText.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreadMarshal.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AddGroupDlg.h
# End Source File
# Begin Source File

SOURCE=.\AutoRetryDlg.h
# End Source File
# Begin Source File

SOURCE=.\BitmapLabel.h
# End Source File
# Begin Source File

SOURCE=.\crc32table.h
# End Source File
# Begin Source File

SOURCE=.\DelayedPostingDlg.h
# End Source File
# Begin Source File

SOURCE=.\DeleteHistoryConfirmDlg.h
# End Source File
# Begin Source File

SOURCE=.\DeleteTaskDlg.h
# End Source File
# Begin Source File

SOURCE=.\EncoderThread.h
# End Source File
# Begin Source File

SOURCE=.\EncodingProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\HistoryDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\par2format.h
# End Source File
# Begin Source File

SOURCE=.\PosterThread.h
# End Source File
# Begin Source File

SOURCE=.\PostingProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\PostMost.h
# End Source File
# Begin Source File

SOURCE=.\PostMostDoc.h
# End Source File
# Begin Source File

SOURCE=.\PostMostPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\PostMostView.h
# End Source File
# Begin Source File

SOURCE=.\ProgramSettings.h
# End Source File
# Begin Source File

SOURCE=.\proppage_add_checksums.h
# End Source File
# Begin Source File

SOURCE=.\PropPage_Add_FileOrder.h
# End Source File
# Begin Source File

SOURCE=.\PropPage_Add_Groups.h
# End Source File
# Begin Source File

SOURCE=.\PropPage_Add_Recovery.h
# End Source File
# Begin Source File

SOURCE=.\PropPage_Add_Subject.h
# End Source File
# Begin Source File

SOURCE=.\PropPage_Add_TextPrefix.h
# End Source File
# Begin Source File

SOURCE=.\PropPage_Groups.h
# End Source File
# Begin Source File

SOURCE=.\PropPage_Headers.h
# End Source File
# Begin Source File

SOURCE=.\PropPage_ProgramSettings.h
# End Source File
# Begin Source File

SOURCE=.\PropPage_Server.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SetMaxLinesDlg.h
# End Source File
# Begin Source File

SOURCE=.\SplashWindow.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TaskObject.h
# End Source File
# Begin Source File

SOURCE=.\TaskProp_FileAndParts.h
# End Source File
# Begin Source File

SOURCE=.\TaskProp_Groups.h
# End Source File
# Begin Source File

SOURCE=.\TaskProp_Log.h
# End Source File
# Begin Source File

SOURCE=.\TaskProp_PrefixText.h
# End Source File
# Begin Source File

SOURCE=.\ThreadMarshal.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ERROR.ICO
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\PostMost.ico
# End Source File
# Begin Source File

SOURCE=.\res\PostMost.rc2
# End Source File
# Begin Source File

SOURCE=.\res\PostMostDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\QUESTION.ICO
# End Source File
# Begin Source File

SOURCE=.\Hist\Splash.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stopwatch.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\assert_readme.txt
# End Source File
# Begin Source File

SOURCE=.\original_source_readme.txt
# End Source File
# Begin Source File

SOURCE=.\yenc_source_readme.txt
# End Source File
# End Target
# End Project
