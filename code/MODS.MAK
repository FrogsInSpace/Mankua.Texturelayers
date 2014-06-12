# Microsoft Developer Studio Generated NMAKE File, Based on Mods.dsp
!IF "$(CFG)" == ""
CFG=mods - Win32 Release
!MESSAGE No configuration specified. Defaulting to mods - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "mods - Win32 Release" && "$(CFG)" != "mods - Win32 Debug" &&\
 "$(CFG)" != "mods - Win32 Hybrid"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mods - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\PLUGIN\MODS.DLM"

!ELSE 

ALL : "..\..\PLUGIN\MODS.DLM"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\afregion.obj"
	-@erase "$(INTDIR)\bend.obj"
	-@erase "$(INTDIR)\bomb.obj"
	-@erase "$(INTDIR)\clstnode.obj"
	-@erase "$(INTDIR)\clustmod.obj"
	-@erase "$(INTDIR)\deflect.obj"
	-@erase "$(INTDIR)\delmod.obj"
	-@erase "$(INTDIR)\delsmod.obj"
	-@erase "$(INTDIR)\dispmod.obj"
	-@erase "$(INTDIR)\editmesh.obj"
	-@erase "$(INTDIR)\editmops.obj"
	-@erase "$(INTDIR)\editpat.obj"
	-@erase "$(INTDIR)\editpops.obj"
	-@erase "$(INTDIR)\editsops.obj"
	-@erase "$(INTDIR)\editspl.obj"
	-@erase "$(INTDIR)\edmdata.obj"
	-@erase "$(INTDIR)\edmrest.obj"
	-@erase "$(INTDIR)\edmui.obj"
	-@erase "$(INTDIR)\extrude.obj"
	-@erase "$(INTDIR)\fextrude.obj"
	-@erase "$(INTDIR)\gravity.obj"
	-@erase "$(INTDIR)\mapmod.obj"
	-@erase "$(INTDIR)\meshsel.obj"
	-@erase "$(INTDIR)\mirror.obj"
	-@erase "$(INTDIR)\mods.obj"
	-@erase "$(INTDIR)\Mods.pch"
	-@erase "$(INTDIR)\mods.res"
	-@erase "$(INTDIR)\noisemod.obj"
	-@erase "$(INTDIR)\optmod.obj"
	-@erase "$(INTDIR)\resettm.obj"
	-@erase "$(INTDIR)\sctex.obj"
	-@erase "$(INTDIR)\selmod.obj"
	-@erase "$(INTDIR)\sinwave.obj"
	-@erase "$(INTDIR)\skew.obj"
	-@erase "$(INTDIR)\surfmod.obj"
	-@erase "$(INTDIR)\surfrev.obj"
	-@erase "$(INTDIR)\taper.obj"
	-@erase "$(INTDIR)\tessmod.obj"
	-@erase "$(INTDIR)\twist1.obj"
	-@erase "$(INTDIR)\twist2.obj"
	-@erase "$(INTDIR)\unwrap.obj"
	-@erase "$(INTDIR)\uvwxform.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\mods.exp"
	-@erase "$(OUTDIR)\mods.lib"
	-@ERASE "..\..\PLUGIN\MODS.DLM"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /Fp"$(INTDIR)\Mods.pch" /Yu"mods.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mods.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Mods.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /base:"0x28250000" /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\mods.pdb" /machine:I386 /def:".\mods.def"\
 /OUT:"..\..\PLUGIN\MODS.DLM" /IMPLIB:"$(OUTDIR)\MODS.LIB" 
DEF_FILE= \
	".\mods.def"
LINK32_OBJS= \
	"$(INTDIR)\afregion.obj" \
	"$(INTDIR)\bend.obj" \
	"$(INTDIR)\bomb.obj" \
	"$(INTDIR)\clstnode.obj" \
	"$(INTDIR)\clustmod.obj" \
	"$(INTDIR)\deflect.obj" \
	"$(INTDIR)\delmod.obj" \
	"$(INTDIR)\delsmod.obj" \
	"$(INTDIR)\dispmod.obj" \
	"$(INTDIR)\editmesh.obj" \
	"$(INTDIR)\editmops.obj" \
	"$(INTDIR)\editpat.obj" \
	"$(INTDIR)\editpops.obj" \
	"$(INTDIR)\editsops.obj" \
	"$(INTDIR)\editspl.obj" \
	"$(INTDIR)\edmdata.obj" \
	"$(INTDIR)\edmrest.obj" \
	"$(INTDIR)\edmui.obj" \
	"$(INTDIR)\extrude.obj" \
	"$(INTDIR)\fextrude.obj" \
	"$(INTDIR)\gravity.obj" \
	"$(INTDIR)\mapmod.obj" \
	"$(INTDIR)\meshsel.obj" \
	"$(INTDIR)\mirror.obj" \
	"$(INTDIR)\mods.obj" \
	"$(INTDIR)\mods.res" \
	"$(INTDIR)\noisemod.obj" \
	"$(INTDIR)\optmod.obj" \
	"$(INTDIR)\resettm.obj" \
	"$(INTDIR)\sctex.obj" \
	"$(INTDIR)\selmod.obj" \
	"$(INTDIR)\sinwave.obj" \
	"$(INTDIR)\skew.obj" \
	"$(INTDIR)\surfmod.obj" \
	"$(INTDIR)\surfrev.obj" \
	"$(INTDIR)\taper.obj" \
	"$(INTDIR)\tessmod.obj" \
	"$(INTDIR)\twist1.obj" \
	"$(INTDIR)\twist2.obj" \
	"$(INTDIR)\unwrap.obj" \
	"$(INTDIR)\uvwxform.obj" \
	"\MAXSDK\LIB\BMM.LIB" \
	"\MAXSDK\LIB\CORE.LIB" \
	"\MAXSDK\LIB\EDMODEL.LIB" \
	"\MAXSDK\LIB\GEOM.LIB" \
	"\MAXSDK\LIB\GFX.LIB" \
	"\MAXSDK\LIB\MESH.LIB" \
	"\MAXSDK\LIB\MNMATH.LIB" \
	"\MAXSDK\LIB\PATCH.LIB" \
	"\MAXSDK\LIB\UTIL.LIB"

"..\..\PLUGIN\MODS.DLM" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\PLUGIN\MODS.DLM"

!ELSE 

ALL : "..\..\PLUGIN\MODS.DLM"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\afregion.obj"
	-@erase "$(INTDIR)\bend.obj"
	-@erase "$(INTDIR)\bomb.obj"
	-@erase "$(INTDIR)\clstnode.obj"
	-@erase "$(INTDIR)\clustmod.obj"
	-@erase "$(INTDIR)\deflect.obj"
	-@erase "$(INTDIR)\delmod.obj"
	-@erase "$(INTDIR)\delsmod.obj"
	-@erase "$(INTDIR)\dispmod.obj"
	-@erase "$(INTDIR)\editmesh.obj"
	-@erase "$(INTDIR)\editmops.obj"
	-@erase "$(INTDIR)\editpat.obj"
	-@erase "$(INTDIR)\editpops.obj"
	-@erase "$(INTDIR)\editsops.obj"
	-@erase "$(INTDIR)\editspl.obj"
	-@erase "$(INTDIR)\edmdata.obj"
	-@erase "$(INTDIR)\edmrest.obj"
	-@erase "$(INTDIR)\edmui.obj"
	-@erase "$(INTDIR)\extrude.obj"
	-@erase "$(INTDIR)\fextrude.obj"
	-@erase "$(INTDIR)\gravity.obj"
	-@erase "$(INTDIR)\mapmod.obj"
	-@erase "$(INTDIR)\meshsel.obj"
	-@erase "$(INTDIR)\mirror.obj"
	-@erase "$(INTDIR)\mods.obj"
	-@erase "$(INTDIR)\Mods.pch"
	-@erase "$(INTDIR)\mods.res"
	-@erase "$(INTDIR)\noisemod.obj"
	-@erase "$(INTDIR)\optmod.obj"
	-@erase "$(INTDIR)\resettm.obj"
	-@erase "$(INTDIR)\sctex.obj"
	-@erase "$(INTDIR)\selmod.obj"
	-@erase "$(INTDIR)\sinwave.obj"
	-@erase "$(INTDIR)\skew.obj"
	-@erase "$(INTDIR)\surfmod.obj"
	-@erase "$(INTDIR)\surfrev.obj"
	-@erase "$(INTDIR)\taper.obj"
	-@erase "$(INTDIR)\tessmod.obj"
	-@erase "$(INTDIR)\twist1.obj"
	-@erase "$(INTDIR)\twist2.obj"
	-@erase "$(INTDIR)\unwrap.obj"
	-@erase "$(INTDIR)\uvwxform.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\mods.exp"
	-@erase "$(OUTDIR)\mods.lib"
	-@erase "$(OUTDIR)\mods.pdb"
	-@ERASE "..\..\PLUGIN\MODS.DLM"
	-@ERASE "..\..\PLUGIN\MODS.ILK"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /Zi /Od /I "..\..\include" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mods.pch" /Yu"mods.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mods.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Mods.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /base:"0x28250000" /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\mods.pdb" /debug /machine:I386 /def:".\mods.def"\
 /OUT:"..\..\PLUGIN\MODS.DLM" /IMPLIB:"$(OUTDIR)\MODS.LIB" 
DEF_FILE= \
	".\mods.def"
LINK32_OBJS= \
	"$(INTDIR)\afregion.obj" \
	"$(INTDIR)\bend.obj" \
	"$(INTDIR)\bomb.obj" \
	"$(INTDIR)\clstnode.obj" \
	"$(INTDIR)\clustmod.obj" \
	"$(INTDIR)\deflect.obj" \
	"$(INTDIR)\delmod.obj" \
	"$(INTDIR)\delsmod.obj" \
	"$(INTDIR)\dispmod.obj" \
	"$(INTDIR)\editmesh.obj" \
	"$(INTDIR)\editmops.obj" \
	"$(INTDIR)\editpat.obj" \
	"$(INTDIR)\editpops.obj" \
	"$(INTDIR)\editsops.obj" \
	"$(INTDIR)\editspl.obj" \
	"$(INTDIR)\edmdata.obj" \
	"$(INTDIR)\edmrest.obj" \
	"$(INTDIR)\edmui.obj" \
	"$(INTDIR)\extrude.obj" \
	"$(INTDIR)\fextrude.obj" \
	"$(INTDIR)\gravity.obj" \
	"$(INTDIR)\mapmod.obj" \
	"$(INTDIR)\meshsel.obj" \
	"$(INTDIR)\mirror.obj" \
	"$(INTDIR)\mods.obj" \
	"$(INTDIR)\mods.res" \
	"$(INTDIR)\noisemod.obj" \
	"$(INTDIR)\optmod.obj" \
	"$(INTDIR)\resettm.obj" \
	"$(INTDIR)\sctex.obj" \
	"$(INTDIR)\selmod.obj" \
	"$(INTDIR)\sinwave.obj" \
	"$(INTDIR)\skew.obj" \
	"$(INTDIR)\surfmod.obj" \
	"$(INTDIR)\surfrev.obj" \
	"$(INTDIR)\taper.obj" \
	"$(INTDIR)\tessmod.obj" \
	"$(INTDIR)\twist1.obj" \
	"$(INTDIR)\twist2.obj" \
	"$(INTDIR)\unwrap.obj" \
	"$(INTDIR)\uvwxform.obj" \
	"\MAXSDK\LIB\BMM.LIB" \
	"\MAXSDK\LIB\CORE.LIB" \
	"\MAXSDK\LIB\EDMODEL.LIB" \
	"\MAXSDK\LIB\GEOM.LIB" \
	"\MAXSDK\LIB\GFX.LIB" \
	"\MAXSDK\LIB\MESH.LIB" \
	"\MAXSDK\LIB\MNMATH.LIB" \
	"\MAXSDK\LIB\PATCH.LIB" \
	"\MAXSDK\LIB\UTIL.LIB"

"..\..\PLUGIN\MODS.DLM" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

OUTDIR=.\Hybrid
INTDIR=.\Hybrid

!IF "$(RECURSE)" == "0" 

ALL : "..\..\PLUGIN\MODS.DLM"

!ELSE 

ALL : "..\..\PLUGIN\MODS.DLM"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\afregion.obj"
	-@erase "$(INTDIR)\bend.obj"
	-@erase "$(INTDIR)\bomb.obj"
	-@erase "$(INTDIR)\clstnode.obj"
	-@erase "$(INTDIR)\clustmod.obj"
	-@erase "$(INTDIR)\deflect.obj"
	-@erase "$(INTDIR)\delmod.obj"
	-@erase "$(INTDIR)\delsmod.obj"
	-@erase "$(INTDIR)\dispmod.obj"
	-@erase "$(INTDIR)\editmesh.obj"
	-@erase "$(INTDIR)\editmops.obj"
	-@erase "$(INTDIR)\editpat.obj"
	-@erase "$(INTDIR)\editpops.obj"
	-@erase "$(INTDIR)\editsops.obj"
	-@erase "$(INTDIR)\editspl.obj"
	-@erase "$(INTDIR)\edmdata.obj"
	-@erase "$(INTDIR)\edmrest.obj"
	-@erase "$(INTDIR)\edmui.obj"
	-@erase "$(INTDIR)\extrude.obj"
	-@erase "$(INTDIR)\fextrude.obj"
	-@erase "$(INTDIR)\gravity.obj"
	-@erase "$(INTDIR)\mapmod.obj"
	-@erase "$(INTDIR)\meshsel.obj"
	-@erase "$(INTDIR)\mirror.obj"
	-@erase "$(INTDIR)\mods.obj"
	-@erase "$(INTDIR)\Mods.pch"
	-@erase "$(INTDIR)\mods.res"
	-@erase "$(INTDIR)\noisemod.obj"
	-@erase "$(INTDIR)\optmod.obj"
	-@erase "$(INTDIR)\resettm.obj"
	-@erase "$(INTDIR)\sctex.obj"
	-@erase "$(INTDIR)\selmod.obj"
	-@erase "$(INTDIR)\sinwave.obj"
	-@erase "$(INTDIR)\skew.obj"
	-@erase "$(INTDIR)\surfmod.obj"
	-@erase "$(INTDIR)\surfrev.obj"
	-@erase "$(INTDIR)\taper.obj"
	-@erase "$(INTDIR)\tessmod.obj"
	-@erase "$(INTDIR)\twist1.obj"
	-@erase "$(INTDIR)\twist2.obj"
	-@erase "$(INTDIR)\unwrap.obj"
	-@erase "$(INTDIR)\uvwxform.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\mods.exp"
	-@erase "$(OUTDIR)\mods.lib"
	-@erase "$(OUTDIR)\mods.pdb"
	-@ERASE "..\..\PLUGIN\MODS.DLM"
	-@ERASE "..\..\PLUGIN\MODS.ILK"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MD /W3 /Gm /Zi /Od /I "..\..\include" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mods.pch" /Yu"mods.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Hybrid/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mods.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Mods.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /base:"0x28250000" /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\mods.pdb" /debug /machine:I386 /def:".\mods.def"\
 /OUT:"..\..\PLUGIN\MODS.DLM" /IMPLIB:"$(OUTDIR)\MODS.LIB" 
DEF_FILE= \
	".\mods.def"
LINK32_OBJS= \
	"$(INTDIR)\afregion.obj" \
	"$(INTDIR)\bend.obj" \
	"$(INTDIR)\bomb.obj" \
	"$(INTDIR)\clstnode.obj" \
	"$(INTDIR)\clustmod.obj" \
	"$(INTDIR)\deflect.obj" \
	"$(INTDIR)\delmod.obj" \
	"$(INTDIR)\delsmod.obj" \
	"$(INTDIR)\dispmod.obj" \
	"$(INTDIR)\editmesh.obj" \
	"$(INTDIR)\editmops.obj" \
	"$(INTDIR)\editpat.obj" \
	"$(INTDIR)\editpops.obj" \
	"$(INTDIR)\editsops.obj" \
	"$(INTDIR)\editspl.obj" \
	"$(INTDIR)\edmdata.obj" \
	"$(INTDIR)\edmrest.obj" \
	"$(INTDIR)\edmui.obj" \
	"$(INTDIR)\extrude.obj" \
	"$(INTDIR)\fextrude.obj" \
	"$(INTDIR)\gravity.obj" \
	"$(INTDIR)\mapmod.obj" \
	"$(INTDIR)\meshsel.obj" \
	"$(INTDIR)\mirror.obj" \
	"$(INTDIR)\mods.obj" \
	"$(INTDIR)\mods.res" \
	"$(INTDIR)\noisemod.obj" \
	"$(INTDIR)\optmod.obj" \
	"$(INTDIR)\resettm.obj" \
	"$(INTDIR)\sctex.obj" \
	"$(INTDIR)\selmod.obj" \
	"$(INTDIR)\sinwave.obj" \
	"$(INTDIR)\skew.obj" \
	"$(INTDIR)\surfmod.obj" \
	"$(INTDIR)\surfrev.obj" \
	"$(INTDIR)\taper.obj" \
	"$(INTDIR)\tessmod.obj" \
	"$(INTDIR)\twist1.obj" \
	"$(INTDIR)\twist2.obj" \
	"$(INTDIR)\unwrap.obj" \
	"$(INTDIR)\uvwxform.obj" \
	"\MAXSDK\LIB\BMM.LIB" \
	"\MAXSDK\LIB\CORE.LIB" \
	"\MAXSDK\LIB\EDMODEL.LIB" \
	"\MAXSDK\LIB\GEOM.LIB" \
	"\MAXSDK\LIB\GFX.LIB" \
	"\MAXSDK\LIB\MESH.LIB" \
	"\MAXSDK\LIB\MNMATH.LIB" \
	"\MAXSDK\LIB\PATCH.LIB" \
	"\MAXSDK\LIB\UTIL.LIB"

"..\..\PLUGIN\MODS.DLM" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "mods - Win32 Release" || "$(CFG)" == "mods - Win32 Debug" ||\
 "$(CFG)" == "mods - Win32 Hybrid"
SOURCE=.\afregion.cpp
DEP_CPP_AFREG=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\afregion.obj" : $(SOURCE) $(DEP_CPP_AFREG) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\bend.cpp
DEP_CPP_BEND_=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\partclib.h"\
	"..\..\include\particle.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\simpmod.h"\
	"..\..\include\simpobj.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\bend.obj" : $(SOURCE) $(DEP_CPP_BEND_) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\bomb.cpp
DEP_CPP_BOMB_=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\bomb.obj" : $(SOURCE) $(DEP_CPP_BOMB_) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\clstnode.cpp
DEP_CPP_CLSTN=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\clstnode.obj" : $(SOURCE) $(DEP_CPP_CLSTN) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\clustmod.cpp
DEP_CPP_CLUST=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\simpmod.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\clustmod.obj" : $(SOURCE) $(DEP_CPP_CLUST) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\deflect.cpp
DEP_CPP_DEFLE=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\partclib.h"\
	"..\..\include\particle.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\simpmod.h"\
	"..\..\include\simpobj.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\texutil.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\deflect.obj" : $(SOURCE) $(DEP_CPP_DEFLE) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\delmod.cpp
DEP_CPP_DELMO=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\delmod.obj" : $(SOURCE) $(DEP_CPP_DELMO) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\delsmod.cpp
DEP_CPP_DELSM=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\captypes.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mstshape.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\polyshp.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\shape.h"\
	"..\..\include\shphier.h"\
	"..\..\include\shpsels.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\spline3d.h"\
	"..\..\include\splshape.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\delsmod.obj" : $(SOURCE) $(DEP_CPP_DELSM) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\dispmod.cpp
DEP_CPP_DISPM=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\bitmap.h"\
	"..\..\include\bmmlib.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linklist.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\palutil.h"\
	"..\..\include\partclib.h"\
	"..\..\include\particle.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\simpmod.h"\
	"..\..\include\simpobj.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\texutil.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MAPPING.H"\
	".\MODS.H"\
	".\SCTEX.H"\
	

"$(INTDIR)\dispmod.obj" : $(SOURCE) $(DEP_CPP_DISPM) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\editmesh.cpp
DEP_CPP_EDITM=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\EDITMESH.H"\
	".\EDMDATA.H"\
	".\MODS.H"\
	".\NAMESEL.H"\
	

"$(INTDIR)\editmesh.obj" : $(SOURCE) $(DEP_CPP_EDITM) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\editmops.cpp
DEP_CPP_EDITMO=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\decomp.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\EDITMESH.H"\
	".\EDMDATA.H"\
	".\MODS.H"\
	".\NAMESEL.H"\
	

"$(INTDIR)\editmops.obj" : $(SOURCE) $(DEP_CPP_EDITMO) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\editpat.cpp
DEP_CPP_EDITP=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\decomp.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\EDITPAT.H"\
	".\MODS.H"\
	".\NAMESEL.H"\
	

"$(INTDIR)\editpat.obj" : $(SOURCE) $(DEP_CPP_EDITP) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\editpops.cpp
DEP_CPP_EDITPO=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\EDITPAT.H"\
	".\MODS.H"\
	".\NAMESEL.H"\
	

"$(INTDIR)\editpops.obj" : $(SOURCE) $(DEP_CPP_EDITPO) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\editsops.cpp
DEP_CPP_EDITS=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\captypes.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evrouter.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mstshape.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\polyshp.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\shape.h"\
	"..\..\include\shphier.h"\
	"..\..\include\shpsels.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\spline3d.h"\
	"..\..\include\splshape.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\EDITSPL.H"\
	".\MODS.H"\
	

"$(INTDIR)\editsops.obj" : $(SOURCE) $(DEP_CPP_EDITS) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\editspl.cpp
DEP_CPP_EDITSP=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\captypes.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mstshape.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\polyshp.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\shape.h"\
	"..\..\include\shphier.h"\
	"..\..\include\shpsels.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\spline3d.h"\
	"..\..\include\splshape.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\EDITSPL.H"\
	".\MODS.H"\
	

"$(INTDIR)\editspl.obj" : $(SOURCE) $(DEP_CPP_EDITSP) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\edmdata.cpp
DEP_CPP_EDMDA=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\EDITMESH.H"\
	".\EDMDATA.H"\
	".\MODS.H"\
	".\NAMESEL.H"\
	

"$(INTDIR)\edmdata.obj" : $(SOURCE) $(DEP_CPP_EDMDA) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\edmrest.cpp
DEP_CPP_EDMRE=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\EDITMESH.H"\
	".\EDMDATA.H"\
	".\MODS.H"\
	".\NAMESEL.H"\
	

"$(INTDIR)\edmrest.obj" : $(SOURCE) $(DEP_CPP_EDMRE) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\edmui.cpp
DEP_CPP_EDMUI=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\EDITMESH.H"\
	".\EDMDATA.H"\
	".\MODS.H"\
	".\NAMESEL.H"\
	

"$(INTDIR)\edmui.obj" : $(SOURCE) $(DEP_CPP_EDMUI) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\extrude.cpp
DEP_CPP_EXTRU=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\captypes.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linshape.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mstshape.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\polyshp.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\shape.h"\
	"..\..\include\shphier.h"\
	"..\..\include\shpsels.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\spline3d.h"\
	"..\..\include\splshape.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\surf_api.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\extrude.obj" : $(SOURCE) $(DEP_CPP_EXTRU) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\fextrude.cpp
DEP_CPP_FEXTR=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshadj.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\fextrude.obj" : $(SOURCE) $(DEP_CPP_FEXTR) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\gravity.cpp
DEP_CPP_GRAVI=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\partclib.h"\
	"..\..\include\particle.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\simpmod.h"\
	"..\..\include\simpobj.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\texutil.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\gravity.obj" : $(SOURCE) $(DEP_CPP_GRAVI) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\mapmod.cpp
DEP_CPP_MAPMO=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\bitmap.h"\
	"..\..\include\bmmlib.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\decomp.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linklist.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\palutil.h"\
	"..\..\include\partclib.h"\
	"..\..\include\particle.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\simpmod.h"\
	"..\..\include\simpobj.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\texutil.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MAPPING.H"\
	".\MODS.H"\
	

"$(INTDIR)\mapmod.obj" : $(SOURCE) $(DEP_CPP_MAPMO) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\meshsel.cpp
DEP_CPP_MESHS=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\istdplug.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshadj.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\nsclip.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\shpsels.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	".\NAMESEL.H"\
	

"$(INTDIR)\meshsel.obj" : $(SOURCE) $(DEP_CPP_MESHS) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\mirror.cpp
DEP_CPP_MIRRO=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\mirror.obj" : $(SOURCE) $(DEP_CPP_MIRRO) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\mods.cpp
DEP_CPP_MODS_=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

!IF  "$(CFG)" == "mods - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32"\
 /D "_WINDOWS" /Fp"$(INTDIR)\Mods.pch" /Yc"mods.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mods.obj"	"$(INTDIR)\Mods.pch" : $(SOURCE) $(DEP_CPP_MODS_)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /Zi /Od /I "..\..\include" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mods.pch" /Yc"mods.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mods.obj"	"$(INTDIR)\Mods.pch" : $(SOURCE) $(DEP_CPP_MODS_)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

CPP_SWITCHES=/nologo /G6 /MD /W3 /Gm /Zi /Od /I "..\..\include" /D "_DEBUG" /D\
 "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\Mods.pch" /Yc"mods.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mods.obj"	"$(INTDIR)\Mods.pch" : $(SOURCE) $(DEP_CPP_MODS_)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\mods.rc
DEP_RSC_MODS_R=\
	".\addvertc.cur"\
	".\attach.cur"\
	".\bmp00001.bmp"\
	".\bmp00002.bmp"\
	".\booleant.bmp"\
	".\boolinte.cur"\
	".\boolsubt.cur"\
	".\boolunio.cur"\
	".\bulbmask.bmp"\
	".\bulbs.bmp"\
	".\extrudec.cur"\
	".\faceselt.bmp"\
	".\mask_boo.bmp"\
	".\mask_fac.bmp"\
	".\mask_unw.bmp"\
	".\outline.cur"\
	".\segbreak.cur"\
	".\segrefin.cur"\
	".\thselcur.cur"\
	".\unwrapto.bmp"\
	".\vertconn.cur"\
	".\vinsert.cur"\
	

"$(INTDIR)\mods.res" : $(SOURCE) $(DEP_RSC_MODS_R) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\noisemod.cpp
DEP_CPP_NOISE=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\partclib.h"\
	"..\..\include\particle.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\simpmod.h"\
	"..\..\include\simpobj.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\texutil.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\noisemod.obj" : $(SOURCE) $(DEP_CPP_NOISE) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\optmod.cpp
DEP_CPP_OPTMO=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\optmod.obj" : $(SOURCE) $(DEP_CPP_OPTMO) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\resettm.cpp
DEP_CPP_RESET=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\istdplug.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\modstack.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\simpmod.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilapi.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\resettm.obj" : $(SOURCE) $(DEP_CPP_RESET) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\sctex.cpp
DEP_CPP_SCTEX=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gamma.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	".\SCTEX.H"\
	

"$(INTDIR)\sctex.obj" : $(SOURCE) $(DEP_CPP_SCTEX) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\selmod.cpp
DEP_CPP_SELMO=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\selmod.obj" : $(SOURCE) $(DEP_CPP_SELMO) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\sinwave.cpp
DEP_CPP_SINWA=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\partclib.h"\
	"..\..\include\particle.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\simpmod.h"\
	"..\..\include\simpobj.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\texutil.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\sinwave.obj" : $(SOURCE) $(DEP_CPP_SINWA) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\skew.cpp
DEP_CPP_SKEW_=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\partclib.h"\
	"..\..\include\particle.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\simpmod.h"\
	"..\..\include\simpobj.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\skew.obj" : $(SOURCE) $(DEP_CPP_SKEW_) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\surfmod.cpp
DEP_CPP_SURFM=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\EDMDATA.H"\
	".\MODS.H"\
	

"$(INTDIR)\surfmod.obj" : $(SOURCE) $(DEP_CPP_SURFM) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\surfrev.cpp
DEP_CPP_SURFR=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\captypes.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\linshape.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mstshape.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\polyshp.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\shape.h"\
	"..\..\include\shphier.h"\
	"..\..\include\shpsels.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\spline3d.h"\
	"..\..\include\splshape.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\surf_api.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\surfrev.obj" : $(SOURCE) $(DEP_CPP_SURFR) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\taper.cpp
DEP_CPP_TAPER=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\partclib.h"\
	"..\..\include\particle.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\simpmod.h"\
	"..\..\include\simpobj.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\taper.obj" : $(SOURCE) $(DEP_CPP_TAPER) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\tessmod.cpp
DEP_CPP_TESSM=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\MNBigMat.h"\
	"..\..\include\MNCommon.h"\
	"..\..\include\MNMath.h"\
	"..\..\include\MNMesh.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\tessmod.obj" : $(SOURCE) $(DEP_CPP_TESSM) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\twist1.cpp
DEP_CPP_TWIST=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\partclib.h"\
	"..\..\include\particle.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\simpmod.h"\
	"..\..\include\simpobj.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	".\TWIST.CPP"\
	

"$(INTDIR)\twist1.obj" : $(SOURCE) $(DEP_CPP_TWIST) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\twist2.cpp
DEP_CPP_TWIST2=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\twist2.obj" : $(SOURCE) $(DEP_CPP_TWIST2) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\unwrap.cpp
DEP_CPP_UNWRA=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gport.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshadj.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	".\SCTEX.H"\
	

"$(INTDIR)\unwrap.obj" : $(SOURCE) $(DEP_CPP_UNWRA) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\uvwxform.cpp
DEP_CPP_UVWXF=\
	"..\..\include\acolor.h"\
	"..\..\include\animtbl.h"\
	"..\..\include\appio.h"\
	"..\..\include\assert1.h"\
	"..\..\include\bitarray.h"\
	"..\..\include\box2.h"\
	"..\..\include\box3.h"\
	"..\..\include\buildver.h"\
	"..\..\include\channels.h"\
	"..\..\include\cmdmode.h"\
	"..\..\include\color.h"\
	"..\..\include\control.h"\
	"..\..\include\coreexp.h"\
	"..\..\include\custcont.h"\
	"..\..\include\dbgprint.h"\
	"..\..\include\dpoint3.h"\
	"..\..\include\euler.h"\
	"..\..\include\evuser.h"\
	"..\..\include\export.h"\
	"..\..\include\gencam.h"\
	"..\..\include\genhier.h"\
	"..\..\include\genlight.h"\
	"..\..\include\genshape.h"\
	"..\..\include\geom.h"\
	"..\..\include\geomlib.h"\
	"..\..\include\gfloat.h"\
	"..\..\include\gfx.h"\
	"..\..\include\gfxlib.h"\
	"..\..\include\gutil.h"\
	"..\..\include\hitdata.h"\
	"..\..\include\hold.h"\
	"..\..\include\impapi.h"\
	"..\..\include\impexp.h"\
	"..\..\include\imtl.h"\
	"..\..\include\inode.h"\
	"..\..\include\interval.h"\
	"..\..\include\ioapi.h"\
	"..\..\include\iparamb.h"\
	"..\..\include\iparamm.h"\
	"..\..\include\ipoint2.h"\
	"..\..\include\ipoint3.h"\
	"..\..\include\lockid.h"\
	"..\..\include\log.h"\
	"..\..\include\matrix2.h"\
	"..\..\include\matrix3.h"\
	"..\..\include\Max.h"\
	"..\..\include\maxapi.h"\
	"..\..\include\maxcom.h"\
	"..\..\include\maxtess.h"\
	"..\..\include\maxtypes.h"\
	"..\..\include\mesh.h"\
	"..\..\include\meshlib.h"\
	"..\..\include\mouseman.h"\
	"..\..\include\mtl.h"\
	"..\..\include\nametab.h"\
	"..\..\include\object.h"\
	"..\..\include\objmode.h"\
	"..\..\include\patch.h"\
	"..\..\include\patchlib.h"\
	"..\..\include\patchobj.h"\
	"..\..\include\plugapi.h"\
	"..\..\include\plugin.h"\
	"..\..\include\point2.h"\
	"..\..\include\point3.h"\
	"..\..\include\point4.h"\
	"..\..\include\ptrvec.h"\
	"..\..\include\quat.h"\
	"..\..\include\ref.h"\
	"..\..\include\render.h"\
	"..\..\include\rtclick.h"\
	"..\..\include\sceneapi.h"\
	"..\..\include\snap.h"\
	"..\..\include\soundobj.h"\
	"..\..\include\stack.h"\
	"..\..\include\stack3.h"\
	"..\..\include\strbasic.h"\
	"..\..\include\strclass.h"\
	"..\..\include\tab.h"\
	"..\..\include\trig.h"\
	"..\..\include\triobj.h"\
	"..\..\include\units.h"\
	"..\..\include\utilexp.h"\
	"..\..\include\utillib.h"\
	"..\..\include\vedge.h"\
	"..\..\include\winutil.h"\
	".\MODS.H"\
	

"$(INTDIR)\uvwxform.obj" : $(SOURCE) $(DEP_CPP_UVWXF) "$(INTDIR)"\
 "$(INTDIR)\Mods.pch"


SOURCE=.\TWIST.CPP

!ENDIF 

