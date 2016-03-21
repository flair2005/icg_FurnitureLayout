#ifndef __OBJECT_MANAGER_H__
#define __OBJECT_MANAGER_H__

#include "doba-cg.h"
#include "object.h"

//=======================================================================
#define NUM_OBJ_FILE 100
//=======================================================================
class TobjectManager {
	Tobject	*object;
	int		numObject;
	int		objectBufferSize;
	int		numTotalVert;
	Tvector3<float>  bbox[2];

public:

	TobjectManager() {
		object = NULL;
		setNumObject(0);
		setNumTotalVertex(0);
		objectBufferSize = 0;
	}
	~TobjectManager() {
		if(object != NULL) delete [] object;
	}
	void setNumObject(int n) {
		numObject = n;
	}
	void setNumTotalVertex(int n) {
		numTotalVert = n;
	}
	void setBoundingBox(const Tvector3<float>& bmin, const Tvector3<float>& bmax) {
		bbox[0] = bmin;
		bbox[1] = bmax;
	}


	int getNumObject() {
		return(numObject);
	}
	int getNumTotalVertex() {
		return(numTotalVert);
	}

	Tobject* getObjectAddress(int id) {
		return(&(object[id]));
	}
	Tvector3<float>& getMinimumPoint() {
		return(bbox[0]);
	}
	Tvector3<float>& getMaximumPoint() {
		return(bbox[1]);
	}

	void createObject(int n) {
		int n_old = numObject;
		numObject += n;
		if(objectBufferSize < numObject) {
			objectBufferSize = numObject*2;
			Tobject *o = new Tobject[objectBufferSize];
			if(n_old > 0) {
				for(int i=0; i<n_old; i++) o[i] = object[i];
				delete [] object;
			}
			object = o;
		}
	}

	void addObject(Tobject& obj) {
		createObject(1);
		object[numObject-1] = obj;
	}

	void deleteObject(int id) {
		for(int i=id; i<numObject-1; i++)
			object[i] = object[i+1];
		numObject--;
	}

	void calBoundingBox();

	bool readObjectDataFromFile(FILE *fp);
	bool readObjectFromObjFile(const char *filename, bool invert_face_direction = false);
	void shadeObjects(TparallelLight& light);
};
//=======================================================================
#endif