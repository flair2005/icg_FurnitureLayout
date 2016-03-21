#include"calcAverageIlluminance.h"

vector<Tvector3<float>> livingFloor;

float linerInterpolationTable(vector<vector<float>>& fluxTable, int i_z, int i, vector<Tvector3<float>>& tablePoint, float heightMesh, float widthMesh, vector<vector<Tvector2<int>>>& point);
float calcTableArea(vector<vector<float>>& fluxTable, int i_z,int i, vector<vector<Tvector2<int>>>& point);

void saveImage(vector<float>& table, char* filename)
{
	Timage image(mesh_size, mesh_size, 3);
	Tcolor4<unsigned char> c;
	for (int i = 0; i < mesh_size; i++){
		for (int j = 0; j < mesh_size; j++){
			c.r = (unsigned char)CLAMP(0, (table[i*mesh_size + j] / table[mesh_size*mesh_size - 1]) * 255, 255);
			c.g = c.b = c.r;
			image.set(i, j, c);
		}
	}
	image.save(filename);
	cout << "save完了です" << endl;
}


float pLightFlux_ij(Tvector3<float>& p, vector<PointLight>& pointLight, vector<SpotLight>& spotLight)
{
	float lightFlux = 0;

	if (p.x < livingFloor[0].x  || p.x > livingFloor[3].x  || p.y < livingFloor[0].y  || p.y > livingFloor[1].y ){
		return 0;
	}

	//pointLightのときの光束
	for (int k = 0; k < pointLight.size(); k++){
		Tvector3<float> lightPosition = pointLight[k].getPosition() / 100.0;
		Tvector3<float> dif_living = p - lightPosition;
		float r2_living = dif_living.lengthSquared();
		dif_living = dif_living.normalize();
		float co_living = dif_living.dot(Tvector3<float>(0.0, 0.0, -1.0));
		lightFlux += pointLight[k].getLuminosity()*co_living / r2_living;
	}
	float co_theta = 1;
	//spotLightのときの光束
	for (int k = 0; k < spotLight.size(); k++){
		Tvector3<float> lightPosition = spotLight[k].getPosition() / 100.0;
		Tvector3<float> dif_living = p - lightPosition;
		float r2_living = dif_living.lengthSquared();
		float co_living = dif_living.normalize().dot(Tvector3<float>(0.0, 0.0, -1.0));
		for (int i = 0; i < spotLight[k].getDirectivity(); i++){
			co_living *= co_living;
		}
		//後で変更したい
		//for (int i = 0; i < spotLight[k].getDirectivity(); i++){
		//	co_theta *= spotLight[k].get
		//}
		lightFlux += spotLight[k].getLuminosity()*co_living*co_theta / r2_living;
	} 

	return lightFlux;
	//return 1;

}
//テーブルを作成
void makeLightFluxTable(vector<vector<float>>& FluxTable, float table_height, vector<PointLight>& pointLight, vector<SpotLight>& spotLight)
{
	//living_heightは100で割ったメートル単位の値を入れてるよ
	livingFloor.resize(4);

	livingFloor[0] = Tvector3<float>(-6.55, -4.7, table_height);
	livingFloor[1] = Tvector3<float>(-6.55, 4.7, table_height);
	livingFloor[2] = Tvector3<float>(6.55, 4.7, table_height);
	livingFloor[3] = Tvector3<float>(6.55, -4.7, table_height);

	//回転のメッシュの細かさ
	float zrot;
	float zrot_mesh = 720 / (float)zrot_size;

	FluxTable.resize(zrot_size); 
	vector<Tvector3<float>> boundingTemp,boundingFloor;
	boundingTemp.resize(4); boundingFloor.resize(4);

	std::cout << "テーブル作成中。。。" << endl;
	for (int z_mesh_k = 0; z_mesh_k < zrot_size; z_mesh_k++){
		FluxTable[z_mesh_k].resize(mesh_size*mesh_size);
		zrot = -360 + zrot_mesh*z_mesh_k;
		zrot = TO_RAD(zrot);
		float x_max=INT_MIN, x_min=INT_MAX, y_max=INT_MIN, y_min=INT_MAX;
		for (int i = 0; i < 4; i++){
			boundingTemp[i].x = cos(-zrot)*livingFloor[i].x - sin(-zrot)*livingFloor[i].y;
			boundingTemp[i].y = sin(-zrot)*livingFloor[i].x + cos(-zrot)*livingFloor[i].y;
			boundingTemp[i].z = livingFloor[i].z;
		}
		
		for (int i = 0; i < 4; i++){
			if (x_max < boundingTemp[i].x){ x_max = boundingTemp[i].x; }
			if (x_min > boundingTemp[i].x){ x_min = boundingTemp[i].x; }
			if (y_max < boundingTemp[i].y){	y_max = boundingTemp[i].y; }
			if (y_min > boundingTemp[i].y){	y_min = boundingTemp[i].y; }
		}	

		boundingTemp[0] = Tvector3<float>(x_min, y_max, table_height);
		boundingTemp[1] = Tvector3<float>(x_max, y_max, table_height);
		boundingTemp[2] = Tvector3<float>(x_max, y_min, table_height);
		boundingTemp[3] = Tvector3<float>(x_min, y_min, table_height);

		for (int i = 0; i < 4; i++){
			boundingFloor[i].x = cos(zrot)*boundingTemp[i].x - sin(zrot)*boundingTemp[i].y;
			boundingFloor[i].y = sin(zrot)*boundingTemp[i].x + cos(zrot)*boundingTemp[i].y;
			boundingFloor[i].z = boundingTemp[i].z;
		}
		
		//boundingFloorの横と縦
		float height = (boundingFloor[0] - boundingFloor[1]).length();
		float width = (boundingFloor[0] - boundingFloor[3]).length();
		
		//mesh幅
		float mesh_height = height / (float)mesh_size;
		float mesh_width = width / (float)mesh_size;

		Tvector3<float> p0p1 = boundingFloor[1] - boundingFloor[0];
		Tvector3<float> p0p3 = boundingFloor[3] - boundingFloor[0];

		for (int j = 0; j < mesh_size; j++){
			Tvector3<float> p = boundingFloor[0] + (0 * mesh_height / height)*p0p1 + (j*mesh_width / width)*p0p3;
			FluxTable[z_mesh_k][j*mesh_size + 0] = pLightFlux_ij(p,pointLight,spotLight)*mesh_height*mesh_width;
			for (int i = 1; i < mesh_size; i++){
				p = boundingFloor[0] + (i*mesh_height / height)*p0p1 + (j*mesh_width / width)*p0p3;
				FluxTable[z_mesh_k][j*mesh_size + i] = FluxTable[z_mesh_k][j*mesh_size + (i - 1)] 
														+ pLightFlux_ij(p,pointLight,spotLight)*mesh_height*mesh_width;
			}
		}
		for (int i = 0; i < mesh_size; i++){
			for (int j = 1; j < mesh_size; j++){
				FluxTable[z_mesh_k][j*mesh_size + i] += FluxTable[z_mesh_k][(j - 1)*mesh_size + i];
			}
		}

		////----------------照度確認用画像に保存---------------------
		//float maxIll = INT_MIN; int maxi, maxj;
		//Timage image(mesh_size, mesh_size, 3);
		//Tcolor4<unsigned char> c;
		//for (int i = 0; i < mesh_size; i++){
		//	for (int j = 0; j < mesh_size; j++){
		//		Tvector3<float> p = boundingFloor[0] + (i*mesh_height / height)*p0p1 + (j*mesh_width / width)*p0p3;
		//		if (maxIll < pLightFlux_ij(p, pointLight, spotLight)){
		//			maxIll = pLightFlux_ij(p, pointLight, spotLight);
		//			maxi = i; maxj = j;
		//		}
		//		c.r = (unsigned char)CLAMP(0, (pLightFlux_ij(p, pointLight, spotLight) / 393.737) * 255, 251);
		//		c.g = (unsigned char)CLAMP(0, (pLightFlux_ij(p, pointLight, spotLight) / 393.737) * 255, 208);
		//		c.b = (unsigned char)CLAMP(0, (pLightFlux_ij(p, pointLight, spotLight) / 393.737) * 255, 29);
		//		image.set(i, j, c);
		//	}
		//}
		//cout << "maxIll = " << maxIll << " | maxi = " << maxi << " | maxj = " << maxj << endl;
		//c.g = 1; c.b = 15; c.r = 41;
		//image.set(maxi, maxj, c);
		//image.save("illuminance.bmp");
		//cout << "照度save完了です" << endl;
		//system("PAUSE");
		////----------------------------------------------------------
	}
	std::cout << "テーブル完了" << endl;



}

//平均照度を測る
float calcAverageIlluminance(vector<vector<float>>& fluxTable, float table_height, int tableNum ,Furniture& objects)
{
	livingFloor[0] = Tvector3<float>(-6.55, -4.7, table_height);
	livingFloor[1] = Tvector3<float>(-6.55, 4.7, table_height);
	livingFloor[2] = Tvector3<float>(6.55, 4.7, table_height);
	livingFloor[3] = Tvector3<float>(6.55, -4.7, table_height);


	//回転のメッシュの細かさ
	float zrot_mesh = 720 / (float)zrot_size;
	float max_zrot, min_zrot, x_max = INT_MIN, x_min = INT_MAX, y_max = INT_MIN, y_min = INT_MAX;
	int i_max, i_min;
	vector<Tvector3<float>> maxLivingFloor, minLivingFloor, maxBoundingTemp, minBoundingTemp, table, tableTemp, maxTable, minTable;
	maxLivingFloor.resize(4); minLivingFloor.resize(4); maxBoundingTemp.resize(4); minBoundingTemp.resize(4); table.resize(4); tableTemp.resize(4); maxTable.resize(4); minTable.resize(4);

	//testでlivingTableの座標を考える。
	table[0] = Tvector3<float>( objects.obj[tableNum].getMinimumPoint().x/100.0, objects.obj[tableNum].getMaximumPoint().y/100.0, table_height);
	table[1] = Tvector3<float>( objects.obj[tableNum].getMaximumPoint().x/100.0, objects.obj[tableNum].getMaximumPoint().y/100.0, table_height);
	table[2] = Tvector3<float>( objects.obj[tableNum].getMaximumPoint().x/100.0, objects.obj[tableNum].getMinimumPoint().y/100.0, table_height);
	table[3] = Tvector3<float>( objects.obj[tableNum].getMinimumPoint().x/100.0, objects.obj[tableNum].getMinimumPoint().y/100.0, table_height);

	float tableArea = (objects.obj[tableNum].getMaximumPoint().x - objects.obj[tableNum].getMinimumPoint().x)*(objects.obj[tableNum].getMaximumPoint().y - objects.obj[tableNum].getMinimumPoint().y) / 10000.0;

	float zrot = objects.zrot[tableNum];
	for (int i = 0; i < 4; i++){ livingFloor[i].z = table_height; }
	//小数点(decinal)
	float decinal = zrot - (int)zrot;
	if (zrot < 0){
		zrot = -zrot; zrot = (int)zrot % 360; zrot = -zrot;	zrot = zrot + decinal;
	}
	else{
		zrot = (int)zrot % 360;	zrot = zrot + decinal;
	}
	zrot = TO_RAD(zrot);

	for (int i = 0; i < 4; i++){
		tableTemp[i].x = cos(0)*table[i].x - sin(0)*table[i].y;
		tableTemp[i].y = sin(0)*table[i].x + cos(0)*table[i].y;
		tableTemp[i].z = table[i].z;
	}
	for (int i = 0; i < 4; i++){ table[i] = tableTemp[i] + objects.trans[tableNum] / 100.0; }
	
	zrot = TO_DEG(zrot);
	for (int i = 0; i < zrot_size; i++){
		max_zrot = -360 + i*zrot_mesh;
		min_zrot = -360 + (i - 1)*zrot_mesh;
		if (zrot < max_zrot && zrot > min_zrot) { i_max = i; i_min = i - 1; break; }
	}
	float gamma = zrot - min_zrot;

	max_zrot = TO_RAD(max_zrot);
	min_zrot = TO_RAD(min_zrot);

	for (int i = 0; i < 4; i++){
		maxBoundingTemp[i].x = cos(-max_zrot)*livingFloor[i].x - sin(-max_zrot)*livingFloor[i].y;
		maxBoundingTemp[i].y = sin(-max_zrot)*livingFloor[i].x + cos(-max_zrot)*livingFloor[i].y;
		maxBoundingTemp[i].z = livingFloor[i].z;

		minBoundingTemp[i].x = cos(-min_zrot)*livingFloor[i].x - sin(-min_zrot)*livingFloor[i].y;
		minBoundingTemp[i].y = sin(-min_zrot)*livingFloor[i].x + cos(-min_zrot)*livingFloor[i].y;
		minBoundingTemp[i].z = livingFloor[i].z;
	}
	for (int i = 0; i < 4; i++){
		if (x_max < maxBoundingTemp[i].x){ x_max = maxBoundingTemp[i].x; }
		if (x_min > maxBoundingTemp[i].x){ x_min = maxBoundingTemp[i].x; }
		if (y_max < maxBoundingTemp[i].y){ y_max = maxBoundingTemp[i].y; }
		if (y_min > maxBoundingTemp[i].y){ y_min = maxBoundingTemp[i].y; }
	}
	maxLivingFloor[0] = Tvector3<float>(x_min, y_max, table_height);
	maxLivingFloor[1] = Tvector3<float>(x_max, y_max, table_height);
	maxLivingFloor[2] = Tvector3<float>(x_max, y_min, table_height);
	maxLivingFloor[3] = Tvector3<float>(x_min, y_min, table_height);

	x_max = INT_MIN; x_min = INT_MAX; y_max = INT_MIN; y_min = INT_MAX;
	for (int i = 0; i < 4; i++){
		if (x_max < minBoundingTemp[i].x){ x_max = minBoundingTemp[i].x; }
		if (x_min > minBoundingTemp[i].x){ x_min = minBoundingTemp[i].x; }
		if (y_max < minBoundingTemp[i].y){ y_max = minBoundingTemp[i].y; }
		if (y_min > minBoundingTemp[i].y){ y_min = minBoundingTemp[i].y; }
	}
	minLivingFloor[0] = Tvector3<float>(x_min, y_max, table_height);
	minLivingFloor[1] = Tvector3<float>(x_max, y_max, table_height);
	minLivingFloor[2] = Tvector3<float>(x_max, y_min, table_height);
	minLivingFloor[3] = Tvector3<float>(x_min, y_min, table_height);

	//注Heightが横、widthが縦になった
	float maxHeightMesh = (maxLivingFloor[0] - maxLivingFloor[1]).length() / (float)mesh_size;
	float maxWidthMesh = (maxLivingFloor[0] - maxLivingFloor[3]).length() / (float)mesh_size;

	float minHeightMesh = (minLivingFloor[0] - minLivingFloor[1]).length() / (float)mesh_size;
	float minWidthMesh = (minLivingFloor[0] - minLivingFloor[3]).length() / (float)mesh_size;


	//改めてFloor[0]を原点にした座標系を考える
	vector<Tvector3<float>> maxTempLivingFloor, minTempLivingFloor;
	maxTempLivingFloor.resize(4); minTempLivingFloor.resize(4);
	for (int i = 0; i < 4; i++){
		maxTable[i] = table[i] - maxLivingFloor[0];
		maxTempLivingFloor[i] = maxLivingFloor[i] - maxLivingFloor[0];

		minTable[i] = table[i] - minLivingFloor[0];
		minTempLivingFloor[i] = minLivingFloor[i] - minLivingFloor[0];
	}

	vector<vector<Tvector2<int>>> max_point, min_point;
	max_point.resize(4); min_point.resize(4);


	for (int i = 0; i < 4; i++){
		max_point[i].resize(4);
		max_point[i][0].x = abs((int)(maxTable[i].x / maxHeightMesh)) ;	max_point[i][0].y = abs((int)(maxTable[i].y / maxWidthMesh)) ;
		max_point[i][1].x = max_point[i][0].x + 1;	max_point[i][1].y = max_point[i][0].y;
		max_point[i][2].x = max_point[i][0].x + 1;	max_point[i][2].y = max_point[i][0].y + 1;
		max_point[i][3].x = max_point[i][0].x;	max_point[i][3].y = max_point[i][0].y + 1;

		min_point[i].resize(4);
		min_point[i][0].x = abs((int)(minTable[i].x / minHeightMesh));	min_point[i][0].y = abs((int)(minTable[i].y / minWidthMesh));
		min_point[i][1].x = min_point[i][0].x + 1;	min_point[i][1].y = min_point[i][0].y;
		min_point[i][2].x = min_point[i][0].x + 1;	min_point[i][2].y = min_point[i][0].y + 1;
		min_point[i][3].x = min_point[i][0].x;	min_point[i][3].y = min_point[i][0].y + 1;
	}

	float max_interpolationArea;
	max_interpolationArea =
		linerInterpolationTable(fluxTable, i_max, 0, maxTable, maxHeightMesh, maxWidthMesh, max_point)
		- linerInterpolationTable(fluxTable, i_max, 1, maxTable, maxHeightMesh, maxWidthMesh, max_point)
		+ linerInterpolationTable(fluxTable, i_max, 2, maxTable, maxHeightMesh, maxWidthMesh, max_point)
		- linerInterpolationTable(fluxTable, i_max, 3, maxTable, maxHeightMesh, maxWidthMesh, max_point);
	float min_interpolationArea;
	min_interpolationArea =
		linerInterpolationTable(fluxTable, i_min, 0, minTable, minHeightMesh, minWidthMesh, min_point)
		- linerInterpolationTable(fluxTable, i_min, 1, minTable, minHeightMesh, minWidthMesh, min_point)
		+ linerInterpolationTable(fluxTable, i_min, 2, minTable, minHeightMesh, minWidthMesh, min_point)
		- linerInterpolationTable(fluxTable, i_min, 3, minTable, minHeightMesh, minWidthMesh, min_point);
	float all_interpolationArea ;
	all_interpolationArea = gamma*max_interpolationArea + (1 - gamma)*min_interpolationArea;
	//std::cout << "全体線形補間したよ = " << all_interpolationArea << endl;
	//std::cout << "平均照度　=　Φ/Ｓ = " << all_interpolationArea / tableArea << endl;
	float illuminance = all_interpolationArea / tableArea;

	return illuminance;
}
//(注)point[机の頂点][机の頂点の周りの頂点]
//(ex)point[0][(0～3)]
//	（０）・    　・（１）
//　　　　　　[0]　　　　　　　　　 [1]
//　　　　　　○ーーーーーーーーーー○
//　　　　　　｜                    ｜
//　（３）・　｜　・（２）　　　　　｜　
//　　　　　　｜　　　　　　　　　　｜
//　　　　　　｜　　　　　　　　　　｜
//　　　　　　｜　　　　　　　　　　｜
//　　　　　　｜　　　　　　　　　　｜
//　　　　　　｜　　　　　　　　　　｜
//　　　　　　○ーーーーーーーーーー○
//            [3]                   [2]
float linerInterpolationTable(vector<vector<float>>& fluxTable, int i_z, int i,vector<Tvector3<float>>& tablePoint, float heightMesh, float widthMesh, vector<vector<Tvector2<int>>>& point)
{	
	float alpha, beta;
	alpha = abs(tablePoint[i].x / heightMesh) - abs((int)(tablePoint[i].x / heightMesh));
	beta = abs(tablePoint[i].y / widthMesh) - abs((int)(tablePoint[i].y / widthMesh));

	if (i_z >= 720){
		i_z = 719;
	}
	if (i_z <= -360){
		i_z = -360;
	}

	float linerTable;
	linerTable =
		  (1 - alpha)*(1 - beta)*fluxTable[i_z][point[i][0].y*mesh_size + point[i][0].x]
		+ alpha*(1 - beta)*fluxTable[i_z][point[i][1].y*mesh_size + point[i][1].x]
		+ alpha*beta*fluxTable[i_z][point[i][2].y*mesh_size + point[i][2].x]
		+ (1 - alpha)*beta*fluxTable[i_z][point[i][3].y*mesh_size + point[i][3].x];

	return linerTable;
}




