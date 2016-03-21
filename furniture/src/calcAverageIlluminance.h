#ifndef __AVERAGE_ILLUMINANCE_H__

#include "icg.h"
#include "Furniture.h"
#include "myLight.h"
#include "constants.h"

//縦と横のメッシュの細かさ(初めは100)
#define mesh_size 200
//回転のメッシュの細かさ(初めは100)
#define zrot_size 720

float calcAverageIlluminance(vector<vector<float>>& fluxTable, float table_height, int tableNum, Furniture& objects);
void makeLightFluxTable(vector<vector<float>>& FluxTable, float table_height, vector<PointLight>& pointLight, vector<SpotLight>& spotLight);

#endif