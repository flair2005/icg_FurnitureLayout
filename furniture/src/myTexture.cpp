#include"myTexture.h"

Ttexturef texture,texture2;


//light.setDirection(Tvector3<float>(1, 1, 1).normalize());
//light.setAmbient(Tcolor3<float>(0.5, 0.5, 0.5));
//light.setColor(Tcolor3<float>(1.0, 1.0, 1.0));

Tcolor3<float> shade(Tvector3<float> p, Tvector3<float> n, Tcolor3<float> diffuse)
{
	float co = n.dot(Tvector3<float>(1.0,1.0,1.0).normalize());
	Tcolor3<float> c = 1.0 * diffuse * Tcolor3<float>(1.0,1.0,1.0) * co;
	return c;
}


void initTexture()
{
	Timage image("texture/wood_white_2.bmp");
	texture.init(image.getWidth(), image.getHeight());
	texture.setPixels(image);
	texture.glInit();
	

	Timage image2("texture/concrete_wall.bmp");
	texture2.init(image2.getWidth(), image2.getHeight());
	texture2.setPixels(image2);
	texture2.glInit();

}

void drawTexturedQuad(float floorX, float floorY)
{
	float scaleX = 1.4, scaleY = 1.4;
	float r, g, b;
	Tcolor3<float> c;
	glColor3f(1.0, 1.0, 1.0);
	texture.bind();
	texture.enable();
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		//c = shade(Tvector3<float>((-floorX / 2.0)*scaleX, (-floorY / 2.0)*scaleY, 0.0), Tvector3<float>(0.0,0.0,1.0), Tcolor3<float>(1.0,1.0,1.0));
		//r = c.r;	g = c.g;	b = c.b;
		//glColor3f(r, g, b);
		glVertex3f((-floorX/2.0)*scaleX, (-floorY/2.0)*scaleY, 0.0);

		glTexCoord2f(1, 0);
		//c = shade(Tvector3<float>((floorX / 2.0)*scaleX, (-floorY / 2.0)*scaleY, 0.0), Tvector3<float>(0.0,0.0,1.0), Tcolor3<float>(1.0,1.0,1.0));
		//r = c.r;	g = c.g;	b = c.b;
		//glColor3f(r, g, b);
		glVertex3f((floorX/2.0)*scaleX, (-floorY/2.0)*scaleY, 0);

		glTexCoord2f(1, 1);
		//c = shade(Tvector3<float>((floorX / 2.0)*scaleX, (floorY / 2.0)*scaleY, 0.0), Tvector3<float>(0.0,0.0,1.0), Tcolor3<float>(1.0,1.0,1.0));
		//r = c.r;	g = c.g;	b = c.b;
		//glColor3f(r, g, b);
		glVertex3f((floorX/2.0)*scaleX, (floorY/2.0)*scaleY, 0);

		glTexCoord2f(0, 1);
		//c = shade(Tvector3<float>((-floorX / 2.0)*scaleX, (floorY / 2.0)*scaleY, 0.0), Tvector3<float>(0.0,0.0,1.0), Tcolor3<float>(1.0,1.0,1.0));
		//r = c.r;	g = c.g;	b = c.b;
		//glColor3f(r, g, b);
		glVertex3f((-floorX/2.0)*scaleX, (floorY/2.0)*scaleY, 0);

	glEnd();
	texture.disable();
	texture.unbind();

	glColor3f(1.0, 1.0, 1.0);
	texture2.bind();
	texture2.enable();
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f((-floorX/2.0)*scaleX, (-floorY/2.0)*scaleY, floorX/2.2);

		glTexCoord2f(1, 0);
		glVertex3f((floorX/2.0)*scaleX, (-floorY/2.0)*scaleY, floorX/2.2);

		glTexCoord2f(1, 1);
		glVertex3f((floorX/2.0)*scaleX, (-floorY/2.0)*scaleY, 0);

		glTexCoord2f(0, 1);
		glVertex3f((-floorX/2.0)*scaleX, (-floorY/2.0)*scaleY, 0);
	glEnd();

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f((-floorX/2.0)*scaleX, (floorY/2.0)*scaleY, floorX/2.2);

		glTexCoord2f(1, 0);
		glVertex3f((-floorX/2.0)*scaleX, (-floorY/2.0)*scaleY, floorX/2.2);

		glTexCoord2f(1, 1);
		glVertex3f((-floorX/2.0)*scaleX, (-floorY/2.0)*scaleY, 0);

		glTexCoord2f(0, 1);
		glVertex3f((-floorX/2.0)*scaleX, (floorY/2.0)*scaleY, 0);
	glEnd();

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f((floorX/2.0)*scaleX, (-floorY/2.0)*scaleY, floorX/2.2);

		glTexCoord2f(1, 0);
		glVertex3f((floorX/2.0)*scaleX, (floorY/2.0)*scaleY, floorX/2.2);

		glTexCoord2f(1, 1);
		glVertex3f((floorX/2.0)*scaleX, (floorY/2.0)*scaleY, 0);

		glTexCoord2f(0, 1);
		glVertex3f((floorX/2.0)*scaleX, (-floorY/2.0)*scaleY, 0);
	glEnd();
	texture2.disable();
	texture2.unbind();

}

