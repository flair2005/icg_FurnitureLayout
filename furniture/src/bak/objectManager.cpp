
#include <string.h>
#include "objectManager.h"
#include "objLoader.h"

//----------------------------------------------------------------------
void TobjectManager::calBoundingBox() {
	Tvector3<float> bbox[2], box[2];

	bbox[0].assign(BIG_VALUE, BIG_VALUE, BIG_VALUE);
	bbox[1].assign(-BIG_VALUE, -BIG_VALUE, -BIG_VALUE);
	Tobject *obj = object;
	for(int i=0; i<numObject; i++) {
		box[0] = obj->getMinimumPoint();
		box[1] = obj->getMaximumPoint();
		for(int j=0; j<3; j++) {
			bbox[0][j] = MIN(bbox[0][j], box[0][j]);
			bbox[1][j] = MAX(bbox[1][j], box[1][j]);
		}
		obj++;
	}
	setBoundingBox(bbox[0], bbox[1]);
}

//----------------------------------------------------------------------
bool TobjectManager::readObjectDataFromFile(FILE *ffp) {

    FILE *fp;
    int    i,iend, nfile;
    int no, no1, no2, n, io1,io2;
    char comd[10];
    float u,v,w,tu,tv,tw;
    char filename[NUM_OBJ_FILE][80];
	char comment[1024];

    fscanf(ffp, "%d", &nfile);
    if(nfile > NUM_OBJ_FILE) {
        printf("set NUM_OBJ_FILE > %d in render.c\n", nfile);
        return(false);
    }

    int numObject = 0;
    for(i=0; i<nfile; i++) {
        fscanf(ffp,"%s",filename[i]);
        fp=fopen(filename[i],"r");
        if(fp == NULL) {
            fprintf(stderr,"Can't open the file: %s\n",filename[i]);
            return(false);
        }
        fscanf(fp,"%d%s",&no, comment);
        fclose(fp);
        numObject += no;
    }

	createObject(numObject);

	char buf[256];

    no1 = 0;
    for(n=0; n<nfile; n++) {

		printf("reading object file:%s\n",filename[n]);

		fp=fopen(filename[n],"r");
		if(fp == NULL) {
			printf("can't open the file %s.", filename[n]);
			break;
		}

		fgets(buf,256,fp);
		sscanf(buf,"%d",&no);
		no2 = no1+no;
		numTotalVert=0;

		for(i=no1; i<no2; i++) {
			object[i].readData(fp);
			numTotalVert += object[i].getNumVertex();
		}


	/*** CHANGE GEOMETRY ***/
		iend=1;
		while(iend) {
			fgets(buf,256,fp);
			sscanf(buf,"%s%d%d%f%f%f%f%f%f",comd,&io1,&io2
										  ,&u,&v,&w,&tu,&tv,&tw);
			io1--; io2--; io1 += no1; io2 += no1;
			if(!strcmp(comd,"END")) {
				iend=0;
			} else {
				if(!strcmp(comd, "MOVE"))
					for(i=io1; i<=io2; i++) object[i].translate(Tvector3<float>(u, v, w));
				else if(!strcmp(comd, "SCAL") || !strcmp(comd,"SCAL")) {
					for(i=io1; i<=io2; i++) {
						object[i].translate(Tvector3<float>(u, v, w));
						object[i].scale(Tvector3<float>(tu, tv, tw));
						object[i].translate(Tvector3<float>(-u, -v, -w));
					}
				} else if(!strcmp(comd, "ZREV"))
					for(i=io1; i<=io2; i++) {
						object[i].translate(Tvector3<float>(u, v, w));
						object[i].rotateZ(sin(TO_RAD(tu)), cos(TO_RAD(tu)));
						object[i].translate(Tvector3<float>(-u, -v, -w));
					}
				else if(!strcmp(comd, "YREV"))
					for(i=io1; i<=io2; i++) {
						object[i].translate(Tvector3<float>(u, v, w));
						object[i].rotateY(sin(TO_RAD(tu)), cos(TO_RAD(tu)));
						object[i].translate(Tvector3<float>(-u, -v, -w));
					}
				else if(!strcmp(comd, "XREV"))
					for(i=io1; i<=io2; i++) {
						object[i].translate(Tvector3<float>(u, v, w));
						object[i].rotateX(sin(TO_RAD(tu)), cos(TO_RAD(tu)));
						object[i].translate(Tvector3<float>(-u, -v, -w));
					}
			}
		}
		no1 = no2;
		fclose(fp);
	}
	for(i=0; i<numObject; i++) {
//		object[i].setID(i);
//		object[i].cal_normal();
		object[i].calVertexNormal();
		object[i].calBoundingBox();
	}

	calBoundingBox();

	return(true);
}
//----------------------------------------------------------------------
bool TobjectManager::readObjectFromObjFile(const char *filename, bool invert_face_direction) {
	std::string	group_name;
	vector<Tvector3<float>> vertex_list;
	vector<Tvector3<float>> normal_list;
	vector<Tvector2<float>> tex_coord_list;
	vector<vector<int>>     face_vertex_list;
	vector<vector<int>>     face_normal_list;
	vector<vector<int>>     face_tex_coord_list;


	ObjLoader objLoader;

	objLoader.load(	filename,
					&group_name,
					&vertex_list,
					&normal_list,
					&tex_coord_list,
					&face_vertex_list,
					&face_normal_list,
					&face_tex_coord_list);

	printf("group name: %s\n", group_name.c_str());
	//for(int i=0; i<vertex_list.size(); i++)
	//	printf("v %f, %f, %f\n", vertex_list.at(i).x, vertex_list.at(i).y, vertex_list.at(i).z);
	//for(int i=0; i<normal_list.size(); i++)
	//	printf("vn %f, %f, %f\n", normal_list.at(i).x, normal_list.at(i).y, normal_list.at(i).z);
	//for(int i=0; i<tex_coord_list.size(); i++)
	//	printf("vt %f, %f\n", tex_coord_list.at(i).x, tex_coord_list.at(i).y);

	//
	//for(int i=0; i<face_vertex_list.size(); i++) {
	//	printf("f ");
	//	for(int j=0; j<face_vertex_list.at(i).size(); j++) {
	//		printf(" %d ", face_vertex_list.at(i).at(j));
	//	}
	//	printf("\n");
	//}

	int n_vert = vertex_list.size();
	int n_tri = 0;

	for(int i=0; i<face_vertex_list.size(); i++) {
		n_tri += face_vertex_list.at(i).size() - 2;
	}

	printf("n_vert, n_tri: %d, %d\n", n_vert, n_tri);

	Tobject *obj = new Tobject(n_vert, n_tri);
	Tvertex v;
	Ttriangle tri;

	for(int i=0; i<vertex_list.size(); i++) {
		v.setPosition(vertex_list.at(i));
		v.setNormal(Tvector3<float>(0, 0, 0));
		obj->setVertex(i, v);
	}

	int k=0;
	for(int i=0; i<face_vertex_list.size(); i++) {
		if(face_vertex_list.at(i).size() >= 2) {
			int iv[3];
			iv[0] = face_vertex_list.at(i).at(0);
			iv[1] = face_vertex_list.at(i).at(1);
			iv[2] = face_vertex_list.at(i).at(2);
			if(invert_face_direction)
				tri.setVertexAddress(	obj->getVertexAddress(iv[2]),
										obj->getVertexAddress(iv[1]),
										obj->getVertexAddress(iv[0]));
			else
				tri.setVertexAddress(	obj->getVertexAddress(iv[0]),
										obj->getVertexAddress(iv[1]),
										obj->getVertexAddress(iv[2]));
			obj->setTriangle(k, tri);
			k++;
			for(int j=2; j<face_vertex_list.at(i).size()-1; j++) {
				iv[1] = face_vertex_list.at(i).at(j);
				iv[2] = face_vertex_list.at(i).at(j+1);				
				if(invert_face_direction)
					tri.setVertexAddress(	obj->getVertexAddress(iv[2]),
											obj->getVertexAddress(iv[1]),
											obj->getVertexAddress(iv[0]));
				else
					tri.setVertexAddress(	obj->getVertexAddress(iv[0]),
											obj->getVertexAddress(iv[1]),
											obj->getVertexAddress(iv[2]));
				obj->setTriangle(k, tri);
				k++;
			}
		}
	}

	Tmaterial material;
	material.setDiffuse(Tcolor3<float>(1., 1., 1.));
	obj->setMaterial(material);
	obj->calVertexNormal();
	obj->calBoundingBox();

	this->addObject(*obj);

	delete obj;

	calBoundingBox();

	return(true);
}

//----------------------------------------------------------------------
void TobjectManager::shadeObjects(TparallelLight& light) {
	for(int io=0; io<numObject; io++) {
		Tcolor3<float> ref = object[io].getMaterial().getDiffuse();
		for(int iv=0; iv<object[io].getNumVertex(); iv++) {
			Tvertex *v = object[io].getVertexAddress(iv);
			Tvector3<float> n = v->getNormal();
			float co = n.dot(light.getDirection());
			Tcolor3<float> color = co*ref*light.getColor();
			v->setColor(color);
		}
	}
}