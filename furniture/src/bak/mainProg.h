#ifndef MAIN_PROG_H__
#define MAIN_PROG_H__

#include "icg.h"

class MainProg {
public:

	TglutCamera		cam;
	TparallelLight	light;
	TobjectManager	objects;
	Ttexturef		texture;

	Tfog			fog;
	TrealSky		sky;
	TskyDome		sky_dome;

	void init(Tvector2<int> window_size);
	void draw();

};


#endif
