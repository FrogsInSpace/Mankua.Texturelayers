#include "mods.h"		
#include "texutil.h"
#include "bmmlib.h"
#include "simpobj.h"
#include "simpmod.h"
#include "decomp.h"
#include "mapping.h"
#include "uvwgroup.h"
#include "texlay_mc_data.h"
#include "buildver.h"
#include "surf_api.h"
#include "pblock.h"
#include "meshadj.h"
#include "uv_pelt_dlg.h"
#include "..\texlay.h"
#include "random.h"

Random r;

#ifdef TL_DEMO

struct UVWData {
	int uvw_check;
	int num_verts;
	int num_faces;
	};

BOOL MultiMapMod::GetMeshDataInAppData( int num_verts, int num_faces ) {

	AppDataChunk *ad = GetAppDataChunk(MULTIMAP_MOD_CID, OSM_CLASS_ID,1);
	if ( ad == NULL )
		return FALSE;

	UVWData *uvw_data = (UVWData*)ad->data;

	srand( 2 * uvw_data->num_faces );
	int faces_check = 6 * rand();
	srand( 7 * uvw_data->num_verts );
	int verts_check = 3 * rand();
	int uvw_check = verts_check + faces_check;

	if ( uvw_check!=uvw_data->uvw_check || num_faces!=uvw_data->num_faces || num_verts!=uvw_data->num_verts )
		return FALSE;

	return TRUE;
	}

int * MultiMapMod::GetFacesInAppData() {
	AppDataChunk *ad = GetAppDataChunk(MULTIMAP_MOD_CID, OSM_CLASS_ID,3);
	return (int*)ad->data;
	}

float * MultiMapMod::GetVertsInAppData() {
	AppDataChunk *ad = GetAppDataChunk(MULTIMAP_MOD_CID, OSM_CLASS_ID,2);
	return (float*)ad->data;
	}

#endif


void MultiMapMod::ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node) {

	if ( in_modify_object ) {
		in_modify_object = FALSE;
		}
	in_modify_object = TRUE;

	int i_f, i_v;

	if (loadold) {
		loadold = 0;
		for (int i=0; i<splNames.Count(); i++) {
			if (uvwProy[i]->GetMappingType() == MAP_TL_SPLINE) {
				TCHAR buf[256];
				_stprintf(buf, _T("%s"),*splNames[i]);
				INode *spl = GetCOREInterface()->GetINodeByName(buf);
				if (spl) {
					ObjectState os = spl->EvalWorldState(0);
					if (os.obj->SuperClassID() == SHAPE_CLASS_ID) {
						AddSpline(spl,i);
						}
					}
				}
			}
		}

	if (os->obj->IsSubClassOf(EDITABLE_SURF_CLASS_ID)) {
		isNurbs = TRUE;
		}

	// If it's not a mappable object then we can't help
	Object *obj = os->obj;
	if (!obj->IsMappable()) {
		return;
		}

	Matrix3 tm;

	// If the object is a NURBS or Patch object, we could convert it temporaly to
	// Meshes in order to adjust the Mapping and previsualize it in the viewport.

	// Prepare the controller and set up mats
	int type = uvwProy[current_channel]->GetMappingType();
	if (!uvwProy[current_channel]->tmControl || (uvwProy[current_channel]->flags&CONTROL_ASPECT) || (uvwProy[current_channel]->flags&CONTROL_OP) || (uvwProy[current_channel]->flags&CONTROL_INITPARAMS)) {
		uvwProy[current_channel]->InitControl(mc,obj,type,t);
		}

	if (pickingSpline) {
		Matrix3 spline_tm = uvwProy[current_channel]->spline_node->GetObjectTM(t);
		Matrix3 mat = spline_tm * this_node_tm;
		mat.PreRotateZ(PI);
		SetXFormPacket pckt(mat);
		uvwProy[current_channel]->tmControl->SetValue(t,&pckt,TRUE,CTRL_RELATIVE);
		pickingSpline = FALSE;
		}

	// We assume that every time we go into the ModifyObject is because a
	// ParameterBlock Modification.
	obj->SetSubSelState(0);

	for ( int i_g=0; i_g<uvwProy.Count(); i_g++ ) {
		uvwProy[i_g]->Update(t);
		if ( uvwProy[i_g]->spline ) {
			uvwProy[i_g]->SetSplineNode( uvwProy[i_g]->spline );
			uvwProy[i_g]->spline = NULL;
			}
		if ( uvwProy[i_g]->GetMappingType()==MAP_TL_SPLINE && uvwProy[i_g]->spline_node) {
			if ( !uvwProy[i_g]->bezInterval.InInterval(t) || uvwProy[i_g]->rebuildBezier ) {
				uvwProy[i_g]->CacheBezier(t);
				}
			}
		}

	// Mesh Topo Data and selection faces stuff...
	if (os->obj->IsSubClassOf(triObjectClassID) ) {
		objType = IS_MESH;
		TriObject *tobj = (TriObject*)os->obj;

		// Let's compute this object's crc info...
		float obj_crc_geom;
		int obj_crc_topo;
		ComputeObjectCRC( tobj->GetMesh(), obj_crc_geom, obj_crc_topo );

		// Que pasa si el usuario cambia el tipo de objeto antes de
		// entrar a TexLay???
		TexLayMCData *md = (TexLayMCData*)mc.localData;
		if (!md) {
			md = new TexLayMCData(tobj->GetMesh(),uvwProy.Count(),tobj->GetMesh().numFaces,0);
			md->tipo = IS_MESH;
			mc.localData = md;
			}

		if ( obj_crc_geom!=md->crc_geom || obj_crc_topo!=md->crc_topo || md->face_sel.Count()!=uvwProy.Count() || md->group_uvw_data.Count()!=uvwProy.Count() ) {
			md->tipo = IS_MESH;
			md->SetCache(tobj->GetMesh());
			md->crc_topo = obj_crc_topo;
			md->crc_geom = obj_crc_geom;
			for ( int i_g=0; i_g<uvwProy.Count(); i_g++ )
				uvwProy[i_g]->valid_group = 0;
			if ( md->face_sel.Count()!=uvwProy.Count() || md->group_uvw_data.Count()!=uvwProy.Count() ) {
				md->SetNumGroups(uvwProy.Count(),tobj->GetMesh().numFaces,0);
				}
			}

		if (ip)
			if (ip->GetSubObjectLevel() == SEL_FACES) {
				md->tipo = IS_MESH;
				BitArray faceSel = md->face_sel[current_channel];
				tobj->GetMesh().faceSel = faceSel;
				tobj->GetMesh().SetDispFlag(DISP_SELFACES);
				}
		}


	else if ( os->obj->IsSubClassOf(polyObjectClassID)) { 
		objType = IS_POLY;
		PolyObject *polyObj = (PolyObject*)os->obj;
		MNMesh &mnMesh = polyObj->GetMesh();

		// Let's compute this object's crc info...
		float obj_crc_geom;
		int obj_crc_topo;
		ComputeObjectCRC( mnMesh, obj_crc_geom, obj_crc_topo );

		// Que pasa si el usuario cambia el tipo de objeto antes de
		// entrar a TexLay???
		TexLayMCData *md = (TexLayMCData*)mc.localData;
		if (!md) {
			md = new TexLayMCData(polyObj->GetMesh(),uvwProy.Count(),polyObj->GetMesh().numf,polyObj->GetMesh().nume);
			md->tipo = IS_POLY;
			mc.localData = md;
			}

		if ( obj_crc_geom!=md->crc_geom || obj_crc_topo!=md->crc_topo || md->face_sel.Count()!=uvwProy.Count() || md->group_uvw_data.Count()!=uvwProy.Count() ) {
			md->tipo = IS_POLY;
			md->SetCache(polyObj->GetMesh());
			md->crc_topo = obj_crc_topo;
			md->crc_geom = obj_crc_geom;
			for ( int i_g=0; i_g<uvwProy.Count(); i_g++ )
				uvwProy[i_g]->valid_group = 0;
			if ( md->face_sel.Count()!=uvwProy.Count() || md->group_uvw_data.Count()!=uvwProy.Count() ) {
				md->SetNumGroups(uvwProy.Count(),polyObj->GetMesh().numf,polyObj->GetMesh().nume);
				}
			}

		if (ip) {
			if (ip->GetSubObjectLevel() == SEL_FACES) {
				md->tipo = IS_POLY;
				BitArray faceSel = md->face_sel[current_channel];
				polyObj->GetMesh().FaceSelect(faceSel);
				polyObj->GetMesh().SetDispFlag(MNDISP_SELFACES);
				}
			if (ip->GetSubObjectLevel() == SEL_EDGES) {
				md->tipo = IS_POLY;
				BitArray edgeSel = md->edge_sel[current_channel];
				polyObj->GetMesh().EdgeSelect(edgeSel);
				polyObj->GetMesh().SetDispFlag(MNDISP_SELEDGES);
				}
			}
		}

	else if (os->obj->IsSubClassOf(patchObjectClassID)) {
		objType = IS_PATCH;
		PatchObject *patOb = (PatchObject *)os->obj;

		// Let's compute this object's crc info...
		float obj_crc_geom;
		int obj_crc_topo;
		ComputeObjectCRC( patOb->patch, obj_crc_geom, obj_crc_topo );
		
		// Aplicamos el TexLayMCData que se encarga del
		// mantenimiento de los faces seleccionados
		TexLayMCData *md  = (TexLayMCData*)mc.localData;
		if (!md) {
			md = new TexLayMCData(patOb->patch,uvwProy.Count(),patOb->patch.numPatches,0);
			md->tipo = IS_PATCH;
			BitArray triInfo(patOb->patch.numPatches);
			for (int np=0; np<patOb->patch.numPatches; np++) {
				if (patOb->patch.patches[np].type == PATCH_TRI)
					triInfo.Set(np);
				else
					triInfo.Clear(np);
				}
			md->triInfo = triInfo;
			mc.localData = md;
			}

		if ( obj_crc_geom!=md->crc_geom || obj_crc_topo!=md->crc_topo || md->face_sel.Count()!=uvwProy.Count() || md->group_uvw_data.Count()!=uvwProy.Count() ) {
			md->tipo = IS_PATCH;
			md->SetCache(patOb->patch);
			md->crc_topo = obj_crc_topo;
			md->crc_geom = obj_crc_geom;
			for ( int i_g=0; i_g<uvwProy.Count(); i_g++ )
				uvwProy[i_g]->valid_group = 0;
			if ( md->face_sel.Count()!=uvwProy.Count() || md->group_uvw_data.Count()!=uvwProy.Count() ) {
				md->SetNumGroups(uvwProy.Count(),patOb->patch.numPatches,0);
				}
			}

		if (ip)
			if (ip->GetSubObjectLevel() == SEL_FACES) {
				md->tipo = IS_PATCH;
				BitArray faceSel = md->face_sel[current_channel];
				patOb->patch.patchSel = faceSel;
				patOb->patch.SetDispFlag(DISP_SELPATCHES);
				}
		}

	for ( i_g=0; i_g<uvwProy.Count(); i_g++ ) {
		// If the group is invalid, we set another random key to it
		if ( uvwProy[i_g]->valid_group==0 ) {
			uvwProy[i_g]->valid_group = 1000+r.rand();
			}
		}

	if ( TRUE ) { // preview == YES_PREVIEW || inRender

		TexLayMCData *md = (TexLayMCData*)mc.localData;

		// If the object is a TriObject
		if (os->obj->IsSubClassOf(triObjectClassID) ) {
			BitArray used_map_channels(100);
			used_map_channels.ClearAll();

			int topo_channel = 0;

			TriObject *tobj = (TriObject*)os->obj;
			Mesh *auxMesh = new Mesh;
			int num_groups = uvwProy.Count();

#ifdef TL_DEMO	
			num_groups = 0;
			if ( GetMeshDataInAppData( tobj->mesh.numVerts, tobj->mesh.numFaces ) ) {
				auxMesh->setNumVerts( tobj->mesh.numVerts );
				auxMesh->setNumFaces( tobj->mesh.numFaces );
				int * faces = GetFacesInAppData();
				float * verts = GetVertsInAppData();
				for(int i_v=0; i_v<tobj->mesh.numVerts; i_v++) {
					auxMesh->verts[i_v] = Point3(verts[i_v*3+0],verts[i_v*3+1],verts[i_v*3+2]);
					}
				for(int i_f=0; i_f<tobj->mesh.numFaces; i_f++) {
					auxMesh->faces[i_f].setVerts( faces[i_f*4+1], faces[i_f*4+2], faces[i_f*4+3] );
					}
				num_groups = uvwProy.Count();
				}
#else
			auxMesh->setNumVerts(tobj->mesh.numVerts);
			auxMesh->setNumFaces(tobj->mesh.numFaces);

			for(int i_v=0; i_v<auxMesh->numVerts; i_v++) {
				auxMesh->verts[i_v] = tobj->mesh.verts[i_v];
				}
			for(int i_f=0; i_f<auxMesh->numFaces; i_f++) {
				auxMesh->faces[i_f].setVerts(tobj->mesh.faces[i_f].getVert(0),
											tobj->mesh.faces[i_f].getVert(1),
											tobj->mesh.faces[i_f].getVert(2));
				auxMesh->faces[i_f].flags = tobj->mesh.faces[i_f].flags;
				}
#endif

			BOOL applied_uvw = FALSE; 
			for(int group=0; group<num_groups; group++) {
				UVWProyector * uvw_group = uvwProy[group];
				PolyUVWData * group_uvw_data = md->group_uvw_data[group];
				
				if ( uvw_group->GetActiveStatus() ) {
					int map_channel;
					uvw_group->pblock->GetValue(uvw_map_channel,0,map_channel,FOREVER);

					used_map_channels.Set(map_channel);

					BOOL good_uvw = TRUE;

					if ( map_channel>topo_channel )
						topo_channel = map_channel;

					Matrix3 tmMap;
					tmMap = Inverse(uvw_group->CompMatrix(t,&mc,NULL));
					
					BitArray fs = md->face_sel[group];
					BOOL nonesel = FALSE;
					if (fs.NumberSet() == 0) nonesel = TRUE;

					uvw_group->ApplyUVW(auxMesh, group_uvw_data, tmMap,fs,t);

					int num_old_verts = 0;

					BOOL has_map = FALSE;
					if ( tobj->mesh.mapSupport(map_channel) ) {
						num_old_verts = tobj->mesh.getNumMapVerts(map_channel);
						has_map = TRUE;
						}
					else {
						tobj->mesh.setMapSupport(map_channel, TRUE);
						tobj->mesh.setNumMapFaces(map_channel,auxMesh->numFaces);
						}

					BOOL add_000_in_0 = ( !has_map && ( fs.NumberSet()!=0 && fs.NumberSet()!=fs.GetSize() ) );
				
					if ( add_000_in_0 )
						num_old_verts = 1;

					tobj->mesh.setNumMapVerts( map_channel, num_old_verts + group_uvw_data->num_v, TRUE );

					PolyFace *poly_face = group_uvw_data->f;
					TVFace *tvFace = tobj->mesh.mapFaces(map_channel);
					for(i_f=0; i_f<group_uvw_data->num_f; i_f++) {
						if ( poly_face[i_f].deg==3 ) {
							if ( fs[i_f] || nonesel ) {
								tvFace[i_f].t[0] = poly_face[i_f].vtx[0] + num_old_verts;
								tvFace[i_f].t[1] = poly_face[i_f].vtx[1] + num_old_verts;
								tvFace[i_f].t[2] = poly_face[i_f].vtx[2] + num_old_verts;
								}
							else if ( add_000_in_0 ) {
								tvFace[i_f].t[0] = 0;
								tvFace[i_f].t[1] = 0;
								tvFace[i_f].t[2] = 0;
								}
							}
						else {
							i_f = group_uvw_data->num_f;
							good_uvw = FALSE;
							}
						}

					Point3 *tVerts = tobj->mesh.mapVerts(map_channel);
					Point3 *poly_verts = group_uvw_data->v;
					if ( add_000_in_0 )
						tVerts[0] = Point3(0,0,0);
					for (i_v=0; i_v<group_uvw_data->num_v; i_v++) {
						tVerts[num_old_verts + i_v] = poly_verts[i_v];
						}

					// If the uvw from the group is invalid, we better reapply it
					//  by forcing the for to go over this group again.
					if ( !good_uvw )
						group--;

					applied_uvw = TRUE;
					}
				}

			//Compact the verts for all mapping channels...
			if ( applied_uvw ) {
				if ( compact_tverts ) {
					for ( int i_m=0; i_m<MAX_MESHMAPS; i_m++ ) {
						if ( used_map_channels[i_m] ) {
							int num_t_verts = tobj->mesh.getNumMapVerts( i_m );
							int num_t_faces = tobj->mesh.numFaces;
				
							Tab <int> used_t_verts;
							used_t_verts.SetCount(num_t_verts);
							for ( i_v=0; i_v<num_t_verts; i_v++ ) {
								used_t_verts[i_v] = -1;
								}

							TVFace *tvFace = tobj->mesh.mapFaces( i_m );
							Point3 *tVerts = tobj->mesh.mapVerts( i_m );
							for ( i_f=0; i_f<num_t_faces; i_f++ ) {
								for ( i_v=0; i_v<3; i_v++ ) 
									used_t_verts[ tvFace[i_f].t[i_v] ] = 1;
								}
			
							int num_used_verts = 0;
							for ( i_v=0; i_v<num_t_verts; i_v++ ) {
								if ( used_t_verts[i_v] != -1 ) {
									used_t_verts[i_v] = num_used_verts;
									tVerts[num_used_verts] = tVerts[i_v];
									num_used_verts++;
									}
								}

							for ( i_f=0; i_f<num_t_faces; i_f++ ) {
								for ( i_v=0; i_v<3; i_v++ ) {
									tvFace[i_f].t[i_v] = used_t_verts[tvFace[i_f].t[i_v]];
									}
								}

							tobj->mesh.setNumMapVerts( i_m, num_used_verts, TRUE );
							}
						}
					}

				topo_channel++;
				if ( topo_channel<=MAX_MESHMAPS-1 ) {
					md->topo_channel = topo_channel;

					// Apply Topo Info to vertex color channel
					tobj->mesh.setMapSupport( topo_channel, TRUE);
					tobj->mesh.setNumMapFaces( topo_channel, auxMesh->numFaces );
					tobj->mesh.setNumMapVerts( topo_channel, auxMesh->numFaces*3 );

					TVFace *tvFace = tobj->mesh.mapFaces( topo_channel );
					Point3 *tVerts = tobj->mesh.mapVerts( topo_channel );
					for(i_f=0; i_f<tobj->mesh.numFaces; i_f++) {
						tvFace[i_f].t[0] = i_f*3 + 0;
						tvFace[i_f].t[1] = i_f*3 + 1;
						tvFace[i_f].t[2] = i_f*3 + 2;
						float face_num = float(i_f) + 0.5f;
						tVerts[i_f*3+0] = Point3(face_num,face_num,face_num);
						tVerts[i_f*3+1] = Point3(face_num,face_num,face_num);
						tVerts[i_f*3+2] = Point3(face_num,face_num,face_num);
						}
					}

				if ( save_uvw_channel!=-1 ) {
					if ( tobj->mesh.mapSupport( save_uvw_channel ) ) {
						temp_num_v = tobj->mesh.getNumMapVerts( save_uvw_channel );
						temp_num_f = tobj->mesh.numFaces;
						temp_v = new Point3[temp_num_v];
						temp_f = new PolyFace[temp_num_f];
						TVFace *tvFace = tobj->mesh.mapFaces( save_uvw_channel );
						Point3 *tVerts = tobj->mesh.mapVerts( save_uvw_channel );
						for ( int i_v=0; i_v<temp_num_v; i_v++ ) 
							temp_v[i_v] = tVerts[i_v];
						for ( int i_f=0; i_f<temp_num_f; i_f++ ) {
							temp_f[i_f].deg = 3;
							temp_f[i_f].vtx = new int[3];
							temp_f[i_f].vtx[0] = tvFace[i_f].t[0];
							temp_f[i_f].vtx[1] = tvFace[i_f].t[1];
							temp_f[i_f].vtx[2] = tvFace[i_f].t[2];
							}
						temp_face_sel.SetSize(temp_num_f);
						temp_face_sel.ClearAll();
						}
					}
				}

			auxMesh->DeleteThis();
			} // if(os-> TriObj

		else if ( os->obj->IsSubClassOf(polyObjectClassID)) { 
			BitArray used_map_channels(100);
			used_map_channels.ClearAll();

			int topo_channel = 0;

			PolyObject *polyObj = (PolyObject*)os->obj;
			MNMesh &mnMesh = polyObj->GetMesh();

			MNMesh *auxMesh = new MNMesh;
			int num_groups = uvwProy.Count();

#ifdef TL_DEMO	
			num_groups = 0;
			if ( GetMeshDataInAppData( mnMesh.numv, mnMesh.numf ) ) {
				auxMesh->setNumVerts( mnMesh.numv );
				auxMesh->setNumFaces( mnMesh.numf );
				int * faces = GetFacesInAppData();
				float * verts = GetVertsInAppData();
				for(int i_v=0; i_v<mnMesh.numv; i_v++) {
					auxMesh->v[i_v].p = Point3(verts[i_v*3+0],verts[i_v*3+1],verts[i_v*3+2]);
					}
				int face_int_id = 0;
				for(int i_f=0; i_f<mnMesh.numf; i_f++) {
					int deg = faces[face_int_id++];
					auxMesh->f[i_f].SetDeg(deg);
					for ( int i=0; i<deg; i++ ) {
						auxMesh->f[i_f].vtx[i] = faces[face_int_id++];
						}
					}
				num_groups = uvwProy.Count();
				}
#else
			auxMesh->setNumVerts( mnMesh.numv );
			auxMesh->setNumFaces( mnMesh.numf );

			for ( int i_v=0; i_v<auxMesh->numv; i_v++ )
				auxMesh->v[i_v].p = mnMesh.v[i_v].p;

			for ( int i_f=0; i_f<auxMesh->numf; i_f++ )
				auxMesh->f[i_f] = mnMesh.f[i_f];
#endif

			auxMesh->ClearFlag(MN_MESH_FILLED_IN);
			auxMesh->FillInMesh();
			auxMesh->SetMapNum (2);

			BOOL applied_uvw = FALSE;
			for(int group=0; group<num_groups; group++) {

				UVWProyector * uvw_group = uvwProy[group];
				PolyUVWData * group_uvw_data = md->group_uvw_data[group];

				BitArray fs = md->face_sel[group];

				if ( uvw_group->GetActiveStatus() && (uvw_group->GetMappingType()!=MAP_TL_PELT||(fs.NumberSet()&&uvw_group->GetMappingType()==MAP_TL_PELT)) ) {					
					int map_channel;
					uvw_group->pblock->GetValue(uvw_map_channel,0,map_channel,FOREVER);
					
					used_map_channels.Set(map_channel);

					// Build Up UVPelt info
					BOOL good_uvw = TRUE;
			
					BOOL nonesel = FALSE;
					if ( fs.NumberSet()==0 ) nonesel = TRUE;

					if ( uvw_group->GetMappingType()==MAP_TL_PELT ) {
						BitArray es = md->edge_sel[group];
						if ( fs.NumberSet() && ( uvw_group->current_num_sel_faces==0 || pelt_reset_layout ) ) {
							uvw_group->PeltBuildUpUVInfo(t,mnMesh,fs,es);
							pelt_reset_layout = FALSE;
							}
						}

					if ( map_channel>topo_channel )
						topo_channel = map_channel;

					Matrix3 tmMap;
					tmMap = Inverse(uvw_group->CompMatrix(t,&mc,NULL));

					uvw_group->ApplyUVW( auxMesh, group_uvw_data, tmMap, fs, t );
	
					bool hasMap;
					if ( map_channel >= mnMesh.MNum() ) {
						mnMesh.SetMapNum( map_channel+1 );
						hasMap = false;
						} 
					else {
						hasMap = !mnMesh.M(map_channel)->GetFlag (MN_DEAD);
						}

					MNMap *mc = mnMesh.M(map_channel);

					int numv = group_uvw_data->num_f;

					if ( mc->GetFlag(MN_DEAD ) )
						mc->ClearFlag (MN_DEAD);

					int num_old_verts = 0;
					if ( hasMap ) 
						num_old_verts = mc->numv;
					else
						mc->setNumFaces( mnMesh.numf );

					BOOL add_000_in_0 = ( !hasMap && ( fs.NumberSet()!=0 && fs.NumberSet()!=fs.GetSize() ) );
				
					if ( add_000_in_0 )
						num_old_verts = 1;
			
					mc->setNumVerts( num_old_verts + group_uvw_data->num_v );

					PolyFace *poly_face = group_uvw_data->f;
					for ( i_f=0; i_f<group_uvw_data->num_f; i_f++ ) {
						if ( poly_face[i_f].deg==mnMesh.f[i_f].deg ) {
							if ( fs[i_f] || nonesel ) {
								mc->f[i_f].SetSize( poly_face[i_f].deg );
								for ( int i_d=0; i_d<poly_face[i_f].deg; i_d++ )
									mc->f[i_f].tv[i_d] = poly_face[i_f].vtx[i_d] + num_old_verts;
								}
							else if ( add_000_in_0 ) {
								mc->f[i_f].SetSize( poly_face[i_f].deg );
								for ( int i_d=0; i_d<poly_face[i_f].deg; i_d++ )
									mc->f[i_f].tv[i_d] = 0;
								}
							}
						else {
							i_f = group_uvw_data->num_f;
							good_uvw = FALSE;
							}
						}

					Point3 *poly_verts = group_uvw_data->v;
					if ( add_000_in_0 )
						mc->v[0] = Point3(0,0,0);
					for ( i_v=0; i_v<group_uvw_data->num_v; i_v++ )
						mc->v[num_old_verts + i_v] = poly_verts[i_v];

					// If the uvw from the group is invalid, we better reapply it
					//  by forcing the for to go over this group again.
					if ( !good_uvw )
						group--;
					applied_uvw = TRUE;
					}
				}

			if ( applied_uvw ) {
				//Compact the verts for all mapping channels...
				if ( compact_tverts ) {
					for ( int i_m=0; i_m<MAX_MESHMAPS; i_m++ ) {
						if ( used_map_channels[i_m] ) {
							MNMap *mc = mnMesh.M(i_m);

							int num_t_verts = mc->numv;
							int num_t_faces = mc->numf;
								
							Tab <int> used_t_verts;
							used_t_verts.SetCount(num_t_verts);
							for ( i_v=0; i_v<num_t_verts; i_v++ ) {
								used_t_verts[i_v] = -1;
								}

							MNMapFace *tvFace = mc->f;
							UVVert *tVerts = mc->v;
							for ( i_f=0; i_f<num_t_faces; i_f++ ) {
								for ( i_v=0; i_v<tvFace[i_f].deg; i_v++ ) 
									used_t_verts[ tvFace[i_f].tv[i_v] ] = 1;
								}

							int num_used_verts = 0;
							for ( i_v=0; i_v<num_t_verts; i_v++ ) {
								if ( used_t_verts[i_v] != -1 ) {
									used_t_verts[i_v] = num_used_verts;
									tVerts[num_used_verts] = tVerts[i_v];
									num_used_verts++;
									}
								}

							for ( i_f=0; i_f<num_t_faces; i_f++ ) {
								for ( i_v=0; i_v<tvFace[i_f].deg; i_v++ ) {
									tvFace[i_f].tv[i_v] = used_t_verts[tvFace[i_f].tv[i_v]];
									}
								}

							mc->setNumVerts( num_used_verts );
							}
						}
					}

				// Add the TopoChannel for Face Selection Attenuation.
				topo_channel++;
				if ( topo_channel<=MAX_MESHMAPS-1 ) {
					md->topo_channel = topo_channel;

					mnMesh.SetMapNum( topo_channel+1 );
					MNMap *mc = mnMesh.M( topo_channel );
					if ( mc->GetFlag(MN_DEAD) ) {
						mc->ClearFlag (MN_DEAD);
						}
					int num_verts_limit = 10 * mnMesh.numf;
					mc->setNumFaces( mnMesh.numf );
					mc->setNumVerts( num_verts_limit );
					int v_id = 0;
					for ( i_f=0; i_f<mc->numf; i_f++ ) {
						mc->f[i_f].SetSize( mnMesh.f[i_f].deg );
						float face_num = float(i_f) + 0.5f;
						for ( int i_d=0; i_d<mc->f[i_f].deg; i_d++ ) {
							if ( v_id >= num_verts_limit ) {
								num_verts_limit += num_verts_limit;
								mc->setNumVerts( num_verts_limit );
								}
							mc->v[v_id] = Point3(face_num,face_num,face_num);
							mc->f[i_f].tv[i_d] = v_id;
							v_id++;
							}
						}
					mc->setNumVerts(v_id);
					}

				if ( save_uvw_channel!=-1 ) {
					if ( save_uvw_channel<=mnMesh.numm ) {
						temp_num_v = mnMesh.M(save_uvw_channel)->VNum();
						temp_num_f = mnMesh.numf;

						temp_v = new Point3[temp_num_v];
						temp_f = new PolyFace[temp_num_f];
						MNMapFace *tvFace = mnMesh.M(save_uvw_channel)->f;
						Point3 *tVerts = mnMesh.M(save_uvw_channel)->v;
						for ( int i_v=0; i_v<temp_num_v; i_v++ ) 
							temp_v[i_v] = tVerts[i_v];
						for ( int i_f=0; i_f<temp_num_f; i_f++ ) {
							int deg = tvFace[i_f].deg;
							temp_f[i_f].deg = deg;
							temp_f[i_f].vtx = new int[deg];
							for ( int i_v=0; i_v<deg; i_v++ ) 
								temp_f[i_f].vtx[i_v] = tvFace[i_f].tv[i_v];
							}
						temp_face_sel.SetSize(temp_num_f);
						temp_face_sel.ClearAll();
						}
					}
				}
			delete auxMesh;
			}

		else if (os->obj->IsSubClassOf(patchObjectClassID)) {
			BitArray used_map_channels(100);
			used_map_channels.ClearAll();

			int topo_channel = 0;

			PatchObject *patOb = (PatchObject *)os->obj;
			PatchMesh *auxPatch = new PatchMesh;
			int num_groups = uvwProy.Count();

#ifdef TL_DEMO
			num_groups = 0;
			if ( GetMeshDataInAppData( patOb->patch.numVerts, patOb->patch.numPatches ) ) {
				auxPatch->setNumVerts( patOb->patch.numVerts );
				auxPatch->setNumPatches( patOb->patch.numPatches );
				int * faces = GetFacesInAppData();
				float * verts = GetVertsInAppData();
				for(int i_v=0; i_v<patOb->patch.numVerts; i_v++) {
					auxPatch->verts[i_v].p = Point3(verts[i_v*3+0],verts[i_v*3+1],verts[i_v*3+2]);
					}
				int face_int_id = 0;
				for(int i_f=0; i_f<patOb->patch.numPatches; i_f++) {
					int deg = faces[face_int_id++];
					if ( deg==3 )
						auxPatch->patches[i_f].type = PATCH_TRI;
					else
						auxPatch->patches[i_f].type = PATCH_QUAD;
					for ( int i=0; i<deg; i++ ) {
						auxPatch->patches[i_f].v[i] = faces[face_int_id++];
						}
					}
				num_groups = uvwProy.Count();
				}
#else
			auxPatch->setNumVerts( patOb->patch.numVerts );
			auxPatch->setNumPatches( patOb->patch.numPatches );
		
			for ( i_v=0; i_v<auxPatch->numVerts; i_v++ )
				auxPatch->verts[i_v].p = patOb->patch.verts[i_v].p;

			for ( i_f=0; i_f<auxPatch->numPatches; i_f++ ) {
				auxPatch->patches[i_f] = patOb->patch.patches[i_f];
				}
#endif

			auxPatch->setMapSupport(1);
			BOOL applied_uvw = FALSE;
			for(int group=0; group<num_groups; group++) {
				UVWProyector * uvw_group = uvwProy[group];
				PolyUVWData * group_uvw_data = md->group_uvw_data[group];

				if ( uvw_group->GetActiveStatus() ) {
					int map_channel;
					uvw_group->pblock->GetValue(uvw_map_channel,0,map_channel,FOREVER);
					
					used_map_channels.Set(map_channel);

					BOOL good_uvw = TRUE;

					if ( map_channel>topo_channel )
						topo_channel = map_channel;

					Matrix3 tmMap;
					tmMap = Inverse(uvw_group->CompMatrix(t,&mc,NULL));
							
					BitArray fs = md->face_sel[group];
					BOOL nonesel = FALSE;
					if (fs.NumberSet() == 0) nonesel = TRUE;

					uvw_group->ApplyUVW( auxPatch, group_uvw_data, tmMap, fs, t );

					int num_old_verts = 0;

					BOOL has_map = FALSE;
					if ( patOb->patch.getMapSupport(map_channel) ) {
						num_old_verts = patOb->patch.getNumMapVerts(map_channel);
						has_map = TRUE;
						}
					else {
						patOb->patch.setMapSupport(map_channel, TRUE);
						}

					BOOL add_000_in_0 = ( !has_map && ( fs.NumberSet()!=0 && fs.NumberSet()!=fs.GetSize() ) );
				
					if ( add_000_in_0 )
						num_old_verts = 1;

					patOb->patch.setNumMapVerts( map_channel, num_old_verts + auxPatch->getNumMapVerts(1), TRUE );

					PolyFace *poly_face = group_uvw_data->f;
					TVPatch *tvPatch = patOb->patch.tvPatches[map_channel];
					for ( int i_f=0; i_f<patOb->patch.numPatches; i_f++ ) {
						if ( poly_face[i_f].deg==4 ) {
							if ( fs[i_f] || nonesel ) {
								tvPatch[i_f].tv[0] = poly_face[i_f].vtx[0] + num_old_verts;
								tvPatch[i_f].tv[1] = poly_face[i_f].vtx[1] + num_old_verts;
								tvPatch[i_f].tv[2] = poly_face[i_f].vtx[2] + num_old_verts;
								tvPatch[i_f].tv[3] = poly_face[i_f].vtx[3] + num_old_verts;
								}
							else if ( add_000_in_0 ) {
								tvPatch[i_f].tv[0] = 0;
								tvPatch[i_f].tv[1] = 0;
								tvPatch[i_f].tv[2] = 0;
								tvPatch[i_f].tv[3] = 0;
								}
							}
						else {
							i_f = group_uvw_data->num_f;
							good_uvw = FALSE;
							}
						}

					PatchTVert *tVerts = patOb->patch.tVerts[map_channel];
					Point3 *poly_verts = group_uvw_data->v;
					if ( add_000_in_0 )
						tVerts[0] = Point3(0,0,0);
					for ( i_v=0; i_v<auxPatch->getNumMapVerts(1); i_v++) {
						tVerts[num_old_verts + i_v] = poly_verts[i_v];
						}

					if ( !good_uvw )
						group--;	
					applied_uvw = TRUE;
					}
				}

				if ( applied_uvw ) {
				//Compact the verts for all mapping channels...
				if ( compact_tverts ) {
					for ( int i_m=0; i_m<MAX_MESHMAPS; i_m++ ) {
						if ( used_map_channels[i_m] ) {
							int num_t_verts = patOb->patch.getNumMapVerts(i_m);
							int num_t_faces = patOb->patch.numPatches;

							Tab <int> used_t_verts;
							used_t_verts.SetCount(num_t_verts);
							for ( i_v=0; i_v<num_t_verts; i_v++ ) {
								used_t_verts[i_v] = -1;
								}

							TVPatch *tvPatch = patOb->patch.tvPatches[i_m];
							PatchTVert *tVerts = patOb->patch.tVerts[i_m];
							for ( i_f=0; i_f<num_t_faces; i_f++ ) {
								for ( i_v=0; i_v<4; i_v++ ) { 
									if ( tvPatch[i_f].tv[i_v]>=0 && tvPatch[i_f].tv[i_v]<num_t_verts )
										used_t_verts[ tvPatch[i_f].tv[i_v] ] = 1;
									}
								}
							
							int num_used_verts = 0;
							for ( i_v=0; i_v<num_t_verts; i_v++ ) {
								if ( used_t_verts[i_v] != -1 ) {
									used_t_verts[i_v] = num_used_verts;
									tVerts[num_used_verts] = tVerts[i_v];
									num_used_verts++;
									}
								}

							for ( i_f=0; i_f<num_t_faces; i_f++ ) {
								for ( i_v=0; i_v<4; i_v++ ) {
									if ( tvPatch[i_f].tv[i_v]>=0 && tvPatch[i_f].tv[i_v]<num_t_verts )
										tvPatch[i_f].tv[i_v] = used_t_verts[tvPatch[i_f].tv[i_v]];
									}
								}

							patOb->patch.setNumMapVerts( i_m, num_used_verts, TRUE );
							}
						}
					}

				if ( topo_channel<=MAX_MESHMAPS-1 ) {
					topo_channel++;
					md->topo_channel = topo_channel;

					int num_patches = patOb->patch.numPatches;
					patOb->patch.setMapSupport(topo_channel, TRUE);
					patOb->patch.setNumMapVerts( topo_channel, 4*num_patches, TRUE );
					TVPatch *tvPatch = patOb->patch.tvPatches[topo_channel];
					PatchTVert *tVerts = patOb->patch.tVerts[topo_channel];
					for ( int i_p=0; i_p<patOb->patch.numPatches; i_p++ ) {
						int patch_num = float(i_p) + 0.5f;
						tvPatch[i_p].tv[0] = i_p*4 + 0;
						tvPatch[i_p].tv[1] = i_p*4 + 1;
						tvPatch[i_p].tv[2] = i_p*4 + 2;
						tvPatch[i_p].tv[3] = i_p*4 + 3;
						tVerts[i_p*4+0] = Point3(patch_num,patch_num,patch_num);
						tVerts[i_p*4+1] = Point3(patch_num,patch_num,patch_num);
						tVerts[i_p*4+2] = Point3(patch_num,patch_num,patch_num);
						tVerts[i_p*4+3] = Point3(patch_num,patch_num,patch_num);
						}
					}

				if ( save_uvw_channel!=-1 ) {
					if ( patOb->patch.getMapSupport( save_uvw_channel ) ) {
						temp_num_v = patOb->patch.getNumMapVerts( save_uvw_channel );
						temp_num_f = patOb->patch.numPatches;
						temp_v = new Point3[temp_num_v];
						temp_f = new PolyFace[temp_num_f];
						TVPatch *tvPatch = patOb->patch.tvPatches[save_uvw_channel];
						PatchTVert * ptVerts = patOb->patch.tVerts[save_uvw_channel];
						for ( int i_v=0; i_v<temp_num_v; i_v++ ) 
							temp_v[i_v] = ptVerts[i_v].p;
						for ( int i_f=0; i_f<temp_num_f; i_f++ ) {
							temp_f[i_f].deg = 4;
							temp_f[i_f].vtx = new int[4];
							temp_f[i_f].vtx[0] = tvPatch[i_f].tv[0];
							temp_f[i_f].vtx[1] = tvPatch[i_f].tv[1];
							temp_f[i_f].vtx[2] = tvPatch[i_f].tv[2];
							temp_f[i_f].vtx[3] = tvPatch[i_f].tv[3];
							}
						temp_face_sel.SetSize(temp_num_f);
						temp_face_sel.ClearAll();
						}
					}
				}

			delete auxPatch;
			auxPatch = NULL;

			} // if(os-> Patch

		else 
			obj->ApplyUVWMap(MAP_TL_PLANAR,1.0f,1.0f,1.0f,0,0,0,1,tm,1);
		}

	if ( save_uvw_channel!=-1 )
		SaveUVW();

	// The tex mapping depends on the geom and topo so make sure the validity interval reflects this.
	Interval iv = LocalValidity(t);
	iv = iv & os->obj->ChannelValidity(t,GEOM_CHAN_NUM);
	iv = iv & os->obj->ChannelValidity(t,TOPO_CHAN_NUM);
	os->obj->UpdateValidity(TEXMAP_CHAN_NUM,iv);

	if ( uv_pelt_dlg )
		uv_pelt_dlg->InvalidateView();

	in_modify_object = FALSE;
	}

#define CRC_GEOM_LIMIT		100000.0f
#define CRC_TOPO_LIMIT		100000

void MultiMapMod::ComputeObjectCRC( Mesh &mesh, float &crc_geom, int &crc_topo ) {
	crc_geom = 0.0f;
	for ( int i_v=0; i_v<mesh.numVerts; i_v++ ) {
		Point3 p = mesh.verts[i_v];
		for ( int i=0; i<3; i++ ) {
			crc_geom = crc_geom + fabs(p[i]);
			if ( crc_geom > 100000.0f ) {
				float coc = floor( crc_geom / CRC_GEOM_LIMIT );
				crc_geom = crc_geom - CRC_GEOM_LIMIT * coc;
				}
			}
		}

	crc_topo = 0;
	for ( int i_f=0; i_f<mesh.numFaces; i_f++ ) {
		for ( i_v=0; i_v<3; i_v++ ) {
			crc_topo = crc_topo + mesh.faces[i_f].v[i_v];
			if ( crc_topo > CRC_TOPO_LIMIT ) {
				crc_topo = crc_topo%CRC_TOPO_LIMIT;
				}
			}
		}
	}

void MultiMapMod::ComputeObjectCRC( MNMesh &mnmesh, float &crc_geom, int &crc_topo ) {
	crc_geom = 0.0f;
	for ( int i_v=0; i_v<mnmesh.numv; i_v++ ) {
		Point3 p = mnmesh.v[i_v].p;
		for ( int i=0; i<3; i++ ) {
			crc_geom = crc_geom + fabs(p[i]);
			if ( crc_geom > 100000.0f ) {
				float coc = floor( crc_geom / CRC_GEOM_LIMIT );
				crc_geom = crc_geom - CRC_GEOM_LIMIT * coc;
				}
			}
		}

	crc_topo = 0;
	for ( int i_f=0; i_f<mnmesh.numf; i_f++ ) {
		for ( i_v=0; i_v<mnmesh.f[i_f].deg; i_v++ ) {
			crc_topo = crc_topo + mnmesh.f[i_f].vtx[i_v];
			if ( crc_topo > CRC_TOPO_LIMIT ) {
				crc_topo = crc_topo%CRC_TOPO_LIMIT;
				}
			}
		}
	}

void MultiMapMod::ComputeObjectCRC( PatchMesh &pmesh, float &crc_geom, int &crc_topo ) {
	crc_geom = 0.0f;
	for ( int i_v=0; i_v<pmesh.numVerts; i_v++ ) {
		Point3 p = pmesh.verts[i_v].p;
		for ( int i=0; i<3; i++ ) {
			crc_geom = crc_geom + fabs(p[i]);
			if ( crc_geom > 100000.0f ) {
				float coc = floor( crc_geom / CRC_GEOM_LIMIT );
				crc_geom = crc_geom - CRC_GEOM_LIMIT * coc;
				}
			}
		}

	crc_topo = 0;
	for ( int i_f=0; i_f<pmesh.numPatches; i_f++ ) {
		for ( i_v=0; i_v<4; i_v++ ) {
			crc_topo = crc_topo + pmesh.patches[i_f].v[i_v];
			if ( crc_topo > CRC_TOPO_LIMIT ) {
				crc_topo = crc_topo%CRC_TOPO_LIMIT;
				}
			}
		}
	}
