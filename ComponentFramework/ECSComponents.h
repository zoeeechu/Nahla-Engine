#ifndef ECSCOMPONENTS_H
#define ECSCOMPONENTS_H

#include "ECS.h"

#include <SDL_image.h>
#include <SDL.h>
#include <unordered_map>
#include <filesystem>
#include <Matrix.h>
#include <MMath.h>
#include <QMath.h>
#include <VMath.h>
#include <string>
#include <glew.h>
#include <vector>

#include "Vector.h"
#include "Quaternion.h"



using namespace MATH;

class Mesh;
class Texture;

class BodyComponent : public ECSComponent
{
public:
    BodyComponent();
    ~BodyComponent();
	Vec3 pos;
	Vec3 vel;
	Vec3 accel;
	float mass = 1.0f;
	// New stuff for physics 2
	Vec3 angularVel;
	Quaternion orientation;
	Vec3 angularAccel;

	Matrix4 modelMatrix;
	Matrix3 iMatrix;
	float radius = 0.5f;
	float rodLength = 0.1f;
private: /// Graphics stuff 
	Mesh *mesh;
	Texture *texture;
	
public:
	bool OnCreate();
	void OnDestroy();
	void Update(float deltaTime);
	void UpdateVel(float deltaTime);
	void UpdatePos(float deltaTime);
	void Render() const;
	void ApplyForce(Vec3 force);
	void UpdateOrientation(float deltaTime);
	void setAccel(const Vec3 &accel_) { accel = accel_;}
	void UpdateAngularVel(float deltaTime);
	void ApplyTorque(Vec3 torque);

		Matrix4 GetModelMatrix() const {
		Matrix4 R = MMath::toMatrix4(orientation);
		Matrix4 T = MMath::translate(pos);
		Vec3 scalingFactor(radius, radius, radius);
		Matrix4 S = MMath::scale(scalingFactor);
		return T * R * S;
	}
	void RodConstraint(float deltaTime, Vec3 anchorPoint, float rodLength);
};

class newTransformComponent : public ECSComponent
{

private:
	Vec3 _pos;
	Vec3 _scale;
	Quaternion _rotation;
	Matrix4 _transform;

public:
	newTransformComponent(Vec3 pos = Vec3(0.0f, 0.0f, 0.0f),
						  Vec3 scale = Vec3(1.0f, 1.0f, 1.0f),
						  Quaternion rotation = Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0f)), newTransformComponent *_parent = nullptr);

	bool OnCreate() override
	{
		return true;
	}
	// add a update function
	~newTransformComponent();
	Vec3 pos() { return _pos; }
	Vec3 scale() { return _scale; }
	Quaternion rotation() { return _rotation; }
	Matrix4 transform() { return _transform; }
	void initTransform(newTransformComponent *_parent = nullptr)
	{
		if (_parent != nullptr)
		{
			_transform = _parent->transform() * MMath::rotate(_rotation.w, _rotation.ijk) * MMath::translate(_pos) * MMath::scale(_scale);
		}
		_transform = MMath::rotate(_rotation.w, _rotation.ijk) * MMath::translate(_pos) * MMath::scale(_scale);
	}
	void setPos(Vec3 pos) { _pos = pos; }
	void setScale(Vec3 scale) { _scale = scale; }
	void setRotation(Quaternion rotation) { _rotation = rotation; }

	void OnDestroy();
	void Update(const float deltaTime);
	void Render() const;
};

class newMeshComponent : public ECSComponent
{

	newMeshComponent(const newMeshComponent &) = delete;
	newMeshComponent(newMeshComponent &&) = delete;
	newMeshComponent &operator=(const newMeshComponent &) = delete;
	newMeshComponent &operator=(newMeshComponent &&) = delete;

private:
	const char *filename;
	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvCoords;
	size_t dataLength;
	GLenum drawmode;

	/// Private helper methods
	void LoadModel(const char *filename);
	void StoreMeshData(GLenum drawmode_);
	GLuint vao, vbo;

public:
	newMeshComponent(ECSComponent *parent_, const char *filename_);
	~newMeshComponent();
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime);
	void Render() const;
	void Render(GLenum drawmode) const;
};

class newShaderComponent : public ECSComponent
{
	newShaderComponent(const newShaderComponent &) = delete;
	newShaderComponent(newShaderComponent &&) = delete;
	newShaderComponent &operator=(const newShaderComponent &) = delete;
	newShaderComponent &operator=(newShaderComponent &&) = delete;

private:
	const char *vertFilename;
	const char *fragFilename;
	const char *tessCtrlFilename;
	const char *tessEvalFilename;
	const char *geomFilename;

	GLuint shaderID;
	GLuint vertShaderID;
	GLuint fragShaderID;
	GLuint tessCtrlShaderID;
	GLuint tessEvalShaderID;
	GLuint geomShaderID;
	std::unordered_map<std::string, GLuint> uniformMap;

	/// Private helper methods
	char *ReadTextFile(const char *fileName);
	bool CompileAttach();
	bool Link();
	void SetUniformLocations();

public:
	newShaderComponent(ECSComponent *parent_, const char *vertFilename_, const char *fragFilename_,
					   const char *tessCtrlFilename_ = nullptr, const char *tessEvalFilename = nullptr,
					   const char *geomFilename_ = nullptr);
	~newShaderComponent();
	bool OnCreate() override;
	void OnDestroy() override;

	void Update(float deltaTime) override; // just cause

	inline GLuint GetProgram() const { return shaderID; }
	GLuint GetUniformID(std::string name);

	virtual void Render() const;
};

class TextureComponent : public ECSComponent
{

public:
	TextureComponent();
	~TextureComponent();

	bool LoadTexture(const char *filename);
	inline GLuint getTextureID() const { return textureID; }
	inline int getImageWidth() const { return image_width; }
	inline int getImageHeight() const { return image_height; }

	bool OnCreate() override { return true; }
	void OnDestroy();
	void Update(const float deltaTime);
	void Render() const;

private:
	GLuint textureID;
	int image_width, image_height;
};


class Texture2DComponent : public ECSComponent
{

public:
	Texture2DComponent();
	~Texture2DComponent();

	bool LoadTexture(const char *filename);
	inline char getImageData() const { return *image_data; }
	inline int getImageWidth() const { return image_width; }
	inline int getImageHeight() const { return image_height; }
	inline char getFilePath() const { return *file; }
	inline GLuint getTexture() const { return *texture; }
	bool OnCreate() override { return true; }
	void OnDestroy();
	void Update(const float deltaTime);
	void Render() const;

private:
	unsigned char* image_data;
	unsigned char* file;
	int image_width, image_height, image_channels;
	GLuint* texture;
};


#endif