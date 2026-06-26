#version 430 core

#define TWOPI 6.283185

out vec2 UV;

#include "UniformBufferObjects/Scene.glsl"

#include "Shared/CharacterQuad.glsl"
layout(std430, binding = 3) readonly buffer CharacterQuads {
    CharacterQuad quads[];
};

#if 1 // Effect 1
vec2 PulseEffect(vec2 pos, vec2 pivot) {
    float period = 2.0;
    float s = 1.0 + 0.05 * sin(TWOPI * TotalTime / period);
    return pivot + (pos - pivot) * s;
}
#endif

#if 1 // Effect 2
float hash12(vec2 p) {
    vec3 p3 = fract(vec3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}
vec2 PulseEffect2(vec2 pos, vec2 pivot, vec2 size) {
    vec2 CellID = floor(pivot / size); // whatever..
    float period = 2.0;
    float h1 = hash12(CellID);
    float h2 = hash12(CellID + 13.37);
    float phase = h1 * 6.283185;
    float amp   = mix(0.13, 0.6, h2);
    float freq  = mix(0.8, 1.2, hash12(CellID + 7.7));
    float s = 1.0 + amp * sin(6.283185 * freq * TotalTime / period + phase);
    return pivot + (pos - pivot) * s;
}
#endif

#if 1 // Effect 3
vec2 WaveEffect(vec2 pos) {
    float frequency = 411.0;
    float strength = 0.025;
    pos.y += strength * sin(((TWOPI * frequency) * pos.x) + TotalTime);
    
    return pos;
}
#endif

#if 1 // Effect 4
vec2 CRTEffect(vec2 pos, vec2 uv, vec2 pivot, vec2 size) {
    vec2 CellID = floor(pivot / size);
    float h = hash12(CellID);
    // per-glyph tuning
    float scanFreq   = mix(30.0, 80.0, hash12(CellID + 1.0));
    float wobbleAmp  = mix(0.0015, 0.016, h);
    float jitterAmp  = mix(0.0005, 0.0012, hash12(CellID + 2.0));
    // horizontal scan wobble (classic CRT beam behavior)
    float scan = sin(uv.y * scanFreq + TotalTime * 6.0 + h * 6.283185);
    pos.x += scan * wobbleAmp;
    // subtle per-glyph jitter (electron instability feel)
    pos.x += (hash12(CellID + floor(TotalTime * 60.0)) - 0.5) * jitterAmp;

    return pos;
}
#endif

void main() {
    UV = vec2((gl_VertexID >> 1) & 1, gl_VertexID & 1);
    
    CharacterQuad quad = quads[gl_InstanceID];
    const vec2 CharacterPosition   = quad.rect.xy;
    const vec2 CharacterSize       = quad.rect.zw;
    const vec2 CharacterUVPosition = quad.uv_rect.xy;
    const vec2 CharacterUVSize     = quad.uv_rect.zw;
    const vec2 CellCenter          = quad.ndc.xy;
    
    vec2 Position = CellCenter + CharacterPosition + UV * CharacterSize;
    
    //Position = PulseEffect(Position, CellCenter);
    //Position = PulseEffect2(Position, CellCenter, CharacterSize);
    //Position = WaveEffect(Position);
    //Position = CRTEffect(Position, UV, CellCenter, CharacterSize);
    
    gl_Position = vec4(Position, 0.0, 1.0);
    UV = CharacterUVPosition + UV * CharacterUVSize;
}

/*
    This shader renders many quads. Each positioned at CellCenter with variations.
    This allows multiple quads in a cell.
    Each quad carries its own CellCenter as well as
    the local position and the size of the vertices
    and the sampling point and the sampling size for that character.
    
    Assuming these 4 effects are 4 different levels.
    How does the setup work? I mean, obviously a uniform and an if on that
    will work just fine. Is this the only way though?
    Another way might be splitting the shaders, Position being calculated
    in a shader pass, then the effect shader pass is drawn and then 
    the final shader pass is run to assign positions and UVs?...
    Maybe not, two passes are way too much as it is...
    
    Another way is to just recompile the shader but each time with a different effect.
    Shader reloading is already a thing anyway. But maybe that's not the way to use it.
    Example: Shader with no effect Level 1->2->3...->MAX_LEVEL
             Reload shader with the CPU adding the effect code.
             And that's it..
    
*/