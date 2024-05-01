#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec4 vVertex;


layout (std140, binding = 0) uniform CameraMatricies {
    mat4 projection;
    mat4 view;
};

layout(location = 1) uniform mat4 modelMatrix;

void main() {
    gl_Position = projection * view * modelMatrix * vVertex;
}