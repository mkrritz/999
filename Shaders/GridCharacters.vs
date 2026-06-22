#version 430 core

out vec2 UV;

#include "UniformBufferObjects/Scene.glsl"

#include "Shared/CharacterQuad.glsl"
layout(std430, binding = 3) readonly buffer CharacterQuads {
    CharacterQuad quads[];
};



void main() {
    UV = vec2((gl_VertexID >> 1) & 1, gl_VertexID & 1);
    
    CharacterQuad quad = quads[gl_InstanceID];
    const vec2 CharacterPosition   = quad.rect.xy;
    const vec2 CharacterSize       = quad.rect.zw;
    const vec2 CharacterUVPosition = quad.uv_rect.xy;
    const vec2 CharacterUVSize     = quad.uv_rect.zw;
    const vec2 CellCenter          = quad.ndc.xy;
    
    vec2 Position = CellCenter + CharacterPosition + UV * CharacterSize;
    
    gl_Position = vec4(Position, 0.0, 1.0);
    UV = CharacterUVPosition + UV * CharacterUVSize;
}

/*
    This shader renders many quads. Each positioned at CellCenter with variations.
    This allows multiple quads in a cell.
    Each quad carries its own CellCenter as well as
    the local position and the size of the vertices
    and the sampling point and the sampling size for that character 
*/