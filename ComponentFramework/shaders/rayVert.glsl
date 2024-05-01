#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 uvCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
//uniform vec3 lightPos;
uniform vec3 Lights[4];

uniform vec4 ks[4];
uniform vec4 kd[4];

layout(location = 0) out vec3 vertNormal;
//layout(location = 1) out vec3 lightDir;

layout(location = 1) out vec3 eyeDir;
layout(location = 2) out vec2 textureCoords; 
layout(location = 3) out vec3 LightsDir[4]; // takes location 3,4,5
layout(location = 7) out vec4 Lights_ks[4]; // takes location 6,7,8
layout(location = 11) out vec4 Lights_kd[4]; // takes location 9,10,11

void main() {
    textureCoords = uvCoord;
    mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
    vertNormal = normalize(normalMatrix * vNormal); /// Rotate the normal to the correct orientation 
    vec3 vertPos = vec3(viewMatrix * modelMatrix * vVertex);
    vec3 vertDir = normalize(vertPos);
    eyeDir = -vertDir;

    for (int i = 0; i < 4; i++) {
        LightsDir[i] = normalize(vec3(Lights[i]) - vertPos); 
        Lights_ks[i] = ks[i];
        Lights_kd[i] = kd[i];
    }


    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vVertex;
    
}
