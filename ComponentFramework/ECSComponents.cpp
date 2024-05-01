#include "ECSComponents.h"

using namespace MATH;

//#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Debug.h"
#include <fstream>
#include <string.h>
#include <SDL_image.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace fs = std::filesystem;


BodyComponent::BodyComponent(): pos{}, vel{}, accel{}, mass{1.0f}, mesh{nullptr},texture{nullptr}, ECSComponent(nullptr){
}

BodyComponent::~BodyComponent() {}

void BodyComponent::Update(float deltaTime) {
	/// From 1st semester physics class
	pos += vel * deltaTime + 0.5f * accel * deltaTime * deltaTime;
	vel += accel * deltaTime;
}

void BodyComponent::ApplyForce(Vec3 force) {
	accel = force / mass;
}

void BodyComponent::UpdateOrientation(float deltaTime)
{
	UpdateAngularVel(deltaTime);

	// Pull out angle from angular velocity
	float angleDegrees = VMath::mag(angularVel) * deltaTime * RADIANS_TO_DEGREES;
	// if angle is zero get outta here
	if (angleDegrees < VERY_SMALL) {
		return;
	}
	// Axis of rotation
	Vec3 axis = VMath::normalize(angularVel);
	// Build a quaternion
	Quaternion rotation = QMath::angleAxisRotation(angleDegrees, axis);
	// Update the orientation
	orientation = orientation * rotation;
}

void BodyComponent::UpdateAngularVel(float deltaTime) {
	angularVel += angularAccel * deltaTime;

}

void BodyComponent::ApplyTorque(Vec3 torque)
{
	float I = (2.0f / 5.0f) * mass * (radius * radius) + mass * (radius * radius);

	iMatrix = MMath::scale(I, I, I);

	angularAccel = MMath::inverse(iMatrix) * torque;
}

void BodyComponent::UpdateVel(float deltaTime)
{
	vel += accel * deltaTime;
}

void BodyComponent::UpdatePos(float deltaTime)
{
	// For constrained motion, skip the accel part
	pos += vel * deltaTime;
}

void BodyComponent::RodConstraint(float deltaTime, Vec3 anchorPoint, float rodLength)
{
	Vec3 rodVector = anchorPoint - pos;
	float positionConstraint = VMath::mag(rodVector) - rodLength;

	float JV = VMath::dot(rodVector, vel) / MATH::VMath::mag(rodVector);
	const float baumgarteStabilizationParameter = 0.15; // Try tuning this number
	float b = -(baumgarteStabilizationParameter / deltaTime) * positionConstraint;
	float lambda = -mass * (JV + b);

	Vec3 Jtransposed = rodVector / VMath::mag(rodVector);
	Vec3 deltaVel = Jtransposed * lambda / mass;
	vel += deltaVel;
}



bool BodyComponent::OnCreate() {
	return true;
}

void BodyComponent::OnDestroy() {
}

void BodyComponent::Render() const {
}





newTransformComponent::newTransformComponent(Vec3 pos, Vec3 scale, Quaternion rotation, newTransformComponent* _parent):  _pos(pos), _scale(scale), _rotation(rotation),ECSComponent(nullptr) {
	_pos = pos;
	_scale = scale;
	_rotation = rotation;
	initTransform(_parent);
}

newTransformComponent::~newTransformComponent() {}

void newTransformComponent::OnDestroy() {}

void newTransformComponent::Update(float deltaTime) {}

void newTransformComponent::Render() const {}

newMeshComponent::newMeshComponent(ECSComponent* parent_, const char* filename_) : ECSComponent(parent_) {
    filename = filename_;
}
newMeshComponent::~newMeshComponent() {}

bool newMeshComponent::OnCreate() {
    LoadModel(filename);
    StoreMeshData(GL_TRIANGLES);
    return true;
}


void newMeshComponent::LoadModel(const char* filename) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    vertices.clear();
    normals.clear();
    uvCoords.clear();

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename)) {
        throw std::runtime_error(warn + err);
    }
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vec3 vertex{};
            vertex.x = attrib.vertices[3 * index.vertex_index + 0];
            vertex.y = attrib.vertices[3 * index.vertex_index + 1];
            vertex.z = attrib.vertices[3 * index.vertex_index + 2];

            Vec3 normal{};
            normal.x = attrib.normals[3 * index.normal_index + 0];
            normal.y = attrib.normals[3 * index.normal_index + 1];
            normal.z = attrib.normals[3 * index.normal_index + 2];

            Vec2 uvCoord{};
            uvCoord.x = attrib.texcoords[2 * index.texcoord_index + 0];
            uvCoord.y = -attrib.texcoords[2 * index.texcoord_index + 1];

            vertices.push_back(vertex);
            normals.push_back(normal);
            uvCoords.push_back(uvCoord);
        }
    }
}

void newMeshComponent::StoreMeshData(GLenum drawmode_) {
    drawmode = drawmode_;
    /// These just make the code easier for me to read
#define VERTEX_LENGTH 	(vertices.size() * (sizeof(Vec3)))
#define NORMAL_LENGTH 	(normals.size() * (sizeof(Vec3)))
#define TEXCOORD_LENGTH (uvCoords.size() * (sizeof(Vec2)))

    const int verticiesLayoutLocation = 0;
    const int normalsLayoutLocation = 1;
    const int uvCoordsLayoutLocation = 2;

    /// create and bind the VOA
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    /// Create and initialize vertex buffer object VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_LENGTH + NORMAL_LENGTH + TEXCOORD_LENGTH, nullptr, GL_STATIC_DRAW);

    /// assigns the addr of "points" to be the beginning of the array buffer "sizeof(points)" in length
    glBufferSubData(GL_ARRAY_BUFFER, 0, VERTEX_LENGTH, &vertices[0]);
    /// assigns the addr of "normals" to be "sizeof(points)" offset from the beginning and "sizeof(normals)" in length  
    glBufferSubData(GL_ARRAY_BUFFER, VERTEX_LENGTH, NORMAL_LENGTH, &normals[0]);
    /// assigns the addr of "texCoords" to be "sizeof(points) + sizeof(normals)" offset from the beginning and "sizeof(texCoords)" in length  
    glBufferSubData(GL_ARRAY_BUFFER, VERTEX_LENGTH + NORMAL_LENGTH, TEXCOORD_LENGTH, &uvCoords[0]);

    glEnableVertexAttribArray(verticiesLayoutLocation);
    glVertexAttribPointer(verticiesLayoutLocation, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(normalsLayoutLocation);
    glVertexAttribPointer(normalsLayoutLocation, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(VERTEX_LENGTH));
    glEnableVertexAttribArray(uvCoordsLayoutLocation);
    glVertexAttribPointer(uvCoordsLayoutLocation, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(VERTEX_LENGTH + NORMAL_LENGTH));


    dataLength = vertices.size();

    /// give back the memory used in these vectors. The data is safely stored in the GPU now
    vertices.clear();
    normals.clear();
    uvCoords.clear();

    /// Don't need these defines sticking around anymore
#undef VERTEX_LENGTH
#undef NORMAL_LENGTH
#undef TEXCOORD_LENGTH

}

void newMeshComponent::Render() const {
    glBindVertexArray(vao);
    glDrawArrays(drawmode, 0, dataLength);
    glBindVertexArray(0); // Unbind the VAO
}

void newMeshComponent::Render(GLenum drawmode_) const {
    glBindVertexArray(vao);
    glDrawArrays(drawmode_, 0, dataLength);
    glBindVertexArray(0); // Unbind the VAO
}

void newMeshComponent::OnDestroy() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

/// Currently unused.

void newMeshComponent::Update(const float deltaTime) {}


newShaderComponent::newShaderComponent(ECSComponent* parent_, const char* vertFilename_, const char* fragFilename_,
	const char* tessCtrlFilename_, const char* tessEvalFilename_,
	const char* geomFilename_) :
	ECSComponent(parent_),
	shaderID(0),vertShaderID(0),fragShaderID(0),tessCtrlShaderID(0),
	tessEvalShaderID(0),geomShaderID(0),
	vertFilename(vertFilename_),
	fragFilename(fragFilename_),
	tessCtrlFilename(tessCtrlFilename_),
	tessEvalFilename(tessEvalFilename_),
	geomFilename(geomFilename_) { }
	

newShaderComponent::~newShaderComponent() {}


bool newShaderComponent::OnCreate() {
	bool status;
	status = CompileAttach();
	if (status == false) return false;
	status = Link();
	if (status == false) return false;

	SetUniformLocations();
	return true;
}

void newShaderComponent::OnDestroy() {
	glDetachShader(shaderID, fragShaderID);
	glDetachShader(shaderID, vertShaderID);
	glDeleteShader(fragShaderID);
	glDeleteShader(vertShaderID);


	if (tessCtrlShaderID){ 
		glDetachShader(shaderID, tessCtrlShaderID);
		glDeleteShader(tessCtrlShaderID);
	}
	if (tessEvalShaderID){
		glDetachShader(shaderID, tessEvalShaderID);
		glDeleteShader(tessEvalShaderID);
	}

	if (geomShaderID){
		glDetachShader(shaderID, geomShaderID);
		glDeleteShader(geomShaderID);
	}
	
	glDeleteProgram(shaderID);

}



bool newShaderComponent::CompileAttach() {
    GLint status;
	const char* vertText = nullptr;
	const char* fragText = nullptr;
	const char* tessCtrlText = nullptr;
	const char* tessEvalText = nullptr;
	const char* geomText = nullptr;

	try { 		
		vertText = ReadTextFile(vertFilename);
		fragText = ReadTextFile(fragFilename);
		if (vertText == nullptr || fragText == nullptr) {
			return false;
		}
		/// GL_VERTEX_SHADER and GL_FRAGMENT_SHADER are defined in glew.h
		vertShaderID = glCreateShader(GL_VERTEX_SHADER);
		fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		/// Check for errors
		if (vertShaderID == 0 || fragShaderID == 0) {
			std::string errorMsg("Can't create a new shader");
			throw errorMsg;
		}
    
		glShaderSource(vertShaderID, 1, &vertText, 0);   
		glShaderSource(fragShaderID, 1, &fragText, 0);
    
		glCompileShader(vertShaderID);
		/// Check for errors
		glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &status);
		if(status == 0) {
			GLsizei errorLogSize = 0;
			GLsizei titleLength;
			std::string errorLog = vertFilename;
			errorLog +="\nVERT:\n";
			titleLength = errorLog.length();
			glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &errorLogSize);
			errorLog.resize( titleLength + errorLogSize );
			glGetShaderInfoLog(vertShaderID, errorLogSize, &errorLogSize, &errorLog[titleLength]);
			throw errorLog;
		}

		glCompileShader(fragShaderID);
		/// Check for errors
		glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &status);
		if(status == 0) {
			GLsizei errorLogSize = 0;
			GLsizei titleLength;
			std::string errorLog = fragFilename;
			errorLog += "\nFrag:\n";
			titleLength = errorLog.length();
			glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &errorLogSize);
			errorLog.resize( titleLength + errorLogSize );
			glGetShaderInfoLog(fragShaderID, errorLogSize, &errorLogSize, &errorLog[titleLength]);
			throw errorLog;
		}

		if (tessCtrlFilename != nullptr && tessEvalFilename != nullptr) {
			tessCtrlText = ReadTextFile(tessCtrlFilename);
			tessEvalText = ReadTextFile(tessEvalFilename);
			
			if (tessCtrlText == nullptr || tessEvalText == nullptr) {
				std::string errorLog = "Can't read either tessCtrlFile or tessEvalFile";
				throw errorLog;
			}
			tessCtrlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
			tessEvalShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);

			glShaderSource(tessCtrlShaderID, 1, &tessCtrlText, 0);
			glShaderSource(tessEvalShaderID, 1, &tessEvalText, 0);

			glCompileShader(tessCtrlShaderID);
			/// Check for errors
			glGetShaderiv(tessCtrlShaderID, GL_COMPILE_STATUS, &status);
			if (status == 0) {
				GLsizei errorLogSize = 0;
				GLsizei titleLength;
				std::string errorLog = tessCtrlFilename;
				errorLog += "\nTESS CTRL:\n";
				titleLength = errorLog.length();
				glGetShaderiv(tessCtrlShaderID, GL_INFO_LOG_LENGTH, &errorLogSize);
				errorLog.resize(titleLength + errorLogSize);
				glGetShaderInfoLog(tessCtrlShaderID, errorLogSize, &errorLogSize, &errorLog[titleLength]);
				throw errorLog;
			}

			glCompileShader(tessEvalShaderID);
			/// Check for errors
			glGetShaderiv(tessEvalShaderID, GL_COMPILE_STATUS, &status);
			if (status == 0) {
				GLsizei errorLogSize = 0;
				GLsizei titleLength;
				std::string errorLog = tessEvalFilename;
				errorLog += "\nTESS EVAL:\n";
				titleLength = errorLog.length();
				glGetShaderiv(tessEvalShaderID, GL_INFO_LOG_LENGTH, &errorLogSize);
				errorLog.resize(titleLength + errorLogSize);
				glGetShaderInfoLog(tessEvalShaderID, errorLogSize, &errorLogSize, &errorLog[titleLength]);
				throw errorLog;
			}



		}
		else if ((tessCtrlFilename != nullptr) != (tessEvalFilename != nullptr)) { /// XOR
			std::string errorLog = "To use tessilation there needs to be both a tessCtrlFilename and tessEvalFilename";
			throw errorLog;
		}

		if(geomFilename != nullptr) {
			geomText = ReadTextFile(geomFilename);
			if (geomText == nullptr) {
				std::string errorMsg("Can't open file:");
				throw errorMsg + geomFilename;
			}
			geomShaderID = glCreateShader(GL_GEOMETRY_SHADER);
			if (geomShaderID == 0) {
				std::string errorMsg("Can't create a new geom shader ID");
				throw errorMsg;
			}
			glShaderSource(geomShaderID, 1, &geomText, 0);
			glCompileShader(geomShaderID);
			/// Check for errors
			glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &status);
			if(status == 0) {
				GLsizei errorLogSize = 0;
				GLsizei titleLength;
				std::string errorLog = geomFilename;
				errorLog +="\nGEOM:\n";
				titleLength = errorLog.length();
				glGetShaderiv(geomShaderID, GL_INFO_LOG_LENGTH, &errorLogSize);
				errorLog.resize( titleLength + errorLogSize );
				glGetShaderInfoLog(geomShaderID, errorLogSize, &errorLogSize, &errorLog[titleLength]);
				throw errorLog;
			}

		}
		shaderID = glCreateProgram();
		glAttachShader(shaderID, fragShaderID);
		glAttachShader(shaderID, vertShaderID);
		if(tessCtrlShaderID) glAttachShader(shaderID, tessCtrlShaderID);
		if(tessEvalShaderID) glAttachShader(shaderID, tessEvalShaderID);
		if(geomShaderID) glAttachShader(shaderID, geomShaderID);

		if(vertText) delete[] vertText;
		if(fragText) delete[] fragText;
		if(tessCtrlText) delete[] tessCtrlText;
		if(tessEvalText) delete[] tessEvalText;
		if(geomText) delete[] geomText;

	}catch(std::string error){
		printf("%s\n",&error[0]);
		Debug::Error(error, __FILE__, __LINE__);
		return false;
	}
	return true;
}


bool newShaderComponent::Link() {
	GLint status;
	try {
		glLinkProgram(shaderID);
		/// Check for errors
		glGetProgramiv(shaderID, GL_LINK_STATUS, &status);
		if (status == 0) {
			GLsizei errorLogSize = 0;
			std::string errorLog;
			glGetProgramiv(shaderID, GL_INFO_LOG_LENGTH, &errorLogSize);
			errorLog.resize(errorLogSize);
			glGetProgramInfoLog(shaderID, errorLogSize, &errorLogSize, &errorLog[0]);
			throw errorLog;
		}

	}
	catch (std::string error) {
		Debug::Error(error, __FILE__, __LINE__);
		return false;
	}
	return true;
}

GLuint newShaderComponent::GetUniformID(std::string name) {

	auto id = uniformMap.find(name);
#ifdef _DEBUG
	static bool writeBadUniformWarning = true;
	if (id == uniformMap.end()) {
		if (writeBadUniformWarning){
			Debug::Warning(name + " :No such uniform name", __FILE__, __LINE__);
			writeBadUniformWarning = false;/// I already told you, don't repeat the message 
		}
		return -1;
	}
#endif 
	return id->second;
}

void newShaderComponent::SetUniformLocations() {
	int count;
	GLsizei actualLen;
	GLint size;
	GLenum type;
	char *name;
	int maxUniformListLength;
	unsigned int loc;
	char buffer[512];

	std::string activeUniformList = "Uniforms:\n";
	glGetProgramiv(shaderID, GL_ACTIVE_UNIFORMS, &count);
	sprintf_s(buffer,512,"There are %d active Uniforms\n",count);

	/// get the length of the longest named uniform 
	glGetProgramiv(shaderID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformListLength);

	/// Create a little buffer to hold the uniform's name - old C memory call just for fun 
	name = (char *)malloc(sizeof(char) * maxUniformListLength);

	
	for (int i = 0; i < count; ++i) {
		
		/// Get the name of the ith uniform
		
		glGetActiveUniform(shaderID, i, maxUniformListLength, &actualLen, &size, &type, name);
		/// Get the (unsigned int) loc for this uniform
		loc = glGetUniformLocation(shaderID, name);
		std::string uniformName = name;
		uniformMap[uniformName] = loc;

		sprintf_s(buffer,512,"\"%s\" loc:%d\n", uniformName.c_str() , uniformMap[uniformName]);
		activeUniformList += buffer;
	}
	Debug::Info(activeUniformList, __FILE__, __LINE__);
	free(name);
}

char* newShaderComponent::ReadTextFile(const char *filename) {
		char* buffer = nullptr;
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {								/// Open the file
        file.seekg(0, std::ios::end);					/// goto the end of the file
        int bufferSize = (int) file.tellg();			/// Get the length of the file
		if(bufferSize == 0) {							/// If zero, bad file
			std::string errorMsg("Can't read shader file: ");
			std::string str2(filename);
			errorMsg += str2;
			throw errorMsg;								/// Bail out
		}
		buffer = new char[(int)(bufferSize + 1)];		/// Need to NULL terminate the array
        file.seekg(0, std::ios::beg);
        file.read(buffer, bufferSize);
		buffer[bufferSize] = NULL;						/// Add the NULL
		file.close();
    } else { 
		std::string errorMsg  ("Can't open shader file: ");
		errorMsg += filename;
		Debug::Error(errorMsg, __FILE__, __LINE__);
	}
	return buffer;
}

/// Not used, but must be made concrete
void newShaderComponent::Update(const float deltaTime) {};
void newShaderComponent::Render() const {};


TextureComponent::TextureComponent():textureID(0), ECSComponent(nullptr) {}
void TextureComponent::OnDestroy() {}
void TextureComponent::Update(float deltaTime) {}
void TextureComponent::Render() const {}

bool TextureComponent::LoadTexture(const char* filename) {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	SDL_Surface *textureSurface = IMG_Load(filename);
	if (textureSurface == nullptr) {
		return false;
	}
	image_height = textureSurface->h; image_width = textureSurface->w;
	int mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
	
	SDL_FreeSurface(textureSurface);
	/// Wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0); /// Unbind the texture
	return true;
}


TextureComponent::~TextureComponent(){
	glDeleteTextures(1, &textureID);
}

Texture2DComponent::Texture2DComponent():image_data(nullptr), image_width(0), image_height(0), image_channels(0), ECSComponent(nullptr) {}
void Texture2DComponent::OnDestroy() {}
void Texture2DComponent::Update(float deltaTime) {}
void Texture2DComponent::Render() const {}

bool Texture2DComponent::LoadTexture(const char* filename) {

    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;


    GLuint* image_texture = new GLuint;
    glGenTextures(1, image_texture);
    glBindTexture(GL_TEXTURE_2D, *image_texture);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    texture = image_texture;
    return true;
}



Texture2DComponent::~Texture2DComponent(){
	stbi_image_free(image_data);
}