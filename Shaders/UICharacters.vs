#version 430 core

out vec2 UV;

#include "UniformBufferObjects/Scene.glsl"

#include "Shared/CharacterQuad.glsl"
layout(std430, binding = 3) readonly buffer CharacterQuads {
    CharacterQuad quads[];
};

#include "UniformBufferObjects/UI.glsl"

void main() {
    UV = vec2((gl_VertexID >> 1) & 1, gl_VertexID & 1);
    
    CharacterQuad quad = quads[gl_InstanceID];
    const vec2 CharacterPosition   = quad.rect.xy;
    const vec2 CharacterSize       = quad.rect.zw;
    const vec2 CharacterUVPosition = quad.uv_rect.xy;
    const vec2 CharacterUVSize     = quad.uv_rect.zw;
    const vec2 CellCenter          = quad.ndc.xy;
    
    vec2 Position = vec2(NDCX, NDCY) + CharacterPosition + UV * CharacterSize;
    
    gl_Position = vec4(Position, 0.0, 1.0);
    UV = CharacterUVPosition + UV * CharacterUVSize;
}

/*
    This shader renders many quads.
    Each positioned at the location assigned in the UIUBO. (Similar to the simple Quad.vs)
    This allows multiple quads in a cell. 
                            (
                                There is no cell concept here though.
                                Except that the cell is the quad being rendered.
                                Regarding GridCharacters.vs:
                                    Each cell contains many quads,
                                    but we shouldn't have one UBO per cell.
                                    So there's that. But we probably also shouldn't
                                    have one UBO per UI element.
                                So maybe one grouping is the quads that must be placed
                                somewhere relative to the playing grid. And one grouping
                                for the UI elements (score, time, level, etc.) which are
                                placed relative to the NDC.
                            )
                            (
                                TODO: Render this [[[ Level # ]]]
                                A sentence and a multidigit number.
                                Instead of only the multidigit number.
                            )
    Each quad carries its own CellCenter (But it's useless here) as well as
    the local position and the size of the vertices
    and the sampling point and the sampling size for that character 
*/

