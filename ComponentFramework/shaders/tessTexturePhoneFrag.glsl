#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 1) in vec3 lightDir;
layout(location = 2) in vec3 eyeDir; 
layout(location = 3) in vec2 textureCoords; 
layout(location = 4) in float FogValFromEval; 
out vec4 fragColor;

layout(binding=2) uniform sampler2D textureData;
layout(binding = 1) uniform sampler2D normalMap;

void main() {
    vec4 fogColor = vec4(0.8, 0.5, 0.2, 0.0);
    vec4 ks = vec4(0.3, 0.3, 0.3, 0.0);
    vec4 kd = vec4(0.6, 0.6, 0.3, 0.0);
    vec4 ka = 0.1 * kd;
    
    vec2 uvCoordFromEval = textureCoords; // Assuming textureCoords contain UV coordinates
    vec3 normalFromEval = normalize(texture(normalMap, uvCoordFromEval).rgb * 2.0 - 1.0);

    float diff = max(dot(normalFromEval, lightDir), 0.0);

    vec3 reflection = normalize(reflect(-lightDir, normalFromEval));
    float spec = max(dot(eyeDir, reflection), 0.0);
    spec = pow(spec, 14.0);

    vec4 kt = texture(textureData, uvCoordFromEval);
    vec4 outputColor = kt * (ka + (diff * kd) + (spec * ks));
    fragColor = mix(fogColor, outputColor, FogValFromEval);
}
