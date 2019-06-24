# Microsoft Developer Studio Project File - Name="libmbusmaster" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libmbusmaster - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libmbusmaster.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libmbusmaster.mak" CFG="libmbusmaster - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libmbusmaster - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libmbusmaster - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\lib\win\Win32\Release"
# PROP BASE Intermediate_Dir "..\obj\win\Win32\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\win\Win32\Release"
# PROP Intermediate_Dir "..\obj\win\Win32\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /I "..\src\hmlib\common" /W4 /c /Zl /QIfist
# ADD CPP /nologo /MT /I "..\src\hmlib\common" /W4 /c /Zl /QIfist
# ADD BASE MTL /nologo /win32 
# ADD MTL /nologo /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo 
# ADD LIB32 /nologo /out:"..\lib\win\Win32\Release\libmbusmaster.lib"

!ENDIF

# Begin Target

# Name "libmbusmaster - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;def;odl;idl;hpj;bat;asm;asmx"
# Begin Source File

SOURCE=..\src\BusProtocolErrors.c
# End Source File
# Begin Source File

SOURCE=..\src\hmlib\common\hmcrc16.c
# End Source File
# Begin Source File

SOURCE=..\src\hmlib\common\hmhex.c
# End Source File
# Begin Source File

SOURCE=..\src\hmlib\win32\hmserio.cpp
# End Source File
# Begin Source File

SOURCE=..\src\hmlib\win32\hmtimer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MbusAsciiMasterProtocol.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MbusMasterFunctions.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MbusRtuMasterProtocol.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MbusMasterCexports.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MbusRtuOverTcpMasterProtocol.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MbusSerialMasterProtocol.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MbusTcpMasterProtocol.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc;xsd"
# Begin Source File

SOURCE=..\src\BusProtocolErrors.h
# End Source File
# Begin Source File

SOURCE=..\src\FieldTalkVersion.h
# End Source File
# Begin Source File

SOURCE=..\src\hmlib\common\hmcrc16.h
# End Source File
# Begin Source File

SOURCE=..\src\hmlib\common\hmhex.h
# End Source File
# Begin Source File

SOURCE=..\src\hmlib\common\hmplatf.h
# End Source File
# Begin Source File

SOURCE=..\src\hmlib\common\hmserio.hpp
# End Source File
# Begin Source File

SOURCE=..\src\hmlib\common\hmtcpip.h
# End Source File
# Begin Source File

SOURCE=..\src\hmlib\common\hmtimer.hpp
# End Source File
# Begin Source File

SOURCE=..\src\hmlib\common\hmtracelog.h
# End Source File
# Begin Source File

SOURCE=..\src\hmlib\common\hmtypes.h
# End Source File
# Begin Source File

SOURCE=..\src\libmbusmaster.h
# End Source File
# Begin Source File

SOURCE=..\src\MbusAsciiMasterProtocol.hpp
# End Source File
# Begin Source File

SOURCE=..\src\MbusMasterFunctions.hpp
# End Source File
# Begin Source File

SOURCE=..\src\MbusRtuMasterProtocol.hpp
# End Source File
# Begin Source File

SOURCE=..\src\MbusSerialMasterProtocol.hpp
# End Source File
# Begin Source File

SOURCE=..\src\MbusTcpMasterProtocol.hpp
# End Source File
# End Group
# End Target
# End Project

