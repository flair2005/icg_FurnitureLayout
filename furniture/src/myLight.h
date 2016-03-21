#ifndef __MYLIGHT_H__
#define __MYLIGHT_H__

#include "icg.h"

//点光源クラス（自作）
class PointLight{
private:
	Tvector3<float> position;

	Tcolor3<float> color;

	float intensity;

	Tcolor3<float> ambient;

	float luminosity;	//光度(cd)


public:

	PointLight(const Tcolor3<float>& c = Tcolor3<float>(0., 0., 0.), 
		float intensity = 1.0, 
		const Tcolor3<float>& ambient = Tcolor3<float>(0.0, 0.0, 0.0),
		float cd = 1.0)
	{
		color = c;
		this->intensity = intensity;
		this->ambient = ambient;
		luminosity = cd;
	}

	~PointLight(){}

	void setPosition(const Tvector3<float>& p){
		position = p;
	}

	void setColor(const Tcolor3<float>& c){
		color = c;
	}

	void setIntensity(float val){
		intensity = val;
	}

	void setAmbient(const Tcolor3<float>& c){
		ambient = c;
	}

	void setAmbient(float amb){
		ambient.assign(amb, amb, amb);
	}

	void setLuminosity(float cd){
		luminosity = cd;
	}

	void setAllParam(float x0, float y0, float z0, Tcolor3<float>& c, float Intensity, float amb, float cd)
	{
		Tvector3<float>p(x0, y0, z0);
		position = p;
		color = c;
		intensity = Intensity;
		ambient.assign(amb, amb, amb);
		luminosity = cd;
	}

	Tcolor3<float>& getAmbient(){
		return(ambient);
	}

	Tvector3<float>& getPosition(){
		return (position);
	}

	float& getLuminosity(){
		return (luminosity);
	}

	float& getIntensity(){
		return (intensity);
	}

	Tcolor3<float> shade(Tvector3<float> p, Tvector3<float> n, Tcolor3<float> diffuse);
};

//平行光源クラス（自作：ちょっと変かも）
class ParallelLight{
private:
	Tvector3<float> direction;

	Tcolor3<float> color;

	float intensity;

	Tcolor3<float> ambient;

public:
	ParallelLight(const Tvector3<float>& dir = Tvector3<float>(0.0, 0.0, 1.0),
		const Tcolor3<float>& c = Tcolor3<float>(0., 0., 0.),
		float intensity = 1.0,
		const Tcolor3<float>& ambient = Tcolor3<float>(0.0, 0.0, 0.0))
	{
		direction = dir;
		direction.normalize();

		color = c;

		this->intensity = intensity;
		this->ambient = ambient;
	}

	~ParallelLight(){}

	void setDirection(const Tvector3<float>& dir){
		direction = dir;
		direction.normalize();
	}

	void setColor(const Tcolor3<float>& c){
		color = c;
	}

	void setIntensity(float val){
		intensity = val;
	}

	void setAmbient(const Tcolor3<float>& c){
		ambient = c;
	}

	void setAmbient(float amb){
		ambient.assign(amb, amb, amb);
	}

	Tcolor3<float>& getAmbient(){
		return(ambient);
	}


	Tcolor3<float> shade(Tvector3<float> n, Tcolor3<float> diffuse);
};

//スポットライト
class SpotLight{
private:
	Tvector3<float> position;

	Tcolor3<float> color;

	float intensity;

	Tvector3<float> direction;

	Tcolor3<float> ambient;

	//指向性
	int directivity;

	//光度
	float luminosity;

public:

	SpotLight(const Tcolor3<float>& c = Tcolor3<float>(0., 0., 0.), 
		float intensity = 1.0, 
		const Tcolor3<float>& ambient = Tcolor3<float>(0.0, 0.0, 0.0), 
		const int n = 0.0)
	{
		color = c;
		this->intensity = intensity;
		this->ambient = ambient;
		directivity = n;
	}

	~SpotLight(){}

	void setPosition(const Tvector3<float>& p){
		position = p;
	}

	void setColor(const Tcolor3<float>& c){
		color = c;
	}

	void setIntensity(float val){
		intensity = val;
	}

	void setDirection(const Tvector3<float>& dir){
		direction = dir;
		direction.normalize();
	}

	void setAmbient(const Tcolor3<float>& c){
		ambient = c;
	}

	void setAmbient(float amb){
		ambient.assign(amb, amb, amb);
	}

	void setDirectivity(int n){
		directivity = n;
	}

	void setAllParam(float x0, float y0, float z0, Tcolor3<float>&c, float Intensity, float cd, float xdir, float ydir, float zdir, float amb, int n)
	{
		Tvector3<float> p(x0, y0, z0);
		position = p;
		color = c;
		intensity = Intensity;
		Tvector3<float> dir(xdir, ydir, zdir);
		direction = dir;
		direction.normalize();
		ambient.assign(amb, amb, amb);
		directivity = n;
		luminosity = cd;
	}

	Tcolor3<float>& getAmbient(){
		return(ambient);
	}

	Tvector3<float>& getPosition(){
		return(position);
	}

	Tvector3<float>& getDirection(){
		return (direction);
	}

	int& getDirectivity(){
		return directivity;
	}

	float& getLuminosity(){
		return luminosity;
	}

	float& getIntensity(){
		return intensity;
	}

	Tcolor3<float> shade(Tvector3<float> p, Tvector3<float> n, Tcolor3<float> diffuse);

};

void calcOnceRefraction(vector<Tvector3<float>>& wallpoint, vector<PointLight>& pointLight, vector<SpotLight>& spotLight,
	int pnum, int snum, int imesh/*横のメッシュの数*/, int jmesh/*縦のメッシュの数*/, float reflectance, Tvector3<float> normal, Tcolor3<float>& color);


#endif