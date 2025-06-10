#version 430 core

in vec2 UV;

uniform sampler2D Sampler;

out vec4 FragColor;

const float OneImagePixel = 1.0f / 8.0f;

const vec3 Color = vec3(1); // White

void main() {
    int x = int(UV.x / OneImagePixel);
    int y = int(UV.y / OneImagePixel);
    int c = (x + y) & 1;
    FragColor = vec4(c * Color, 1.0);
    //FragColor = vec4(c * Color + (1-c) * (1-Color), 1.0);
    
}

