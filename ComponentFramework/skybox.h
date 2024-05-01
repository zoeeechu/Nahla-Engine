#pragma once

#include <glew.h>
#include <MMath.h>
#include <SDL_image.h>
#include <Matrix.h>
#include <Quaternion.h>
#include <Vector.h>
#include <MMath.h>
#include <QMath.h>
#include <VMath.h>

using namespace MATH;


class MeshComponent;
class ShaderComponent; 
class Camera;

#include <vector>

class skybox
{
private:
	GLuint textureID; 

	const char* files[6];

	const int cubeMap[6] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, //right
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X, //left
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,	//top
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,	//bottom
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,	//back
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,	//front
	};

	MeshComponent* cube;
	ShaderComponent* shader;

	Quaternion o; // Orientation
	Vec3 p; //Position
	Quaternion trkBll;
	float zoom;

	float maxZoom; 
	float minZoom;



public:
	skybox(const char* right_, const char *left_, const char *top_, const char *bottom_, const char *back_, const char *front_);
	~skybox();
	bool loadImages();
	bool OnCreate();
	void Render(Camera _cam) const;
	GLuint GetTextureID() const;
	Matrix4 v() const ;
	Quaternion setTrackball(Quaternion _t);
	float setZoom(float _z);


};

