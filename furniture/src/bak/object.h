#ifndef _OBJECT_H
#define _OBJECT_H

#include "doba-cg.h"

class Tmaterial {
protected:
	Tcolor3<float> ambient;
	Tcolor3<float> diffuse;
	Tcolor3<float> specular;
	int			   nPhong;
public:
	Tmaterial(Tcolor3<float> amb, Tcolor3<float> dif, Tcolor3<float> spc, int n) {
		ambient =amb;
		diffuse = dif;
		specular = spc;
		nPhong = n;
	}
	Tmaterial() {
		ambient.assign(0.f, 0.f, 0.f);
		diffuse.assign(0.0f, 0.0f, 0.0f);
		specular.assign(0.0f, 0.0f, 0.0f);
		nPhong = 0;
	}

	void setAmbient(Tcolor3<float> val) {
		ambient = val;
	}
	void setDiffuse(Tcolor3<float> val) {
		diffuse = val;
	}
	void setSpecular(Tcolor3<float> val) {
		specular = val;
	}
	void setPhongN(int val ) {
		nPhong = val;
	}

	Tcolor3<float> getAmbient() {
		return(ambient);
	}
	Tcolor3<float> getDiffuse() {
		return(diffuse);
	}
	Tcolor3<float> getSpecular() {
		return(specular);
	}
	int getPhongN() {
		return(nPhong);
	}

	Tmaterial& operator = (const Tmaterial& m) {
		ambient = m.ambient;
		diffuse = m.diffuse;
		specular = m.specular;
		nPhong = m.nPhong;
		return(*this);
	}
};
//=======================================================================
class Tobject {
    int nTri;         /* num of face */
    int nVert;         /* num of vertex */
	int nTexCoord;		//テクスチャ座標数
	Tmaterial material;
    Ttriangle *tri;     /* pointer to face */
    Tvertex *vtx;
	Tvector3<float>	*texCoord; //テクスチャ座標
	bool	smoothNormal; //法線を補間するかどうかのフラグ

	Tvector3<float> box[2];
	Tvector3<float> center;
	float			radius;

	vector<vector<int>> faceTriangleIndexList;

	bool	textured;

public:

	Tobject() {
		nVert = 0;
		nTri = 0;
		tri = NULL;
		vtx = NULL;
//		ref.assign(0, 0, 0);
		//id = 0;
		box[0].assign(0, 0, 0);
		box[1].assign(0, 0, 0);
		center.assign(0, 0, 0);
		radius = 0.0;
		//texAddress = NULL;
		textured = false;
		nTexCoord = 0;
		texCoord = NULL;
		smoothNormal = false;
	}
	Tobject(int n_vertex, int n_tri) {
		nVert = n_vertex;
		nTri = n_tri;
		vtx = new Tvertex[nVert];
		tri = new Ttriangle[nTri];
		//ref.assign(0, 0, 0);
		//id = 0;
		box[0].assign(0, 0, 0);
		box[1].assign(0, 0, 0);
		center.assign(0, 0, 0);
		radius = 0.0;
		nTexCoord = 0;
		texCoord = NULL;
		//texAddress = NULL;
		textured = false;
	}
	Tobject(FILE *fp) {
		//texAddress = NULL;
		textured = false;
		nTexCoord = 0;
		texCoord = NULL;
		readData(fp);
		calBoundingBox();
	}
	~Tobject() {
		delete [] vtx;
		delete [] tri;
		if(texCoord != NULL) delete [] texCoord;
	}

	int getNumVertex() {
		return(nVert);
	}
	int getNumTriangle() {
		return(nTri);
	}
	Tmaterial& getMaterial() {
		return(material);
	}
	Tvector3<float>& getMinimumPoint() {
		return(box[0]);
	}
	Tvector3<float>& getMaximumPoint() {
		return(box[1]);
	}
	Tvector3<float>& getCenter() {
		return(center);
	}
	float getBoundingSphereRadius() {
		return(radius);
	}
	void setNumVertex(int val) {
		nVert = val;
	}
	void setNumTriangle(int val) {
		nTri = val;
	}
	void setMaterial(Tmaterial& m) {
		material = m;
	}
	void setVertex(int n, Tvertex& v) {
		if(n < 0 || n >= nVert) return;
		vtx[n] = v;
	}
	void setTriangle(int n, Ttriangle& t) {
		if(n < 0 || n >= nTri) return;
		tri[n] = t;
	}

	bool isTextured() {
		return(textured);
	}

	bool isSmoothNormal() {
		return(smoothNormal);
	}
	void setSmoothNormal(bool val) {
		smoothNormal = val;
	}

	//void setTextureAddress(Ttexturef *tex) {
	//	texAddress = tex;
	//}
	//Ttexturef *getTextureAddress() {
	//	return(texAddress);
	//}

	Tvertex* getVertexAddress(int i) {
		return(&vtx[CLAMP(0, i, nVert-1)]);
	}
	Tvector3<float>* getTexCoordAddress(int i) {
		return(&(texCoord[i]));
	}
	Ttriangle* getTriangleAddress(int i)  {
		return(&tri[CLAMP(0, i, nTri-1)]);
	}

	Tobject& operator = ( Tobject& b )
	{
		this->nTri = b.nTri;
		this->nVert = b.nVert;
//		this->ref = b.ref;
		this->material = b.material;
		this->box[0] = b.box[0];
		this->box[1] = b.box[1];
		this->center = b.center;
		this->radius = b.radius;
//		this->texAddress = b.texAddress;
		this->textured = b.textured;

		if(this->vtx != NULL) delete [] this->vtx;
		this->vtx = new Tvertex[b.nVert];
		for(int i=0; i<b.nVert; i++)
			this->vtx[i] = b.vtx[i];

		if(this->texCoord != NULL) delete [] this->texCoord;
		this->texCoord= new Tvector3<float>[b.nTexCoord];
		for(int i=0; i<b.nTexCoord; i++)
			this->texCoord[i] = b.texCoord[i];

		if(this->tri != NULL) delete [] this->tri;
		this->tri = new Ttriangle[b.nTri];
		for(int i=0; i<b.nTri; i++) {
			this->tri[i] = b.tri[i];
			int iv0, iv1, iv2;
			iv0 = b.tri[i].getVertexAddress(0) - b.getVertexAddress(0);
			iv1 = b.tri[i].getVertexAddress(1) - b.getVertexAddress(0);
			iv2 = b.tri[i].getVertexAddress(2) - b.getVertexAddress(0);
			this->tri[i].setVertexAddress(
							this->getVertexAddress(iv0),
							this->getVertexAddress(iv1),
							this->getVertexAddress(iv2));
		}


		return *this;
	}

	vector<vector<int>>& getFaceTriangleIndexList() {
		return(faceTriangleIndexList);
	}

	void draw();
	void drawWithNormal();
	void drawNormal(float length = 1.0);
	void calBoundingBox();
	void translate(Tvector3<float>& t);
	void scale(Tvector3<float>& s);
	void rotateX(float st, float ct);
	void rotateY(float st, float ct);
	void rotateZ(float st, float ct);
	void readData(FILE *fp);
	void calVertexNormal();
};
//=======================================================================
#endif