#version 430 core

in vec2 UV;

out vec4 FragColor;

uniform sampler2D Sampler;

#include "UniformBufferObjects/Scene.glsl"

void main()
{
    vec4 Color = texture(Sampler, UV);
    
    //Color = texture(Sampler, UV * abs(sin(TotalTime)) + 0.3);
    //Color.x += fract(TotalTime);
    
    //Color.x = 1 - Color.x;
    FragColor = Color;
    //FragColor.a = 0.25;
    
    //FragColor = vec4(UV, 0, 1);
}