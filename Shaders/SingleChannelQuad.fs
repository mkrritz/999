#version 430 core

in vec2 UV;

uniform sampler2D Sampler;

uniform float Resolution;

out vec4 FragColor;

const vec3 Color = vec3(1); // White

void main() {
    int x = int(Resolution * UV.x);
    int y = int(Resolution * UV.y);
    int c = (x + y) & 1;
    FragColor = vec4(c * Color, 1.0);
    //FragColor = vec4(c * Color + (1-c) * (1-Color), 1.0);
    
}

