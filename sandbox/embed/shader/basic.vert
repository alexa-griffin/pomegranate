#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

layout(location = 2) in float scale;

// layout(set = 0, binding = 0, std140) uniform UniformBufferObject {
//     mat4 matrix;
// } ubo[3];

layout(set = 0, binding = 0, std140) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;


layout(location = 0) out vec4 fragColor;


void main() {
    // gl_Position = ubo[2].matrix * ubo[1].matrix * ubo[0].matrix * vec4(position, 1.0);
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(position, 1.0);

    fragColor = color;
}