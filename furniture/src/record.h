#ifndef __RECORD_H__
#define __RECORD_H__

#include "icg.h"
#include "constants.h"
#include "Furniture.h"
#include "myLight.h"

void recordBox(char* filename, TobjectManager& Wall, Furniture& objects);
void lightRecord(char* filename, vector<PointLight>& pointLight, vector<SpotLight>& spotLight);
void recordFurnitureLayout(char* filename, Furniture& objects);

#endif