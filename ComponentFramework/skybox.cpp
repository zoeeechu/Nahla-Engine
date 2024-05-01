#include "skybox.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "Camera.h"

skybox::skybox(const char *right_, const char *left_, const char *top_, const char *bottom_, const char *back_, const char *front_)
{

	cube = new MeshComponent(nullptr, nullptr);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

 	files[0] = right_; files[1] = left_;
	files[2] = top_; files[3] = bottom_;
	files[4] = back_; files[5] = front_;

	o = Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0f));
	p = Vec3(0.0f, 0.0f, 0.0f);

	maxZoom = -1.46; 
	minZoom = 0.46;
}

skybox::~skybox()
{
	cube->OnDestroy();
	delete cube;
	glDeleteTextures(1, &textureID);
}

bool skybox::OnCreate()
{
	cube = new MeshComponent(nullptr, "meshes/Cube.obj");
	bool status = cube->OnCreate();
	if (!status)
		return status;
	shader = new ShaderComponent(nullptr, "shaders/skyboxVert.glsl", "shaders/skyboxFrag.glsl");
	if (shader->OnCreate() == false)
	{
		std::cout << "Shader failed ... we have a problem\n";
	}
	return loadImages();
}

Matrix4 skybox::v() const
{
	return MMath::toMatrix4(trkBll) * MMath::translate(p) * MMath::toMatrix4(o) * MMath::scale(Vec3(2.0f, 2.0f, 2.0f));
}

Quaternion skybox::setTrackball(Quaternion _t)
{
	trkBll = _t;
	return trkBll;
}


GLuint skybox::GetTextureID() const
{
	return textureID;
}


float skybox::setZoom(float _z)
{
	zoom = _z / 100;
	p.z = _z / 100;
	if (p.z < maxZoom) p.z = maxZoom; 
	if (p.z > minZoom) p.z = minZoom; 
	return zoom;
}

void skybox::Render(Camera _cam) const
{
	glDepthMask(GL_FALSE);
	glCullFace(GL_FALSE);
	glUseProgram(shader->GetProgram());
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, _cam.GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, v());

	cube->Render(GL_TRIANGLES);
	glDepthMask(GL_TRUE);
	glCullFace(GL_TRUE);
}

bool skybox::loadImages()
{

	SDL_Surface *textureSurface;
	int mode;

	for (int i = 0; i < 6; i++)
	{

		textureSurface = IMG_Load(files[i]);
		if (textureSurface == nullptr)
		{
			return false;
		}
		mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(cubeMap[i], 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
		SDL_FreeSurface(textureSurface);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return true;
}
