	for (i=0; i<mesh->getNumVerts(); i++) {
		Point3 uvw = mesh->tVerts[i];

		uvw.x = uvw.x*utile;
		uvw.y = uvw.y*vtile;
		uvw.z = uvw.z*wtile;

		if (uflip) uvw.x =  1.0f - uvw.x;
		if (vflip) uvw.y =  1.0f - uvw.y;
		if (wflip) uvw.z =  1.0f - uvw.z;

		uvw.x -= uoffset;
		uvw.y -= voffset;
		uvw.z -= woffset;

		float tile = tileCrv.GetValue(uvw.y);
		float offset = (tile - 1.0f) / 2.0f;
		uvw.x = uvw.x * tile - offset;

		float radius = radiusCrv.GetValue(uvw.y);
		uvw.z = uvw.z/radius;
		mesh->tVerts[i] = uvw;
		}

	for (i=0; i<mesh->getNumVerts(); i++) {
		Point3 uvw = mesh->tVerts[i];

		uvw.y = uvw.y*vtile;

		float tile = tileCrv.GetValue(uvw.y);
		float offset = (tile - 1.0f) / 2.0f;
		uvw.x = uvw.x * tile - offset;
		uvw.x = uvw.x*utile;

		float radius = radiusCrv.GetValue(uvw.y);
		uvw.z = uvw.z/radius;
		uvw.z = uvw.z*wtile;

		if (uflip) uvw.x =  1.0f - uvw.x;
		uvw.x -= uoffset;
		if (vflip) uvw.y =  1.0f - uvw.y;
		uvw.y -= voffset;
		if (wflip) uvw.z =  1.0f - uvw.z;
		uvw.z -= woffset;

		mesh->tVerts[i] = uvw;
		}


				// Bezier Kind Normals

				Point3 bN,fS,tan;
				tan = Normalize(p1-p0);
				if (j==0) {
					bN = Point3(0,0,1);
					fS = bN^tan0;
					vectors[0].SetVector(Normalize(tan0 ^ fS));
					}
				else {
					bN = vectors[j-1].GetVector();
					fS = bN^tan0;
					}

				vectors[j].SetOutVector(Normalize(BezierTan(1.0f/3.0f,p0,p1,p2,p3) ^ fS));
				vectors[j+1].SetInVector(Normalize(BezierTan(2.0f/3.0f,p0,p1,p2,p3) ^ fS));
				vectors[j+1].SetVector(Normalize((p3-p2) ^ fS));
