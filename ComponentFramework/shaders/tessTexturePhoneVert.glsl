#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 uvCoord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec2 uvCoordFromVert;
out vec3 normalFromVert;
out float vertDistance;
void main() {
    uvCoordFromVert = uvCoord;
    /// This is a hack to just start LOD 
    vertDistance = length(viewMatrix * modelMatrix * vVertex);
    gl_Position =  vVertex;
}
