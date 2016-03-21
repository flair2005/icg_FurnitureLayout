#include "objLoader.h"

bool ObjLoader::load( const char * filename,
					 std::string* group_name,
					 std::vector<Tvector3<float>>* vertex_list,
					 std::vector<Tvector3<float>>* normal_list,
					 std::vector<Tvector2<float>>* tex_coord_list,
					 std::vector<vector<int>>*     face_vertex_list,
					 std::vector<vector<int>>*     face_normal_list,
					 std::vector<vector<int>>*     face_tex_coord_list)
{

	if (getExtension(filename) != "obj") {
		std::cerr << "ObjLoader::loader: error: invalid extention" << std::endl;
		return false;
	}

	//FILE *fp = fopen( filename, "r" );
	std::ifstream ifp( filename );
	if (! ifp) {
		cerr << "ObjLoader::loader: error: cannot open file: " << filename << endl;
		return false;
	}

	while ( ! ifp.eof() ) {
		std::string line;
		std::getline( ifp, line );
		std::istringstream is(line);

		if (line[0] == '#') continue;	// skip comment

		std::string entryName = getToken( is );

		std::string material_name;
		Tvector3<float> p, n;
		Tvector2<float> t;
		std::vector<int> vlist, nlist, tlist;

		
		if (entryName == "g") {

			readGroupName( is, group_name );

			//printf("%s\n", group_name->c_str());

		} else if (entryName == "vn") {

			readNormal( is, &n );
			normal_list->push_back(n);

			//printf("vn %f, %f, %f\n", n.x, n.y, n.z);

		} else if (entryName == "vt") {

			readTexcoord( is, &t );
			tex_coord_list->push_back(t);

			//printf("vt %f, %f\n", t.x, t.y);

		} else if (entryName == "v") {

			readVertex( is,  &p);
			vertex_list->push_back(p);

			//printf("v %f, %f, %f\n", p.x, p.y, p.z);

		} else if (entryName == "f") {

			vlist.clear();
			tlist.clear();
			nlist.clear();

			readFace( is, &vlist, &tlist, &nlist);

			face_vertex_list->push_back(vlist);
			face_tex_coord_list->push_back(tlist);
			face_normal_list->push_back(nlist);

			//printf("f ");
			//for(int i=0; i<vlist.size(); i++) {
			//	printf(" %d ", vlist.at(i));
			//}
			//printf("\n");


		} else if (entryName == "usemtl") {

			readMaterial( is, &material_name );

		} else if (entryName == "mtllib") {

			readMaterialFile( is );

		}

	}

	//ifp.close();

	return true;
}
