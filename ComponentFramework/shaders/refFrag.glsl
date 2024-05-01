#version 450
#extension GL_ARB_separate_shader_objects : enable
in vec3 incident;
in vec3 normal;
in vec3 eyeDir;
uniform samplerCube skyboxTexture;
layout(location = 0) out vec4 fragColor;
void main() {
    vec3 reflectionDirection = reflect(incident, normal);
    vec3 refractionDirection = refract(incident, normal, 1.0/1.33);
   // fragColor = texture(skyboxTexture, reflectionDirection);  
   vec4 reflectionTexture = texture(skyboxTexture, reflectionDirection);  
   vec4 refractionTexture = texture(skyboxTexture, refractionDirection);
   fragColor = mix(reflectionTexture, refractionTexture, dot(incident, eyeDir));
}