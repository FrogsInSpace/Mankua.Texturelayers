CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT( TextureLayers )

SET ( CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE )

SET ( UNICODE_FLAGS -D_UNICODE )
SET ( DEPRECATION_FLAGS -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE )
SET ( RTTI_FLAGS /GR )

SET ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Lib" )
SET ( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Lib" )
SET ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/Bin" )

# 3 d s   m a x
 
# 3ds Max 2010 | Visual Studio 9.0 | 32 Bits | 64 Bits 
# 3ds Max 2011 | Visual Studio 9.0 | 32 Bits | 64 Bits 
# 3ds Max 2012 | Visual Studio 9.0 | 32 Bits | 64 Bits 
                                                   
# Define the Project Name.
#  Projects that are built with the same IDE (ie. Visual Studio 2005) will be
#  added to the same Solution. For this reason all project need to have 
#  unique names.

SET ( PROJECT_NAME_MAX_2010 "TexLayMax2010" )
SET ( PROJECT_NAME_MAX_2011 "TexLayMax2011" )
SET ( PROJECT_NAME_MAX_2012 "TexLayMax2012" )

# Define the header libraries for each SDK

SET ( MAXSDK_2010_HEADERS_DIR "c:/Libraries/maxsdk/12/include" )
SET ( MAXSDK_2011_HEADERS_DIR "c:/Libraries/maxsdk/13/include" )
SET ( MAXSDK_2012_HEADERS_DIR "c:/Libraries/maxsdk/14/include" )

# Define the library files for each SDK, and platform. 64 bits lib path will replace
# 32 bits lib path.

# 32 BITS 

SET ( MAXSDK10_LIBRARY_DIR "c:/Libraries/maxsdk/10/lib" )
SET ( MAXSDK11_LIBRARY_DIR "c:/Libraries/maxsdk/11/lib" )
SET ( MAXSDK_2010_LIBRARY_DIR "c:/Libraries/maxsdk/12/lib" )
SET ( MAXSDK_2011_LIBRARY_DIR "c:/Libraries/maxsdk/13/lib" )
SET ( MAXSDK_2012_LIBRARY_DIR "c:/Libraries/maxsdk/14/lib" )

SET ( PROGRAM_FILES "c:\\Program Files (x86)" )

IF ( CMAKE_CL_64 ) # 64 BITS

	SET ( MAXSDK10_LIBRARY_DIR "c:/Libraries/maxsdk/10/x64/lib" )
	SET ( MAXSDK11_LIBRARY_DIR "c:/Libraries/maxsdk/11/x64/lib" )
	SET ( MAXSDK_2010_LIBRARY_DIR "c:/Libraries/maxsdk/12/x64/lib" )
	SET ( MAXSDK_2011_LIBRARY_DIR "c:/Libraries/maxsdk/13/x64/lib" )
	SET ( MAXSDK_2012_LIBRARY_DIR "c:/Libraries/maxsdk/14/x64/lib" )
	
	SET ( PROGRAM_FILES "c:\\Program Files" )
	
ENDIF ()

# Add the source files.

SET ( SOURCE_FILES
	"../att.cpp"
	"../CCurve.cpp"
	"../common.cpp"
	"../debug.cpp"
	"../ListBoxHandler.cpp"
	"../Mapmod.cpp"
	"../meshdata.cpp"
	"../MODS.CPP"
	"../natural_box.cpp"
	"../texlay.cpp"
	"../texlay_mc_data.cpp"
	"../tl_apply.cpp"
	"../tl_ui.cpp"
	"../undo.cpp"
	"../uv_pelt_dlg.cpp"
	"../uvw_data.cpp"
	"../uvwmap.cpp"
)

# Add the header files.

SET ( HEADER_FILES
	"../att.h"
	"../CCurve.h"
	"../common.h"
	"../debug.h"
	"../ListBoxHandler.h"
	"../Mapping.h"
	"../meshdata.h"
	"../MODS.H"
	"../modsres.h"
	"../natural_box.h"
	"../texlay.h"
	"../texlay_mc_data.h"
	"../undo.h"
	"../uv_pelt_dlg.h"
	"../uvw_data.h"
	"../uvwgroup.h"
)

# Add the resource and definition file.

SET ( RESOURCE_FILES
	"../mods.rc"
	"../mods.def"
)

# Define the max libraries.

SET( MAX_LIBS  "mnmath.lib"
			   "poly.lib"
			   "paramblk2.lib"
			   "core.lib"
			   "geom.lib"
			   "mesh.lib"
			   "maxutil.lib"
			   "bmm.lib"
			   "maxscrpt.lib"
			   "comctl32.lib"
			   "gfx.lib"
)

IF ( MSVC90 )

	ADD_SUBDIRECTORY( Max2010 )
	ADD_SUBDIRECTORY( Max2011 )
	ADD_SUBDIRECTORY( Max2012 )
	
ENDIF ( MSVC90 )