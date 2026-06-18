#version 430 core

in vec2 UV;
out vec4 FragColor;

#include "UniformBufferObjects/Scene.glsl"

const float DURATION = 6.0;
const float PI    = 3.1415925;
const float HPI   = 1.5707963;
const float TWOPI = 6.2831850;

const vec3 color1 = vec3(1,0,0);
const vec3 color2 = vec3(1,1,0);

const float Radius    = 0.45;
const float Thickness = 0.025;

void main() {
    vec2 Center = UV - 0.5;
    float Distance = length(Center);
    float AA = fwidth(5.0 * Distance);
    
    float ring =
        smoothstep(Radius - Thickness - 0.0*AA, Radius - Thickness + 1.0*AA, Distance) *
        (1.0 - smoothstep(Radius + Thickness - 1.0*AA, Radius + Thickness + 15.0*AA, Distance));
    
    float angle = fract((HPI - atan(Center.y, Center.x)) / TWOPI);
    float timer = step(angle, fract((DURATION - TotalTime) / DURATION));
    
    FragColor = vec4(mix(color1, color2, Distance), ring * timer);
}