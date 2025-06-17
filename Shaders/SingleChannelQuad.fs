#version 430 core

in vec2 UV;

uniform float Resolution;

out vec4 FragColor;

const vec3 Color1 = vec3(0.961, 0.961, 0.863);
const vec3 Color2 = vec3(0.294, 0.169, 0.125);

void main() {
    int x = int(Resolution * UV.x);
    int y = int(Resolution * UV.y);
    int c = (x + y) & 1;
    FragColor = vec4(c * Color1 + (1-c) * (1-Color2), 1.0);
}

