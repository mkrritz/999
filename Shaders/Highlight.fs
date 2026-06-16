#version 430 core

in vec2 UV;

out vec4 FragColor;

#include "UniformBufferObjects/Scene.glsl"

const vec3  FILL_COLOR = vec3(0.5, 0.5, 0.5);

void main()
{
    float a_1 = 0.01 * (UV.y-0.5) * (UV.x-0.5) * TotalTime;
    float a_2 = 0.4 * cos(TotalTime * 1.0);
    float a_3 = a_2 * abs(a_1);
    
    float a_4 = clamp(abs(a_3), 0.01, 0.99); // do better
    vec3 highlight = vec3(a_1, a_2, a_3);
    
    FragColor = vec4(FILL_COLOR + highlight, 0.5 + a_4);
    
    //FragColor = vec4(UV, 0, 1);
}