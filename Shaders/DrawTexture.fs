#version 430 core

in vec2 UV;

out vec4 FragColor;

uniform sampler2D Sampler;
uniform float Time;

void main()
{
    vec4 Color = texture(Sampler, UV);
    
    //Color = texture(Sampler, UV * abs(sin(Time)) + 0.3);
    //Color.x += fract(Time);
    
    //Color.x = 1 - Color.x;
    FragColor = Color;
    
    //FragColor = vec4(UV, 0, 1);
}