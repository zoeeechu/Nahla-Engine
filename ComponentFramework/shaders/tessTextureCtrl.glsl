#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(vertices = 3) out;

uniform float tessLevelInner;
uniform float ppLevel; // Tess level - novels idea sorry scoot 
uniform float tessLevelOuter;

in vec2 uvCoordFromVert[];
in vec3 normalFromVert[];
in float vertDistance[];

out vec2 uvCoordFromCtrl[];
out vec3 normalFromCtrl[];
void main() {
    // Pass through input vertices to tessellation evaluation stage
    gl_out[gl_InvocationID].gl_Position =  gl_in[gl_InvocationID].gl_Position;
    uvCoordFromCtrl[gl_InvocationID] = uvCoordFromVert[gl_InvocationID];
    normalFromCtrl[gl_InvocationID] = normalFromVert[gl_InvocationID];
   float tesslevel = ppLevel;
    // Set tessellation levels
    if (gl_InvocationID == 0) {
        
       // if(vertDistance[0] < 30.0){
           // tesslevel = 20.0;
       // }else if (vertDistance[0] < 50){
          // tesslevel = 5.0;
        //}else{
       //     tesslevel = 1.0;
        //}
        gl_TessLevelInner[0] = tesslevel;

        gl_TessLevelOuter[0] = tesslevel;
        gl_TessLevelOuter[1] = tesslevel;
        gl_TessLevelOuter[2] = tesslevel;

    }
}

