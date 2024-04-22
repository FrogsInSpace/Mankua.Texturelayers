#ifndef _TL_LOADCB_H_
#define _TL_LOADCB_H_

#include "max.h"
//#include "pblock.h"
#include "mapping.h"
#include "uvwgroup.h"
#include "texlay_mc_data.h"
#include "att.h"

class MMMLoadCB : public PostLoadCallback 
{
public:
	MultiMapMod * mmm;
	int load_version;

	MMMLoadCB(MultiMapMod * mod, int version) { 
		mmm = mod; 
		load_version = version;
		}
    
	void proc(ILoad *iload) {

		if ( mmm->GetPBlockVersion()<mmm->GetFirstParamVer()) {
			mmm->flags |= CONTROL_INITPARAMS;
			}

		// If loading from TL 1.0 or 1.1 we have to fix the 
		// mapping groups to have the same mapping channel than
		// the mapping group number
		if ( load_version < 100900 ) {
			mmm->texlay_1_0 = TRUE;
			for ( int i_g=0; i_g<mmm->uvwProy.Count(); i_g++ ) {
				int map_channel = i_g+1;
				mmm->uvwProy[i_g]->pblock->SetValue( uvw_map_channel, 0, map_channel );
				}
			}
         
		delete this;
	}
};

class UVWGroupLoadCB : public PostLoadCallback 
{
public:
	UVWProyector * uvwg;
	int load_version;
	int mapping_type;

	UVWGroupLoadCB(UVWProyector * g, int mt) { 
		uvwg = g; 
		mapping_type = mt;
		}
    
	void proc(ILoad *iload) {
		uvwg->SetMappingType(mapping_type);
		delete this;
		}
	};

class AttMapLoadCB : public PostLoadCallback 
{
public:
	AttMap * att;
	int load_version;

	AttMapLoadCB(AttMap * map, int version) { 
		att = map; 
		load_version = version;
		}
    
	void proc(ILoad *iload) {

		// If loading from TL 1.0 or 1.1 we have to fix the 
		// map to use TexLayGroups and set the group to the 
		// same channel number
		if ( load_version < 100900 ) {
			BOOL use_mod = TRUE;
			int mod_group;
			att->pblock->GetValue(pb_map_channel,0,mod_group,FOREVER);
			att->pblock->SetValue(pb_mod_group,0,mod_group);
			}
         
		delete this;
	}
};

 
#endif