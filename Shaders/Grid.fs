#version 430 core

in vec2 UV;

#include "UniformBufferObjects/Scene.glsl"

out vec4 FragColor;

const vec3 Color1 = vec3(0.961, 0.961, 0.863);
const vec3 Color2 = vec3(0.294, 0.169, 0.125);

void main() {
    int x = int(GridResolution * UV.x);
    int y = int(GridResolution * UV.y);
    int c = (x + y) & 1;
    FragColor = vec4(c * Color1 + (1-c) * (1-Color2), 1.0);
    
    //FragColor = vec4(UV, 0, 1);
}

