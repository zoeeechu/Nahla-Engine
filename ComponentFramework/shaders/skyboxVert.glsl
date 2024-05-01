#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 inVertex;


layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 viewMatrix;


out vec3 uvwCoords;


void main() {
    
    uvwCoords = inVertex;
    gl_Position = projectionMatrix * viewMatrix  * vec4(inVertex, 1.0);

}