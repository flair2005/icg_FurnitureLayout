#ifndef __FURNITURE_H__
#define __FURNITURE_H__

#include "icg.h"
//全家具　=　10
#define objectNum 7


struct Furniture
{
	TobjectManager obj;
	//ファイル名とオブジェクトの名前
	string file[objectNum], objname[objectNum];

	//移動量と回転量
	vector<Tvector3<float>> trans;
	vector<float> zrot;

	//希望分布の要素
	vector<Tvector3<float>> hopeCenter;
	vector<float> hopeRadius, hopeScalex, hopeScaley;
};

#endif