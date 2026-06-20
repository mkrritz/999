#version 430 core

#define TWOPI 6.2831850
#define PI    3.1415925

#include "UniformBufferObjects/Scene.glsl"

struct CharacterQuad {
    vec4 rect;          // Are both rect and uv_rect required?
    vec4 uv_rect;       // Where is the this buffer populated? (SetText)
    vec4 ndc;
    // rect is xy NDC position and zw width and height in NDC
    //         xy in range [-1, 1] zx in range [0, 2]
    //              Similar to how 'enum struct Quads' are setup.
    //
    // uv_rect xy is the starting uv position of the character 
    //          and zw is just 0.125 because the Font image
    //          stores the characters in a 8x8 grid.
};

layout(std430, binding = 3) readonly buffer CharacterQuads {
    CharacterQuad quads[];
};

out vec2 UV;

vec3 PulseEffect(vec3 p) {
    float period = 1.0;
    float s = 1.0 + 0.01 * sin(TWOPI * TotalTime / period);
    return p * s;
}

vec3 WaveEffect(vec3 p) {
    float frequency = 1.0;
    float strength = 0.105;
    p.y += strength * sin(((TWOPI * frequency) * p.x) + TotalTime);
    return p;
}

void main() {
    UV = vec2((gl_VertexID >> 1) & 1, gl_VertexID & 1);
    
    CharacterQuad quad = quads[gl_InstanceID];
    
    vec2 Position = quad.ndc.xy + quad.rect.xy + UV * quad.rect.zw;
    //Position = PulseEffect(Position);
    //Position = WaveEffect(Position);
    
    gl_Position = vec4(Position, 0.0, 1.0);
    UV = quad.uv_rect.xy + UV * quad.uv_rect.zw;
}
