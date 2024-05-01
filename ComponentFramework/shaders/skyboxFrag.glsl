#version 450
#extension GL_ARB_separate_shader_objects : enable



layout(location = 0) out vec4 fragColor;

in vec3 uvwCoords;
uniform samplerCube skyboxTexture;


void main() {
    vec3 uvwCoordsFix = uvwCoords;
    uvwCoordsFix.x *= -1.0;
    //fragColor = vec4(1.0,0.0,0.0,0.0); 
     fragColor = texture(skyboxTexture, uvwCoordsFix);
}