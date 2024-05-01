#ifndef SCENE3G_H
#define SCENE3G_H
#include "Scene.h"
#include "Vector.h"
#include "Trackball.h"
#include "Camera.h"
#include <Matrix.h>
using namespace MATH;

/// Forward declarations
union SDL_Event;
class Body;
// class Mesh;
class MeshComponent;
// class Shader;
class ShaderComponent;
// class Texture
class Texture;

class skybox;





//#define MAXFLOAT	99999.99
//#define MAXDEPTH 	5
//#define NUMSAMPLES 	4
//#define ROTATION	true
//
//
//#define LAMBERT    0
//#define METAL      1
//#define DIELECTRIC 2


class Scene3G : public Scene
{
private:
	Body *skullBody;
	Body *eyeBody;
	ShaderComponent *shader;
	MeshComponent *skull;
	MeshComponent *eye;
	MeshComponent *eye2;
	Texture *Skulltexture;
	Texture *Eyetexture;
	Matrix4 projectionMatrix;
	Matrix4 viewMatrix;
	Matrix4 skullMatrix;
	Matrix4 lEyeMatrix, rEyeMatrix;
	Matrix4 RCMatrix;
	bool drawInWireMode;
	bool IsPaused = false;
	Vec3 lightPos;

	skybox *Sky;


	float dt;

	Camera cam;

	float Zoom;
	float initEyeZ = -5.0f;
	float ZoomAmount;

	Trackball trackball = Trackball();

	Vec3 Lights[4]{
		Vec3(0.0f, 8.0f, 5.0f),
		Vec3(0.0f, -10.0f, 5.0f),
	};

	Vec4 kd[4]{
		// color
		Vec4(0.5f, 0.5f, 0.8f, 0.0f), // rgba
		Vec4(0.3f, 0.3f, 0.3f, 0.0f),
	};

	Vec4 ks[4]{
		// specular

		Vec4(0.05f, 0.05f, 0.05f, 0.0f), // rgba
		Vec4(0.06f, 0.06f, 0.03f, 0.0f),

	};

	struct Ray
	{
		Vec3 origin;
		Vec3 direction;
	};

	struct IntersectInfo
	{
		// surface properties
		float t;
		Vec3 p;
		Vec3 normal;

		// material properties
		int materialType;
		Vec3 albedo;
		float fuzz;
		float refractionIndex;
	};

	struct Sphere
	{
		// sphere properties
		Vec3 center;
		float radius;

		// material
		int materialType;
		Vec3 albedo;
		float fuzz;
		float refractionIndex;
	};

public:
	explicit Scene3G();
	virtual ~Scene3G();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;

	//// bool Sphere_hit(Sphere sphere, Ray ray, float t_min, float t_max, out IntersectInfo rec);
	//float schlick(float cos_theta, float n2);
	//// bool refractVec(Vec3 v, Vec3 n, float ni_over_nt, out Vec3 refracted);
	//Vec3 reflectVec(Vec3 v, Vec3 n);
	//float hash(const float n);
	//float rand2D();
	//Vec3 random_in_unit_sphere();
	//Vec3 random_in_unit_disk();
};

#endif // Scene2G_H