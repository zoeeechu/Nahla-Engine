#version 450
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec4 inVertex;
layout(location = 1) in vec4 inNormal;
layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 modelMatrix;
out vec3 incident; 
out vec3 normal;
out vec3 eyeDir;
void main() {

    vec3 vertexPosWorldSpace = (modelMatrix * inVertex).xyz; 
    vec3 vertPos = vec3(viewMatrix * modelMatrix * inVertex);
    vec3 vertDir = normalize(vertPos);
    eyeDir = -vertDir;
    vec3 cameraPos = vec3(0.0, 0.0, 15.0); 
    incident = normalize(vertexPosWorldSpace - cameraPos);
    mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
    normal = normalize(normalMatrix * inNormal.xyz); 
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * inVertex;
}
