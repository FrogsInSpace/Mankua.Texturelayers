#ifndef __PBLOCK__
#define __PBLOCK__

#include "mapping.h"
#include "texlay_mc_data.h"

// Parameters

static int axisIDs[]   = {IDC_MM_X,IDC_MM_Y,IDC_MM_Z};
//static int normsIDs[]  = {IDC_SM_NMPLN,IDC_SM_NMCEN,IDC_SM_NMFLD};
static int normsIDs[]  = {IDC_SM_PL,IDC_SM_CEN,IDC_SM_FL};

static ParamUIDesc descParam[] = {

	// 0
	// U Tiling
	ParamUIDesc(
		PB_US,
		EDITTYPE_FLOAT,
		IDC_MM_US,IDC_MM_USSPIN,
		-999999999.0f,999999999.0f,
		0.01f),

	// 1
	// U Offset
	ParamUIDesc(
		PB_UE,
		EDITTYPE_FLOAT,
		IDC_MM_UE,IDC_MM_UESPIN,
		-999999999.0f,999999999.0f,
		0.01f),

	// 2
	// V Tiling
	ParamUIDesc(
		PB_VS,
		EDITTYPE_FLOAT,
		IDC_MM_VS,IDC_MM_VSSPIN,
		-999999999.0f,999999999.0f,
		0.01f),

	// 3
	// V Offset
	ParamUIDesc(
		PB_VE,
		EDITTYPE_FLOAT,
		IDC_MM_VE,IDC_MM_VESPIN,
		-999999999.0f,999999999.0f,
		0.01f),

	// 4
	// W Tiling
	ParamUIDesc(
		PB_WS,
		EDITTYPE_FLOAT,
		IDC_MM_WS,IDC_MM_WSSPIN,
		-999999999.0f,999999999.0f,
		0.01f),

	// 5
	// W Offset
	ParamUIDesc(
		PB_WE,
		EDITTYPE_FLOAT,
		IDC_MM_WE,IDC_MM_WESPIN,
		-999999999.0f,999999999.0f,
		0.01f),

	// 6
	// Attenuation On Off
	ParamUIDesc(PB_ATTON,TYPE_SINGLECHEKBOX,IDC_MM_ATTON),

	// 7
	// Overall Attenuation
	ParamUIDesc(
		PB_ATT,
		EDITTYPE_FLOAT,
		IDC_MM_ATT,IDC_MM_ATTSPIN,
		0.0f,100.0f,
		0.1f),

	// 8
	// U Atenuation Start
	ParamUIDesc(
		PB_AUS,
		EDITTYPE_FLOAT,
		IDC_MM_AUS,IDC_MM_AUSSPIN,
		0.0f,999999999.0f,
		0.01f),

	// 9
	// U Atenuation End
	ParamUIDesc(
		PB_AUE,
		EDITTYPE_FLOAT,
		IDC_MM_AUE,IDC_MM_AUESPIN,
		0.0f,999999999.0f,
		0.01f),

	// 10
	// V Atenuation Start
	ParamUIDesc(
		PB_AVS,
		EDITTYPE_FLOAT,
		IDC_MM_AVS,IDC_MM_AVSSPIN,
		0.0f,999999999.0f,
		0.01f),

	// 11
	// V Atenuation End
	ParamUIDesc(
		PB_AVE,
		EDITTYPE_FLOAT,
		IDC_MM_AVE,IDC_MM_AVESPIN,
		0.0f,999999999.0f,
		0.01f),

	// 12
	// W Atenuation Start
	ParamUIDesc(
		PB_AWS,
		EDITTYPE_FLOAT,
		IDC_MM_AWS,IDC_MM_AWSSPIN,
		0.0f,999999999.0f,
		0.01f),

	// 13
	// W Atenuation End
	ParamUIDesc(
		PB_AWE,
		EDITTYPE_FLOAT,
		IDC_MM_AWE,IDC_MM_AWESPIN,
		0.0f,999999999.0f,
		0.01f),

	// 14
	// Attenuation Roundness UV
	ParamUIDesc(
		PB_RUV,
		EDITTYPE_FLOAT,
		IDC_MM_RUV,IDC_MM_RUVSPIN,
		0.0f,100.0f,
		0.1f),

	// MAX 3.0 Borre el pblock desc de preview para manejarlo por flyoff button

	// 15
	// Length
	ParamUIDesc(
		PB_LENGTH,
		EDITTYPE_UNIVERSE,
		IDC_MM_LENGTH,IDC_MM_LENGTHSPIN,
		-999999999.0f,999999999.0f,
		SPIN_AUTOSCALE),

	// 16
	// Width
	ParamUIDesc(
		PB_WIDTH,
		EDITTYPE_UNIVERSE,
		IDC_MM_WIDTH,IDC_MM_WIDTHSPIN,
		-999999999.0f,999999999.0f,
		SPIN_AUTOSCALE),

	// 17
	// Height
	ParamUIDesc(
		PB_HEIGHT,
		EDITTYPE_UNIVERSE,
		IDC_MM_HEIGHT,IDC_MM_HEIGHTSPIN,
		-999999999.0f,999999999.0f,
		SPIN_AUTOSCALE),

	// 18
	// Axis
	ParamUIDesc(PB_AXIS,TYPE_RADIO,axisIDs,3),

	// 19
	// Normalization On Off
	ParamUIDesc(PB_NORMALIZE,TYPE_SINGLECHEKBOX,IDC_SM_NORMALIZED),

	// 20
	ParamUIDesc(PB_REVERSE,TYPE_SINGLECHEKBOX,IDC_SM_REVERSE),

	// 21
	ParamUIDesc(PB_NORMALS,TYPE_RADIO,normsIDs,3),

	// 22
	// FFM Factor de seguridad
	ParamUIDesc(
		PB_FS,
		EDITTYPE_UNIVERSE,
		IDC_MM_FS,IDC_MM_FSSPIN,
		0.0f,999999.0f,
		SPIN_AUTOSCALE),
	};
#define PARAMDESC_LENGH 23

static ParamBlockDescID oldVer0[] = {
	{ TYPE_INT, NULL, FALSE, 0 },		// Type
	{ TYPE_FLOAT, NULL, FALSE, 1 },		// U Tiling
	{ TYPE_FLOAT, NULL, FALSE, 2 },		// U Offset
	{ TYPE_FLOAT, NULL, FALSE, 3 },		// V Tiling
	{ TYPE_FLOAT, NULL, FALSE, 4 },		// V Offset
	{ TYPE_FLOAT, NULL, FALSE, 5 },		// W Tiling
	{ TYPE_FLOAT, NULL, FALSE, 6 },		// W Offset
	{ TYPE_INT, NULL, FALSE, 7},		// Atteunuation On Off
	{ TYPE_FLOAT, NULL, FALSE, 8 },		// Overall Atenuation
	{ TYPE_FLOAT, NULL, FALSE, 9 },		// Atenuation U Start
	{ TYPE_FLOAT, NULL, FALSE, 10 },	// Atenuation U End
	{ TYPE_FLOAT, NULL, FALSE, 11 },	// Atenuation V Start
	{ TYPE_FLOAT, NULL, FALSE, 12 },	// Atenuation V End
	{ TYPE_FLOAT, NULL, FALSE, 13 },	// Atenuation W Start
	{ TYPE_FLOAT, NULL, FALSE, 14 },	// Atenuation W End
	{ TYPE_FLOAT, NULL, FALSE, 15 },	// Att Roundness UV
	{ TYPE_INT, NULL, FALSE, 16 },		// NURBS PATCH Preview
	{ TYPE_FLOAT, NULL, FALSE, 18 },	// Length
	{ TYPE_FLOAT, NULL, FALSE, 19 },	// Width
	{ TYPE_FLOAT, NULL, FALSE, 20 },	// Height
	{ TYPE_INT, NULL, FALSE, 21 },		// Axis
	};

static ParamBlockDescID oldVer1[] = {
	{ TYPE_INT, NULL, FALSE, 0 },		// Type
	{ TYPE_FLOAT, NULL, FALSE, 1 },		// U Tiling
	{ TYPE_FLOAT, NULL, FALSE, 2 },		// U Offset
	{ TYPE_FLOAT, NULL, FALSE, 3 },		// V Tiling
	{ TYPE_FLOAT, NULL, FALSE, 4 },		// V Offset
	{ TYPE_FLOAT, NULL, FALSE, 5 },		// W Tiling
	{ TYPE_FLOAT, NULL, FALSE, 6 },		// W Offset
	{ TYPE_INT, NULL, FALSE, 7},		// Atteunuation On Off
	{ TYPE_FLOAT, NULL, FALSE, 8 },		// Overall Atenuation
	{ TYPE_FLOAT, NULL, FALSE, 9 },		// Atenuation U Start
	{ TYPE_FLOAT, NULL, FALSE, 10 },	// Atenuation U End
	{ TYPE_FLOAT, NULL, FALSE, 11 },	// Atenuation V Start
	{ TYPE_FLOAT, NULL, FALSE, 12 },	// Atenuation V End
	{ TYPE_FLOAT, NULL, FALSE, 13 },	// Atenuation W Start
	{ TYPE_FLOAT, NULL, FALSE, 14 },	// Atenuation W End
	{ TYPE_FLOAT, NULL, FALSE, 15 },	// Att Roundness UV
	{ TYPE_INT, NULL, FALSE, 16 },		// NURBS PATCH Preview
	{ TYPE_FLOAT, NULL, FALSE, 18 },	// Length
	{ TYPE_FLOAT, NULL, FALSE, 19 },	// Width
	{ TYPE_FLOAT, NULL, FALSE, 20 },	// Height
	{ TYPE_INT, NULL, FALSE, 21 },		// Axis
	{ TYPE_INT,   NULL, FALSE, 22 },	// Normalize CheckBox
	{ TYPE_INT,	NULL, FALSE, 23 },		// Reverse CheckBox
	{ TYPE_INT,   NULL, FALSE, 24 },	// Normals Radio Buttons
	{ TYPE_FLOAT, NULL, FALSE, 25 },	// FFM Security grid value (v*0.0001)
	};

static ParamBlockDescID oldVer2[] = {
	{ TYPE_FLOAT, NULL, TRUE, uvw_tile_u },		// U Tiling
	{ TYPE_FLOAT, NULL, TRUE, uvw_offset_u },		// U Offset
	{ TYPE_FLOAT, NULL, TRUE, uvw_tile_v },		// V Tiling
	{ TYPE_FLOAT, NULL, TRUE, uvw_offset_v },		// V Offset
	{ TYPE_FLOAT, NULL, TRUE, uvw_tile_w },		// W Tiling
	{ TYPE_FLOAT, NULL, TRUE, uvw_offset_w },		// W Offset
	{ TYPE_INT,	  NULL, FALSE, uvw_atton},		// Atteunuation On Off
	{ TYPE_FLOAT, NULL, TRUE, uvw_att },		// Overall Atenuation
	{ TYPE_FLOAT, NULL, TRUE, uvw_aus },		// Atenuation U Start
	{ TYPE_FLOAT, NULL, TRUE, uvw_aue },		// Atenuation U End
	{ TYPE_FLOAT, NULL, TRUE, uvw_avs },		// Atenuation V Start
	{ TYPE_FLOAT, NULL, TRUE, uvw_ave },		// Atenuation V End
	{ TYPE_FLOAT, NULL, TRUE, uvw_aws },		// Atenuation W Start
	{ TYPE_FLOAT, NULL, TRUE, uvw_awe },		// Atenuation W End
	{ TYPE_FLOAT, NULL, TRUE, uvw_aruv },		// Att Roundness UV
	{ TYPE_FLOAT, NULL, TRUE, uvw_length },		// Length
	{ TYPE_FLOAT, NULL, TRUE, uvw_width },		// Width
	{ TYPE_FLOAT, NULL, TRUE, uvw_height },		// Height
	{ TYPE_INT, NULL, FALSE, uvw_axis },		// Axis
	{ TYPE_INT,   NULL, FALSE, uvw_normlize },	// Normalize CheckBox
	{ TYPE_INT,	NULL, FALSE, uvw_reverse },		// Reverse CheckBox
	{ TYPE_INT,   NULL, FALSE, uvw_normals },	// Normals Radio Buttons
	{ TYPE_FLOAT, NULL, TRUE, uvw_ffm_thresh },		// FFM Security grid value (v*0.0001)
	};
#define PBLOCK_LENGTH	23

// Array of old versions
static ParamVersionDesc versions[] = {
	ParamVersionDesc (oldVer0, 21, 0),
	ParamVersionDesc (oldVer1, 25, 1),
	ParamVersionDesc (oldVer2, 23, 2),
};
#define NUM_OLDVERSIONS	3

#endif //__MAPPING__

