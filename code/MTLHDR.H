/**********************************************************************
 *<
	FILE: mtlhdr.h

	DESCRIPTION:

	CREATED BY: Dan Silva

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __MTLHDR__H
#define __MTLHDR__H

#ifdef BLD_MTL
#define MtlExport __declspec( dllexport )
#else
#define MtlExport __declspec( dllimport )
#endif

#include "max.h"
#include "imtl.h"
#include "texutil.h"

extern ClassDesc* GetBMTexDesc();

TCHAR *GetString(int id);

#endif
