# Microsoft Developer Studio Project File - Name="HPB_bot" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=HPB_bot - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "assist.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "assist.mak" CFG="HPB_bot - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HPB_bot - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HPB_bot - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HPB_bot - Win32 SA DLL ONLY" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HPB_bot - Win32 CS DLL" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SDKSrc/Public/dlls", NVGBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HPB_bot - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "SZ_DLL" /D "CAMERA" /D "GRAPPLE" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\HPB_bot.def" /out:"d:\half life\tfc\dlls\assist.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\HPB_bot___Win"
# PROP BASE Intermediate_Dir ".\HPB_bot___Win"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "GRAPPLE" /D "AVC" /D "SZ_DLL" /D "CAMERA" /D "RECORD" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\engine" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 user32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\assist.def"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 SA DLL ONLY"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "HPB_bot___Win32_SA_DLL_ONLY"
# PROP BASE Intermediate_Dir "HPB_bot___Win32_SA_DLL_ONLY"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\SAOnly"
# PROP Intermediate_Dir ".\SAonly"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "SZ_DLL" /D "CAMERA" /D "GRAPPLE" /YX /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\HPB_bot.def" /out:"d:\half life\tfc\dlls\assist.dll"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\HPB_bot.def" /out:"d:\half life\tfc\dlls\assist.dll"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "HPB_bot - Win32 CS DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "HPB_bot___Win32_CS_DLL"
# PROP BASE Intermediate_Dir "HPB_bot___Win32_CS_DLL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "HPB_bot___Win32_CS_DLL"
# PROP Intermediate_Dir "HPB_bot___Win32_CS_DLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "SZ_DLL" /D "CAMERA" /D "GRAPPLE" /YX /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /G5 /MT /W3 /GX /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "VALVE_DLL" /D "GRAPPLE" /D "AVC" /D "SZ_DLL" /D "CAMERA" /D "RECORD" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\HPB_bot.def" /out:"d:\half life\tfc\dlls\assist.dll"
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /map /debug /machine:I386 /def:".\assist.def" /out:"m:\half life\tfc\dlls\assist.dll"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "HPB_bot - Win32 Release"
# Name "HPB_bot - Win32 Debug"
# Name "HPB_bot - Win32 SA DLL ONLY"
# Name "HPB_bot - Win32 CS DLL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\avbfg.cpp
# End Source File
# Begin Source File

SOURCE=.\avchess.cpp
# End Source File
# Begin Source File

SOURCE=.\avclock.cpp
# End Source File
# Begin Source File

SOURCE=.\avdamage.cpp
# End Source File
# Begin Source File

SOURCE=.\avdance.cpp
# End Source File
# Begin Source File

SOURCE=.\avdll.cpp
# End Source File
# Begin Source File

SOURCE=.\avflare.cpp
# End Source File
# Begin Source File

SOURCE=.\avfountain.cpp
# End Source File
# Begin Source File

SOURCE=.\avfox.cpp
# End Source File
# Begin Source File

SOURCE=.\avgrapple.cpp
# End Source File
# Begin Source File

SOURCE=.\avleash.cpp
# End Source File
# Begin Source File

SOURCE=.\avpaper.cpp
# End Source File
# Begin Source File

SOURCE=.\avpara.cpp
# End Source File
# Begin Source File

SOURCE=.\avrace.cpp
# End Source File
# Begin Source File

SOURCE=.\avradio.cpp
# End Source File
# Begin Source File

SOURCE=.\avrecord.cpp
# End Source File
# Begin Source File

SOURCE=.\avremote.cpp
# End Source File
# Begin Source File

SOURCE=.\avsign.cpp
# End Source File
# Begin Source File

SOURCE=.\avslot.cpp
# End Source File
# Begin Source File

SOURCE=.\avsnow.cpp
# End Source File
# Begin Source File

SOURCE=.\avtractor.cpp
# End Source File
# Begin Source File

SOURCE=.\avtripmine.cpp
# End Source File
# Begin Source File

SOURCE=.\avwep.cpp
# End Source File
# Begin Source File

SOURCE=.\avwritewep.cpp
# End Source File
# Begin Source File

SOURCE=.\bd.cpp
# End Source File
# Begin Source File

SOURCE=.\buildcam.cpp
# End Source File
# Begin Source File

SOURCE=.\cbeam.cpp
# End Source File
# Begin Source File

SOURCE=.\custmotd.cpp
# End Source File
# Begin Source File

SOURCE=".\dll.cpp"
# End Source File
# Begin Source File

SOURCE=.\drivecheck.cpp
# End Source File
# Begin Source File

SOURCE=.\engine.cpp
# End Source File
# Begin Source File

SOURCE=.\h_export.cpp
# End Source File
# Begin Source File

SOURCE=.\linkfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\phatmat.cpp
# End Source File
# Begin Source File

SOURCE=.\sadll.cpp
# End Source File
# Begin Source File

SOURCE=.\smoke.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\avbfg.h
# End Source File
# Begin Source File

SOURCE=.\avchess.h
# End Source File
# Begin Source File

SOURCE=.\avclock.h
# End Source File
# Begin Source File

SOURCE=.\avdamage.h
# End Source File
# Begin Source File

SOURCE=.\avdance.h
# End Source File
# Begin Source File

SOURCE=.\avdll.h
# End Source File
# Begin Source File

SOURCE=.\avflare.h
# End Source File
# Begin Source File

SOURCE=.\avfountain.h
# End Source File
# Begin Source File

SOURCE=.\avfox.h
# End Source File
# Begin Source File

SOURCE=.\avgrapple.h
# End Source File
# Begin Source File

SOURCE=.\avpara.h
# End Source File
# Begin Source File

SOURCE=.\avrace.h
# End Source File
# Begin Source File

SOURCE=.\avradio.h
# End Source File
# Begin Source File

SOURCE=.\avrecord.h
# End Source File
# Begin Source File

SOURCE=.\avremote.h
# End Source File
# Begin Source File

SOURCE=.\avsign.h
# End Source File
# Begin Source File

SOURCE=.\avslot.h
# End Source File
# Begin Source File

SOURCE=.\avsnow.h
# End Source File
# Begin Source File

SOURCE=.\avtractor.h
# End Source File
# Begin Source File

SOURCE=.\avtripmine.h
# End Source File
# Begin Source File

SOURCE=.\avwep.h
# End Source File
# Begin Source File

SOURCE=.\avwritewep.h
# End Source File
# Begin Source File

SOURCE=.\bd.h
# End Source File
# Begin Source File

SOURCE=.\bot.h
# End Source File
# Begin Source File

SOURCE=.\buildcam.h
# End Source File
# Begin Source File

SOURCE=.\cdll_dll.h
# End Source File
# Begin Source File

SOURCE=.\custmotd.h
# End Source File
# Begin Source File

SOURCE=.\drivecheck.h
# End Source File
# Begin Source File

SOURCE=.\engine.h
# End Source File
# Begin Source File

SOURCE=.\enginecallback.h
# End Source File
# Begin Source File

SOURCE=.\extdll.h
# End Source File
# Begin Source File

SOURCE=.\externmsg.h
# End Source File
# Begin Source File

SOURCE=.\paper.h
# End Source File
# Begin Source File

SOURCE=.\phatmat.h
# End Source File
# Begin Source File

SOURCE=.\player.h
# End Source File
# Begin Source File

SOURCE=.\sadll.h
# End Source File
# Begin Source File

SOURCE=.\smoke.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
