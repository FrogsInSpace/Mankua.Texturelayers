#ifndef _COMMON_H_
#define _COMMON_H_

#include "max.h"

struct MultiMapModData {
	MultiMapMod *mod;
	TexLayMCData *mtd;
	};

MultiMapModData GetMultiMapMod( INode* node, TimeValue t );
MultiMapMod * GetModifiersMapChs(MtlBase *mtl,BitArray &mapreq, BitArray &bumpreq);

void DisplayTexLayAbout();

#endif