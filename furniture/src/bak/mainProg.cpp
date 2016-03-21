#include "mainProg.h"


//----------------------------------------------------------------------
void MainProg::init(Tvector2<int> window_size)
{
	objects.load("data/25ground_s.obj", false);
//	objects.readObject("chapel_obj/chapel_obj.obj");
//	objects.readObject("data/arm.obj");
//	objects.readObject("data/bunny.txt");

//	objects.getObjectAddress(0)->invertVertexNormal();
	for(int i=0; i<objects.getObject(0).getNumVertex(); i++)
		objects.getObject(0).getVertex(i).setColor(Tcolor3<float>(1., 1., 1.));

	Tvector3<float> p = objects.getMinimumPoint();
	Tvector3<float> q = objects.getMaximumPoint();
	cam.init(q,
			 (p+q)/2.0,
			 0.1, 5000.0, TO_RAD(30.0), 0.0,
			 window_size);
	cam.setMoveScale(1.);

	light.setDirection(Tvector3<float>(1, 1, 1));
	light.setAmbient(Tcolor3<float>(0.5, 0.5, 0.5));
	//objects.shadeObjects(light);

	Timage image("ter_1C.bmp");
	texture.init(image.getWidth(), image.getHeight());
	texture.setPixels(image);
	texture.glInit();

	fog.init(0.005, Tcolor3<float>(0.001, 0.001, 0.001), Tcolor3<float>(0.2, 0.4, 0.6));
	fog.sendParamToGPU();
	sky.setIntensityScale(0.002);
	sky_dome.init(1000, 18, 36);
	sky_dome.updateColor(light.getDirection(), sky);
}
//----------------------------------------------------------------------
void MainProg::draw()
{
	cam.glCamera();

	//sky_dome.draw();

	fog.enable();

	texture.bind();
	texture.enable();

	//glPushMatrix();
	//glTranslatef(0.0, 0.0, 100.0);
	objects.draw();
	//glPopMatrix();

	texture.disable();
	texture.unbind();

	fog.disable();

}