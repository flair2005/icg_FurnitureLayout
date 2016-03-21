#include "record.h"


void recordBox(char* filename, TobjectManager& Wall,Furniture& objects)
{
	ofstream outputfile(filename);
	if (!outputfile){
		cerr << "ファイルオープン失敗" << endl;
		exit(1);
	}
	outputfile << objectNum + 1 << endl;
	outputfile << Wall[0].getMaximumPoint().x * 1.4 / 100.0 << " " << Wall[0].getMaximumPoint().y * 1.4 /100.0 << " "
		<< Wall[0].getMinimumPoint().x * 1.4 / 100.0 << " " << Wall[0].getMaximumPoint().y * 1.4 / 100.0 << " "
		<< Wall[0].getMinimumPoint().x * 1.4 / 100.0 << " " << Wall[0].getMinimumPoint().y * 1.4 / 100.0 << " "
		<< Wall[0].getMaximumPoint().x * 1.4 / 100.0 << " " << Wall[0].getMinimumPoint().y * 1.4 / 100.0 << " "
		<< (Wall[0].getMaximumPoint().z - Wall[0].getMinimumPoint().z) / 100.0 << " "
		<< WALLCOLOR.r << " " << WALLCOLOR.g << " " << WALLCOLOR.b << endl;
	for (int i = 0; i < objectNum; i++){
		vector <Tvector2<float>> pbox, pbox_temp; pbox.resize(4); pbox_temp.resize(4);
		pbox_temp[0] = Tvector2<float>(objects.obj[i].getMaximumPoint().x / 100.0, objects.obj[i].getMaximumPoint().y / 100.0);
		pbox_temp[1] = Tvector2<float>(objects.obj[i].getMinimumPoint().x / 100.0, objects.obj[i].getMaximumPoint().y / 100.0);
		pbox_temp[2] = Tvector2<float>(objects.obj[i].getMinimumPoint().x / 100.0, objects.obj[i].getMinimumPoint().y / 100.0);
		pbox_temp[3] = Tvector2<float>(objects.obj[i].getMaximumPoint().x / 100.0, objects.obj[i].getMinimumPoint().y / 100.0);

		for (int j = 0; j < 4; j++){
			pbox[j].x = cos(TO_RAD(objects.zrot[i]))*pbox_temp[j].x - sin(TO_RAD(objects.zrot[i]))*pbox_temp[j].y + objects.trans[i].x / 100.0;
			pbox[j].y = sin(TO_RAD(objects.zrot[i]))*pbox_temp[j].x + cos(TO_RAD(objects.zrot[i]))*pbox_temp[j].y + objects.trans[i].y / 100.0;
		}

		for (int k = 0; k < 4; k++){
			outputfile << pbox[k].x << " " << pbox[k].y << " ";
			//outputfile << "v " << pbox[k].x << " 0 " << pbox[k].y << endl; //objファイル出力用

		}
		//outputfile << "f " << i * 4 +1 << " " << i * 4 + 1 + 1 << " ";//objファイル出力用
		//outputfile << i * 4 + 2 + 1 << " " << i * 4 + 3 +1 << endl;//objファイル出力用
		outputfile << (objects.obj[i].getMaximumPoint().z - objects.obj[i].getMinimumPoint().z) / 100.0 << " ";
		if (objects.objname[i] == "coffee_table" || objects.objname[i] == "living_table"){
			outputfile << TABLECOLOR.r << " " << TABLECOLOR.g << " " << TABLECOLOR.b << endl;
		}
		else if (objects.objname[i] == "living_chair"){
			outputfile << DININGCHAIRCOLOR.r << " " << DININGCHAIRCOLOR.g << " " << DININGCHAIRCOLOR.b << endl;
		}
		else{
			outputfile << SOFACOLOR.r << " " << SOFACOLOR.g << " " << SOFACOLOR.b << endl;
		}
			
	}
	outputfile.close();
}

void lightRecord(char* filename, vector<PointLight>& pointLight, vector<SpotLight>& spotLight)
{
	ofstream outputfile(filename);
	if (!outputfile){
		cerr << "ファイルオープン失敗" << endl;
		exit(1);
	}
	outputfile << pointLight.size() + spotLight.size() << endl;
	for (int i = 0; i < spotLight.size(); i++){
		outputfile << spotLight[i].getPosition().x / 100.0 << " "
			<< spotLight[i].getPosition().y / 100.0 << " "
			<< spotLight[i].getPosition().z / 100.0 << " "
			<< spotLight[i].getDirection().x << " "
			<< spotLight[i].getDirection().y << " "
			<< spotLight[i].getDirection().z << " "
			<< spotLight[i].getIntensity() << " "
			<< spotLight[i].getDirectivity() << endl;
	}
	for (int i = 0; i < pointLight.size(); i++){
		outputfile << pointLight[i].getPosition().x / 100.0 << " "
			<< pointLight[i].getPosition().y / 100.0 << " "
			<< pointLight[i].getPosition().z / 100.0 << " "
			<< 0.0 << " " << 0.0 << " " << -1.0 << " "
			<< pointLight[i].getIntensity() << " " << 0 << endl;
	}

}


void recordFurnitureLayout(char* filename, Furniture& objects)
{
	ofstream ofs(filename);
	if (!ofs){
		cerr << "ファイルオープン失敗" << endl;
		system("PAUSE");
		exit(1);
	}
	ofs << "trans[armchair] = " << objects.trans[0] / 100.0 << "   zrot[0] = " << objects.zrot[0] << endl;
	ofs << "trans[sofa] = " << objects.trans[1] / 100.0 << "   zrot[1] = " << objects.zrot[1] << endl;
	ofs << "trans[coffee_table] = " << objects.trans[2] / 100.0 << "   zrot[2] = " << objects.zrot[2] << endl;
	ofs << "trans[armchair] = " << objects.trans[3] / 100.0 << "   zrot[3] = " << objects.zrot[3] << endl;
	ofs << "trans[living_table] = " << objects.trans[4] / 100.0 << "   zrot[4] = " << objects.zrot[4] << endl;
	ofs << "trans[living_chair] = " << objects.trans[5] / 100.0 << "   zrot[5] = " << objects.zrot[5] << endl;
	ofs << "trans[living_chair] = " << objects.trans[6] / 100.0 << "   zrot[6] = " << objects.zrot[6] << endl;
}
