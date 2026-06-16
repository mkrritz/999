#version 430 core

layout(location = 0) in vec2 _UV;

out vec2 UV;

uniform vec2 Position;
uniform vec2 Size;

void main() {
    gl_Position = vec4(Position.x + _UV.x * Size.x,
                       Position.y + _UV.y * Size.y,
                       0.0,
                       1.0);
    UV = _UV;
}


/* 
Replace the above with this

        #version 430 core

        out vec2 UV;

        #include "UniformBufferObjects/UI.glsl"

        void main() {
            UV = vec2((gl_VertexID >> 1) & 1, gl_VertexID & 1);
            
            gl_Position = vec4(NDCX + UV.x * NDCW,
                               NDCY + UV.y * NDCH,
                               0.0, 1.0);
        }

*/