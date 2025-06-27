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
