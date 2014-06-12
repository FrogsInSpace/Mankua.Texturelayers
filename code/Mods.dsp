# Microsoft Developer Studio Project File - Name="mods" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mods - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Mods.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mods.mak" CFG="mods - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mods - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mods - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mods - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mods - Win32 ReleaseR4" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mods - Win32 Release"

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
# ADD CPP /nologo /G6 /MD /W3 /O2 /I "c:\3dsmax3_1\maxsdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 paramblk2.lib maxscrpt.lib bmm.lib core.lib edmodel.lib geom.lib gfx.lib mesh.lib mnmath.lib maxutil.lib paramblk2.lib COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB /nologo /base:"0X28250000" /subsystem:windows /dll /machine:I386 /out:"c:\3dsmax3_1\plugins\TexLay.dlm" /libpath:"c:\3dsmax3_1\maxsdk\lib"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "d:\3dsmax3_1\maxsdk\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS"
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 bmm.lib core.lib edmodel.lib geom.lib gfx.lib mesh.lib mnmath.lib maxutil.lib paramblk2.lib COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB /nologo /base:"0X28250000" /subsystem:windows /dll /debug /machine:I386 /out:"d:\3dsmax3_1\plugins\TexLay.dlm" /libpath:"d:\3DSMAX3_1\MAXSDK\LIB"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\mods___W"
# PROP BASE Intermediate_Dir ".\mods___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Hybrid"
# PROP Intermediate_Dir ".\Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /Gm /GX /Zi /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"mods.h" /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /I "c:\3dsmax3_1\maxsdk\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\PLUGIN\MODS.DLM"
# ADD LINK32 COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB /nologo /base:"0X28250000" /subsystem:windows /dll /debug /machine:I386 /out:"c:\3dsmax3_1\plugins\TexLay.dlm"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mods___Win32_ReleaseR4"
# PROP BASE Intermediate_Dir "mods___Win32_ReleaseR4"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseR4"
# PROP Intermediate_Dir "ReleaseR4"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /O2 /I "d:\3dsmax3_1\maxsdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /FD /c
# SUBTRACT BASE CPP /Fr /YX /Yc /Yu
# ADD CPP /nologo /G6 /MD /W3 /O2 /I "c:\3dsmax4\maxsdk\include" /I "c:\DCPFLICS_SDK\include" /D "DCP_PROTECTED" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 bmm.lib core.lib edmodel.lib geom.lib gfx.lib mesh.lib mnmath.lib maxutil.lib paramblk2.lib COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB /nologo /base:"0X28250000" /subsystem:windows /dll /machine:I386 /out:"d:\3dsmax3_1\plugins\TexLay.dlm" /libpath:"d:\3DSMAX3_1\MAXSDK\LIB"
# ADD LINK32 DCPFLICS.lib maxscrpt.lib mnmath.lib poly.lib bmm.lib core.lib edmodel.lib geom.lib gfx.lib mesh.lib mnmath.lib maxutil.lib paramblk2.lib COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB /nologo /base:"0X28250000" /subsystem:windows /dll /machine:I386 /out:"c:\3dsmax5.1\plugins\TexLay.dlm" /libpath:"c:\3DSMAX4\MAXSDK\LIB" /libpath:"c:\DCPFLICS_SDK\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=c:\DCPFLICS_SDK\Executable\addcrc.exe c:\3dsmax5.1\plugins\TexLay.dlm
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "mods - Win32 Release"
# Name "mods - Win32 Debug"
# Name "mods - Win32 Hybrid"
# Name "mods - Win32 ReleaseR4"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\3d_geom.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\att.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CCurve.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\debug.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ListBoxHandler.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MAPMOD.CPP

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\meshdata.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mods.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

# SUBTRACT CPP /YX /Yc

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

# SUBTRACT CPP /YX /Yc

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

# SUBTRACT BASE CPP /YX /Yc
# SUBTRACT CPP /YX /Yc

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mods.def
# End Source File
# Begin Source File

SOURCE=.\mods.rc
# End Source File
# Begin Source File

SOURCE=.\natural_box.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\texlay.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\texlay_mc_data.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tl_apply.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tl_ui.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\undo.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uv_pelt_dlg.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uvw_data.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uvwmap.cpp

!IF  "$(CFG)" == "mods - Win32 Release"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "mods - Win32 ReleaseR4"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\3d_geom.h
# End Source File
# Begin Source File

SOURCE=.\att.h
# End Source File
# Begin Source File

SOURCE=.\CCurve.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\DFData.h
# End Source File
# Begin Source File

SOURCE=.\ListBoxHandler.h
# End Source File
# Begin Source File

SOURCE=.\MAPPING.H
# End Source File
# Begin Source File

SOURCE=.\meshdata.h
# End Source File
# Begin Source File

SOURCE=.\MODS.H
# End Source File
# Begin Source File

SOURCE=.\natural_box.h
# End Source File
# Begin Source File

SOURCE=.\pblock.h
# End Source File
# Begin Source File

SOURCE=.\SCTEX.H
# End Source File
# Begin Source File

SOURCE=..\texlay.h
# End Source File
# Begin Source File

SOURCE=.\texlay_mc_data.h
# End Source File
# Begin Source File

SOURCE=.\tl_classes.h
# End Source File
# Begin Source File

SOURCE=.\tl_loadcb.h
# End Source File
# Begin Source File

SOURCE=.\undo.h
# End Source File
# Begin Source File

SOURCE=.\uv_pelt_dlg.h
# End Source File
# Begin Source File

SOURCE=.\uvw_data.h
# End Source File
# Begin Source File

SOURCE=.\uvwgroup.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\add.bmp
# End Source File
# Begin Source File

SOURCE=.\addvertc.cur
# End Source File
# Begin Source File

SOURCE=.\attach.cur
# End Source File
# Begin Source File

SOURCE=.\back1.bmp
# End Source File
# Begin Source File

SOURCE=.\back_mas.bmp
# End Source File
# Begin Source File

SOURCE=.\bg_drago.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00004.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00005.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00006.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp16but.bmp
# End Source File
# Begin Source File

SOURCE=.\booleant.bmp
# End Source File
# Begin Source File

SOURCE=.\boolinte.cur
# End Source File
# Begin Source File

SOURCE=.\boolsubt.cur
# End Source File
# Begin Source File

SOURCE=.\boolunio.cur
# End Source File
# Begin Source File

SOURCE=.\boton.bmp
# End Source File
# Begin Source File

SOURCE=.\bulbmask.bmp
# End Source File
# Begin Source File

SOURCE=.\bulbs.bmp
# End Source File
# Begin Source File

SOURCE=.\bulbs_ma.bmp
# End Source File
# Begin Source File

SOURCE=.\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\cur00002.cur
# End Source File
# Begin Source File

SOURCE=.\cur00003.cur
# End Source File
# Begin Source File

SOURCE=.\cur00004.cur
# End Source File
# Begin Source File

SOURCE=.\cur00005.cur
# End Source File
# Begin Source File

SOURCE=.\cur00006.cur
# End Source File
# Begin Source File

SOURCE=.\cur00007.cur
# End Source File
# Begin Source File

SOURCE=.\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\dcp.bmp
# End Source File
# Begin Source File

SOURCE=.\dcp_discreet_header.bmp
# End Source File
# Begin Source File

SOURCE=.\DragonEyes.bmp
# End Source File
# Begin Source File

SOURCE=.\extrudec.cur
# End Source File
# Begin Source File

SOURCE=.\faceselt.bmp
# End Source File
# Begin Source File

SOURCE=.\fox_01_large.bmp
# End Source File
# Begin Source File

SOURCE=.\iconos.bmp
# End Source File
# Begin Source File

SOURCE=.\mankua.bmp
# End Source File
# Begin Source File

SOURCE=.\map_type.bmp
# End Source File
# Begin Source File

SOURCE=.\mask_boo.bmp
# End Source File
# Begin Source File

SOURCE=.\mask_fac.bmp
# End Source File
# Begin Source File

SOURCE=.\mask_unw.bmp
# End Source File
# Begin Source File

SOURCE=.\outline.cur
# End Source File
# Begin Source File

SOURCE=.\ppmlogo.bmp
# End Source File
# Begin Source File

SOURCE=.\ppmlogo2.bmp
# End Source File
# Begin Source File

SOURCE=.\scale_x.cur
# End Source File
# Begin Source File

SOURCE=.\scale_y.cur
# End Source File
# Begin Source File

SOURCE=.\segbreak.cur
# End Source File
# Begin Source File

SOURCE=.\segrefin.cur
# End Source File
# Begin Source File

SOURCE=.\TexLay.bmp
# End Source File
# Begin Source File

SOURCE=.\TexLay2About.bmp
# End Source File
# Begin Source File

SOURCE=.\TexLay2AboutDemo.bmp
# End Source File
# Begin Source File

SOURCE=.\texlay_a.bmp
# End Source File
# Begin Source File

SOURCE=.\thselcur.cur
# End Source File
# Begin Source File

SOURCE=.\Tipos.bmp
# End Source File
# Begin Source File

SOURCE=.\tl2_glenn.bmp
# End Source File
# Begin Source File

SOURCE=.\tl_face.bmp
# End Source File
# Begin Source File

SOURCE=.\type.bmp
# End Source File
# Begin Source File

SOURCE=.\type_mas.bmp
# End Source File
# Begin Source File

SOURCE=.\unwrapto.bmp
# End Source File
# Begin Source File

SOURCE=.\vertconn.cur
# End Source File
# Begin Source File

SOURCE=.\vinsert.cur
# End Source File
# End Group
# Begin Source File

SOURCE=".\TL to do.txt"
# End Source File
# End Target
# End Project
