#ifndef __FURNITURE_H__
#define __FURNITURE_H__

#include "icg.h"
//�S�Ƌ�@=�@10
#define objectNum 7


struct Furniture
{
	TobjectManager obj;
	//�t�@�C�����ƃI�u�W�F�N�g�̖��O
	string file[objectNum], objname[objectNum];

	//�ړ��ʂƉ�]��
	vector<Tvector3<float>> trans;
	vector<float> zrot;

	//��]���z�̗v�f
	vector<Tvector3<float>> hopeCenter;
	vector<float> hopeRadius, hopeScalex, hopeScaley;
};

#endif