#version 430 core

#define TWOPI 6.2831850
#define PI    3.1415925

#include "UniformBufferObjects/Scene.glsl"

struct CharacterQuad {          // Include Shared....
    vec4 rect;
    vec4 uv_rect;
    vec4 ndc;
};

layout(std430, binding = 3) readonly buffer CharacterQuads {
    CharacterQuad quads[];
};

out vec2 UV;

#include "UniformBufferObjects/UI.glsl"

void main() {
    UV = vec2((gl_VertexID >> 1) & 1, gl_VertexID & 1);
    
    CharacterQuad quad = quads[gl_InstanceID];
    
    vec2 Position = vec2(NDCX, NDCY) + quad.rect.xy + UV * quad.rect.zw;
    
    gl_Position = vec4(Position, 0.0, 1.0);
    
    UV = quad.uv_rect.xy + UV * quad.uv_rect.zw;
}



