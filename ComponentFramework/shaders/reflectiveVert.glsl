#version 460 core
// Input from vertex buffer
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

// Output to fragment shader
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Transform vertex position to clip space
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // Calculate world-space coordinates for fragment shader
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Pass normal and texture coordinates to fragment shader
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
}
