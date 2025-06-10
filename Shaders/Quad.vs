#version 430 core

layout(location = 0) in vec2 _UV;

out vec2 UV;

uniform vec4 PositionAndSize;

void main() {
    gl_Position = vec4(PositionAndSize.x + _UV.x * PositionAndSize.z,
                       PositionAndSize.y + _UV.y * PositionAndSize.w,
                       0.0,
                       1.0);
    UV = _UV;
}
