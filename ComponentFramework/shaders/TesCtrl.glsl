#version 450
layout(vertices = 3) out;

uniform float tessLevelInner;
uniform float tessLevelOuter;

in vec2 uvCoordFromVert[];
in vec3 normalFromVert[];
in float vertDistance[];

out vec2 uvCoordFromCtrl[];
out vec3 normalFromCtrl[];


void main(){

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    uvCoordFromCtrl[gl_InvocationID] = uvCoordFromVert[gl_InvocationID];
    normalFromCtrl[gl_InvocationID] = normalFromVert[gl_InvocationID];
    float tesslevel = 1.0;

    if(gl_InvocationID == 0){
        tesslevel = 3.0;
        gl_TessLevelInner[0] = tesslevel;

        gl_TessLevelOuter[0] = tesslevel;
        gl_TessLevelOuter[1] = tesslevel;
        gl_TessLevelOuter[2] = tesslevel;
    }
}