#version 430 core

#define TWOPI 6.2831850
#define PI    3.1415925

#include "UniformBufferObjects/Scene.glsl"

struct CharQuad {
    vec4 rect;
    vec4 uv_rect;
};

layout(std430, binding = 3) readonly buffer TextQuads {
    CharQuad quads[];
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

const int corner_indices[6] = int[](1, 0, 2, 1, 2, 3);

const vec2 corner_positions[4] = vec2[](
    vec2(0.0, 0.0), // 0: BL
    vec2(0.0, 1.0), // 1: TL
    vec2(1.0, 0.0), // 2: BR
    vec2(1.0, 1.0)  // 3: TR
);

void main() {
    uint quad_idx = gl_VertexID / 6;
    uint vertex_idx = gl_VertexID % 6;
    int corner = corner_indices[vertex_idx];
    vec2 local_pos = corner_positions[corner];
    
    CharQuad q = quads[quad_idx];
    vec3 Position = vec3(q.rect.xy + local_pos * q.rect.zw, 0.0);
    
    //Position = PulseEffect(Position);
    //Position = WaveEffect(Position);
    
    gl_Position = vec4(Position, 1.0);
    UV = q.uv_rect.xy + local_pos * q.uv_rect.zw;
}
