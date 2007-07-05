# Microsoft Developer Studio Project File - Name="rmin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=rmin - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rmin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rmin.mak" CFG="rmin - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rmin - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "rmin - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rmin - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RMIN_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G6 /Zp8 /MD /W3 /GR /GX /O2 /Ob2 /I "e:\Program files\orbiter\orbitersdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RMIN_EXPORTS" /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib orbiter.lib orbitersdk.lib /nologo /dll /machine:I386 /out:"Release/transx.dll" /libpath:"e:\Program Files\orbiter\orbitersdk\lib\\"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "rmin - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RMIN_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "e:\Program files\orbiter\orbitersdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RMIN_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /i "c:\program files\orbiter\orbitersdk\include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib orbiter.lib orbitersdk.lib /nologo /dll /debug /machine:I386 /out:"Debug/transx.dll" /pdbtype:sept /libpath:"e:\Program files\orbiter\orbitersdk\lib\\"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "rmin - Win32 Release"
# Name "rmin - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\basefunction.cpp

!IF  "$(CFG)" == "rmin - Win32 Release"

# ADD CPP /Ze /vmb /GX

!ELSEIF  "$(CFG)" == "rmin - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cmdnugget.cpp
# End Source File
# Begin Source File

SOURCE=.\doublelink.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\graph.cpp
# End Source File
# Begin Source File

SOURCE=.\intercept.cpp
# End Source File
# Begin Source File

SOURCE=.\mapfunction.cpp
# End Source File
# Begin Source File

SOURCE=.\mfdfunction.cpp
# End Source File
# Begin Source File

SOURCE=.\mfdvarhandler.cpp
# End Source File
# Begin Source File

SOURCE=.\mfdvariable.cpp
# End Source File
# Begin Source File

SOURCE=.\mfdvartypes.cpp
# End Source File
# Begin Source File

SOURCE=.\orbitelements.cpp
# End Source File
# Begin Source File

SOURCE=.\parser.cpp
# End Source File
# Begin Source File

SOURCE=.\planfunction.cpp
# End Source File
# Begin Source File

SOURCE=.\shiplist.cpp
# End Source File
# Begin Source File

SOURCE=.\transx.cpp
# End Source File
# Begin Source File

SOURCE=.\TransXFunction.cpp
# End Source File
# Begin Source File

SOURCE=.\transxstate.cpp
# End Source File
# Begin Source File

SOURCE=.\viewstate.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\basefunction.h
# End Source File
# Begin Source File

SOURCE=.\cmdnugget.h
# End Source File
# Begin Source File

SOURCE=.\doublelink.h
# End Source File
# Begin Source File

SOURCE=.\graph.h
# End Source File
# Begin Source File

SOURCE=.\intercept.h
# End Source File
# Begin Source File

SOURCE=.\mapfunction.h
# End Source File
# Begin Source File

SOURCE=.\mfd.h
# End Source File
# Begin Source File

SOURCE=.\mfdfunction.h
# End Source File
# Begin Source File

SOURCE=.\mfdvarhandler.h
# End Source File
# Begin Source File

SOURCE=.\mfdvariable.h
# End Source File
# Begin Source File

SOURCE=.\mfdvartypes.h
# End Source File
# Begin Source File

SOURCE=.\orbitelements.h
# End Source File
# Begin Source File

SOURCE=.\parser.h
# End Source File
# Begin Source File

SOURCE=.\planfunction.h
# End Source File
# Begin Source File

SOURCE=.\shiplist.h
# End Source File
# Begin Source File

SOURCE=.\smartptr.h
# End Source File
# Begin Source File

SOURCE=.\transx.h
# End Source File
# Begin Source File

SOURCE=.\TransXFunction.h
# End Source File
# Begin Source File

SOURCE=.\transxstate.h
# End Source File
# Begin Source File

SOURCE=.\viewstate.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
