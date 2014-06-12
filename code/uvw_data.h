#include "max.h"

#ifndef MAX_RELEASE_R9
#include "max_mem.h"
#endif

#include "tl_classes.h"

class PolyUVWData {
	public:
		int valid_group_key;
		int num_v;
		int num_f;
		Point3 * v;
		PolyFace * f;

		PolyUVWData();
		~PolyUVWData();
		
		void DeleteUVWData();
		void CopyUVWData( PolyUVWData *ori_uvw_data );
		BOOL CompareUVWData( PolyUVWData *ori_uvw_data );
		BOOL CompareFaceSize( PolyUVWData *ori_uvw_data );
	};