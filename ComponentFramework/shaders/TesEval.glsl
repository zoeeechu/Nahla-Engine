#version 450
layout (triangles, equal_spacing, ccw) in;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform sampler2D textureData;
in vec2 uvCoordFromCtrl[];
in vec3 normalFromCtrl[];
out vec2 uvCoordFromEval;
out vec3 normalFromEval;

vec2 interpolateVec2(vec2 v0, vec2 v1, vec2 v2){
    return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 = gl_TessCoord.z * v2;
}

vec3 interpolateVec3(vec2 v0, vec2 v1, vec2 v2){
    return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 = gl_TessCoord.z * v2;
}

vec4 interpolateVec4(vec2 v0, vec2 v1, vec2 v2){
    return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 = gl_TessCoord.z * v2;
}


void main(){
    uvCoordFromEval = interpolateVec2(uvCoordFromCtrl[0], uvCoordFromCtrl[1], uvCoordFromCtrl[2]);
    normalFromEval = interpolateVec3(normalFromCtrl[0], normalFromCtrl[1], normalFromCtrl[2]);

    vec4 positions = interpolateVec4(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);
   // vec4 height = vec4(texture(textureData,uvCoordFromEval));

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * (positions);
}