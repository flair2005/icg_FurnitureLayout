//#pragma once
#ifndef __OBJ_LOADER_H__
#define __OBJ_LOADER_H__

#include "doba-cg.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>


class ObjLoader {
public:

	ObjLoader()
	{
	}

	~ObjLoader()
	{
	}

	bool load( const char * filename,
				 std::string* group_name,
				 std::vector<Tvector3<float>>* vertex_list,
				 std::vector<Tvector3<float>>* normal_list,
				 std::vector<Tvector2<float>>* tex_coord_list,
				 std::vector<vector<int>>*     face_vertex_list,
				 std::vector<vector<int>>*     face_normal_list,
				 std::vector<vector<int>>*     face_tex_coord_list);
private:
	std::string getExtension(const std::string &filename) {
		std::string::size_type t = filename.rfind('.');
		return (t == std::string::npos) ? "" : filename.substr(t+1);
	}

	void readGroupName( std::istringstream &is , std::string* groupName)
	{
		is >> *groupName;
	}

	void readMaterial( std::istringstream &is, std::string* materialName )
	{
		is >> *materialName;
	}

	void readMaterialFile( std::istringstream &is )
	{
		std::cerr << "ObjLoader::readMaterialFile: not implemented" << std::endl;
	}

	void readTexcoord( std::istringstream &is, Tvector2<float>* uv )
	{
		is >> uv->x >> uv->y;
	}

	void readNormal( std::istringstream &is, Tvector3<float> *n )
	{
		is >> n->x >> n->y >> n->z;
	}

	void readVertex( std::istringstream &is, Tvector3<float> *p )
	{
		is >> p->x >> p->y >> p->z;
	}

	void readFace(	std::istringstream &is, 
					std::vector<int>* faceVertex, 
					std::vector<int>* faceTexture ,
					std::vector<int>* faceNormal )
	{

		while (! is.eof()) {
			int vert_idx;
			int tex_idx;
			int norm_idx;

			if (! getInt( &vert_idx, is ) )
			{
				std::cerr << "ObjLoader::readFace: invalid vertex index" << std::endl;
				return;
			}

			faceVertex->push_back( vert_idx-1 );

			if (! getSlash( is ) ) continue;

			if ( getInt( &tex_idx, is ) )
	            faceTexture->push_back( tex_idx-1 );
			
			if (! getSlash( is ) ) continue;

			if (isspace(is.get())) {
				skipSpace( is );
				skipComment( is );
				std::cerr << "3rd continue" << std::endl;
				continue;
			}
			is.unget();

			if ( getInt( &norm_idx, is ) )
				faceNormal->push_back( norm_idx-1 );
			
			skipSpace( is );
			skipComment( is );
		}
	}
	
	std::string getToken( std::istringstream &is )
	{
		std::string token;
		is >> token;
		return token;
	}

	bool getInt( int *pIdx, std::istringstream &is )
	{
		skipSpace( is );

		std::string digit;

		int length = is.tellg();

		char c = is.get();
		if (! isdigit(c) ) {
			is.unget();
			//std::cerr << "ObjLoader::getInt: non digit: " << c << std::endl;
			return false;
		}

		while (isdigit(c)) {
			digit.push_back( c );
			c = is.get();
		}

		if (c == '.') {
			is.seekg(length);
			//std::cerr << "ObjLoader::getInt: floating point not allowed" << std::endl;
			return false;
		}

		is.unget();

		*pIdx = atoi(digit.c_str());

		return true;
	}

	bool getSlash( std::istringstream &is )
	{
		skipSpace( is );

		char c = is.get();
		if (c == '/') return true;
		is.unget();
		return false;
	}

	void skipSpace( std::istringstream &is )
	{
		char c = is.get();
		while (isspace(c)) c = is.get();
		is.unget();
	}

	void skipComment( std::istringstream &is )
	{
		char c = is.get();
		if (c == '#')
			is.seekg(ios::end);
		else
			is.unget();
	}
};

#endif