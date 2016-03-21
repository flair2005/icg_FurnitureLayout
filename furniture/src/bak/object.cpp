#define _OBJECT_CPP

#include <windows.h>
#include "object.h"


//----------------------------------------------------------------------
void Tobject::readData(FILE *fp) {
    char comment[256];
	int nvtx = 0;
	int _nf;
	int nv;
	Tcolor3<float> ref;

	textured = false;
	//texAddress = NULL;
	char buf[256];
	fgets(buf,256,fp);
	sscanf(buf,"%d%d%f%f%f%s",&nv
                             ,&_nf
                             ,&ref[0]
                             ,&ref[1]
                             ,&ref[2],comment);
	//fscanf(fp,"%d%d%f%f%f%s",&nv
 //                            ,&_nf
 //                            ,&ref[0]
 //                            ,&ref[1]
 //                            ,&ref[2],comment);
	material.setDiffuse(ref);
	fprintf(stderr, "nv, _nf = %d, %d, color: %6.2f %6.2f %6.2f, comment = %s\n", nv, _nf, ref[0], ref[1], ref[2], comment);
	fflush(stderr);
	tri = new Ttriangle[_nf*2];
	vtx = new Tvertex[nv];
	nvtx+=nv;

	if(!strcmp(comment, "textured")) textured = true;

	//頂点座標の読み込み
	for(int j=0; j<nv; j++) {
		Tvector3<float> p;
		fgets(buf,256,fp);
		sscanf(buf,"%f%f%f",&p[0]
						   ,&p[1]
						   ,&p[2]);
	   vtx[j].setPosition(p);
	}

	//テクスチャ座標の読み込み
	//if(textured) {
	//	//tcoord = new Tvector3<float>[nv];
	//	for(int j=0; j<nv; j++) {
	//		Tvector3<float> t;
	//		fgets(buf,256,fp);
	//		sscanf(buf,"%f%f%f",&t[0]
	//						   ,&t[1]
	//						   ,&t[2]);
	//	   vtx[j].setTexCoord(t);
	//	}
	//}

	//接続情報の読み込み
	int iv[4];
	int ntri = 0;
	vector<int> tri_list;
	vector<bool> is_quad;
	for(int j=0; j<_nf; j++) {
		tri_list.clear();
		fgets(buf,256,fp);
		sscanf(buf,"%d%d%d%d",&iv[0]
							 ,&iv[1]
							 ,&iv[2]
							 ,&iv[3]);
		if(iv[2] == iv[3]) iv[3] = 0;
		iv[0]--;iv[1]--;iv[2]--;iv[3]--;
		tri[ntri].setVertexAddress(&vtx[iv[0]], &vtx[iv[1]], &vtx[iv[2]]);
		tri_list.push_back(ntri);
		ntri++;
		if(iv[3] >= 0) {
			is_quad.push_back(true);
			tri[ntri].setVertexAddress(&vtx[iv[0]], &vtx[iv[2]], &vtx[iv[3]]);
			tri_list.push_back(ntri);
			ntri++;
		} else {
			is_quad.push_back(false);
		}
		faceTriangleIndexList.push_back(tri_list);
	}

	nTri = ntri;
	nVert = nv;

    /*=== READING TEXTURE COORDINATES IF TEXTURED ===*/
	if(textured) {
		fgets(buf,256,fp);
        sscanf(buf, "%d", &nTexCoord);
		texCoord = new Tvector3<float>[nTexCoord];

        for(int j=0; j<nTexCoord; j++) {
			fgets(buf,256,fp);
            sscanf(buf, "%f%f", &texCoord[j].x
                              , &texCoord[j].y);
			//printf("%d: %f, %f\n", j, texCoord[j].x, texCoord[j].y);
        }

		int itex[4], tex_id;
		int k=0;
		for(int j=0; j<_nf; j++) {
			if(!is_quad.at(j)) {
				fgets(buf,256,fp);
				sscanf(buf, "%d%d%d", &itex[0], &itex[1], &itex[2]);
				itex[0]--; itex[1]--; itex[2]--; tex_id--;
			} else {
				fgets(buf,256,fp);
				sscanf(buf, "%d%d%d%d", &itex[0], &itex[1], &itex[2], &itex[3]);
				itex[0]--; itex[1]--; itex[2]--; itex[3]--; tex_id--;
			}
			tri[k].setTexCoordAddress(&texCoord[itex[0]], &texCoord[itex[1]], &texCoord[itex[2]]);
			k++;
			if(is_quad.at(j)) {
				tri[k].setTexCoordAddress(&texCoord[itex[0]], &texCoord[itex[2]], &texCoord[itex[3]]);
				k++;
			}
		}
	}

  //  if(textured == TRUE) {
  //      fgets(buf,256,fp);
  //      sscanf(buf, "%d", &nTexCoord);
		//tcoord = new Tvertex3f<float>[nTexCoord]

  //      for(j=0; j<nTexCoord; j++) {
  //          fgets(buf, 256,fp);
  //          sscanf(buf, "%f%f", &texCoord[j].x
  //                            , &texCoord[j].y);
  //      }

  //      for(j=0; j<nTri; j++) {
  //           fgets(buf,256,fp);
  //           sscanf(buf,"%d%d%d%d",&iv[0]
  //                               ,&iv[1]
  //                               ,&iv[2]
  //                               ,&iv[3]);
  //           iv[0]--;iv[1]--;iv[2]--;iv[3]--;
  //           for(k=0; k<4; k++) {
  //               if(iv[k] >= 0)
  //                   obj[i].face[j].tex[k] = &(obj[i].tex_coords[iv[k]]);
  //               else
  //                   obj[i].face[j].tex[k] = NULL;
  //           }
  //      }
  //  }

}
//----------------------------------------------------------------------
void Tobject::translate(Tvector3<float>& t) {
    for(int j=0; j<nVert; j++) {
		Tvector3<float> p = vtx[j].getPosition() + t;
		vtx[j].setPosition(p);
    }
	for(int j=0; j<nTri; j++)
		tri[j].calNormal();
}
//----------------------------------------------------------------------
void Tobject::scale(Tvector3<float>& s) {
    for(int j=0; j<nVert; j++) {
		Tvector3<float> p = vtx[j].getPosition();
		p.x *= s.x;
		p.y *= s.y;
		p.z *= s.z;
		vtx[j].setPosition(p);
    }
	for(int j=0; j<nTri; j++)
		tri[j].calNormal();
}
//----------------------------------------------------------------------
void Tobject::rotateX(float st, float ct) {
	float x,y;
	Tvector3<float> p;
	for(int j=0; j<nVert; j++) {
		p = vtx[j].getPosition();
		x=(p[1])*ct-(p[2])*st;
		y=(p[1])*st+(p[2])*ct;
		p[1]=x;
		p[2]=y;
		vtx[j].setPosition(p);
	}
	for(int j=0; j<nTri; j++)
		tri[j].calNormal();
}
//----------------------------------------------------------------------
void Tobject::rotateY(float st, float ct) {
	float x,y;
	Tvector3<float> p;
	for(int j=0; j<nVert; j++) {
		p  =vtx[j].getPosition();
		x=(p[0])*ct-(p[2])*st;
		y=(p[0])*st+(p[2])*ct;
		p[0]=x;
		p[2]=y;
		vtx[j].setPosition(p);
	}
	for(int j=0; j<nTri; j++)
		tri[j].calNormal();
}
//----------------------------------------------------------------------
void Tobject::rotateZ(float st, float ct) {
	float x,y;
	Tvector3<float> p;
	for(int j=0; j<nVert; j++) {
		p = vtx[j].getPosition();
		x=(p[0])*ct-(p[1])*st;
		y=(p[0])*st+(p[1])*ct;
		p[0]=x;
		p[1]=y;
		vtx[j].setPosition(p);
	}
	for(int j=0; j<nTri; j++)
		tri[j].calNormal();
}

//----------------------------------------------------------------------
void Tobject::calBoundingBox() {

    int j;
	Tvector3<float> p;

    box[0][0] = box[0][1] = box[0][2] = 100000000000.0f;
    box[1][0] = box[1][1] = box[1][2] = -100000000000.0f;
    for(j=0; j<nVert; j++) {
		p = vtx[j].getPosition();
        if(box[0][0] > p[0]) box[0][0] = p[0];
        if(box[0][1] > p[1]) box[0][1] = p[1];
        if(box[0][2] > p[2]) box[0][2] = p[2];

        if(box[1][0] < p[0]) box[1][0] = p[0];
        if(box[1][1] < p[1]) box[1][1] = p[1];
        if(box[1][2] < p[2]) box[1][2] = p[2];
    }


	float dx, dy, dz, cx0, cy0, cz0, r, rmax;

	cx0 = (box[0][0]+box[1][0])/2.0f;
	cy0 = (box[0][1]+box[1][1])/2.0f;
	cz0 = (box[0][2]+box[1][2])/2.0f;
	rmax = -10000000000000.0f;
	for(j=0; j<nVert; j++) {
		p = vtx[j].getPosition();
		dx = cx0 - p[0];
		dy = cy0 - p[1];
		dz = cz0 - p[2];
		r = dx*dx+dy*dy+dz*dz;
		if(rmax < r) rmax = r;
	}
	radius = sqrt(rmax);
	center.assign(cx0, cy0, cz0);
}
//----------------------------------------------------------------------
void Tobject::calVertexNormal() {
	int nv = nVert;
	int *count = new int[nv];

	for(int k=0; k<nv; k++) {
		vtx[k].setNormal(Tvector3<float>(0., 0., 1.));
		count[k] = 0;
	}
    for(int j=0; j<nTri; j++) {
		for(int k=0; k<3; k++) {
			Tvertex *pv = tri[j].getVertexAddress(k);
			Tvector3<float> n = pv->getNormal() + tri[j].getNormal();
			pv->setNormal(n);
			count[pv - vtx]++;
		}
    }
	for(int k=0; k<nv; k++) {
		if(count[k] > 0) {
			Tvector3<float> n = vtx[k].getNormal();
			n /= (float)count[k];
			n.normalize();
			vtx[k].setNormal(n);
		}
	}

	delete [] count;
}
//----------------------------------------------------------------------
void Tobject::draw()
{

    register int j;

	Ttriangle *t = tri;
	Tvertex		*pv;

//	glColor3f(ref.r, ref.g, ref.b);
    glBegin(GL_TRIANGLES);
    for(j=0; j<nTri; j++) {
		pv = t->getVertexAddress(0);
		Tcolor3<float> col = pv->getColor();
		Tvector3<float> pos = pv->getPosition(); 
		glColor3f(col.r, col.g, col.b);
		glVertex3f(pos.x, pos.y, pos.z);

		pv = t->getVertexAddress(1);
		col = pv->getColor();
		pos = pv->getPosition(); 
		glColor3f(col.r, col.g, col.b);
		glVertex3f(pos.x, pos.y, pos.z);

		pv = t->getVertexAddress(2);
		col = pv->getColor();
		pos = pv->getPosition(); 
		glColor3f(col.r, col.g, col.b);
		glVertex3f(pos.x, pos.y, pos.z);

		t++;
    }
    glEnd();
}
//----------------------------------------------------------------------
void Tobject::drawWithNormal()
{

    register int j;

	Ttriangle *t = tri;
	Tvertex		*pv;

//	glColor3f(ref.r, ref.g, ref.b);
    glBegin(GL_TRIANGLES);
    for(j=0; j<nTri; j++) {
		pv = t->getVertexAddress(0);
		Tcolor3<float> col = pv->getColor();
		Tvector3<float> pos = pv->getPosition(); 
		Tvector3<float> normal = pv->getNormal();
		//glColor3f(col.r, col.g, col.b);
		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(pos.x, pos.y, pos.z);


		pv = t->getVertexAddress(1);
		col = pv->getColor();
		pos = pv->getPosition(); 
		normal = pv->getNormal();
		//glColor3f(col.r, col.g, col.b);
		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(pos.x, pos.y, pos.z);

		pv = t->getVertexAddress(2);
		col = pv->getColor();
		pos = pv->getPosition(); 
		normal = pv->getNormal();
		//glColor3f(col.r, col.g, col.b);
		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(pos.x, pos.y, pos.z);

		t++;
    }
    glEnd();
}
//----------------------------------------------------------------------
void Tobject::drawNormal(float length)
{

    register int j;

	Ttriangle *t = tri;

    glBegin(GL_LINES);
    for(j=0; j<nVert; j++) {
		Tvector3<float> p = vtx[j].getPosition();
		Tvector3<float> n = vtx[j].getNormal()*length + p;

		glVertex3f(p.x, p.y, p.z);
		glVertex3f(n.x, n.y, n.z);
    }
    glEnd();
}
