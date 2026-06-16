#version 430 core

in vec2 UV;
out vec4 FragColor;

#include "UniformBufferObjects/Scene.glsl"

const float DURATION = 6.0;
const vec3 color1 = vec3(1.0, 0.0, 0.0);
const vec3 color2 = vec3(1.0, 1.0, 0.0);
const float TWOPI = 6.2831850;
const float PI    = 3.1415925;
const float HPI   = 1.5707963;

const float R = 0.45;  // Radius
const float T = 0.025; // Thickness
const float INNER_CIRCLE = R - T;
const float OUTER_CIRCLE = R + T;
const float START_ANGLE = HPI;

void main() {
    vec2 centerUV = UV - 0.5; // (...)*2?
    float dist = length(centerUV); // just UV?
    
    float edge  = fwidth(5*dist);
    float RING  = smoothstep(1*INNER_CIRCLE - 5*edge, 1*INNER_CIRCLE + 5*edge, 1*dist) *
           (1.0 - smoothstep(1*OUTER_CIRCLE - 1*edge, 1*OUTER_CIRCLE + 1*edge, 1*dist));
    
    float fragAngle = atan(centerUV.y, centerUV.x);
    if (fragAngle < 0.0) fragAngle += TWOPI;
    float endAngle = fract((DURATION - TotalTime) / DURATION) * TWOPI;
    float delta = START_ANGLE - fragAngle;
    if (delta < 0.0) delta += TWOPI;
    float TIMER = step(delta, endAngle);
    
    //FragColor = vec4(UV, 0, 1);
    //FragColor = vec4(centerUV, 0, 1);
    //FragColor = vec4(UV, 0, RING);
    //FragColor = vec4(UV, 0, TIMER);
    FragColor = vec4(mix(color1, color2, dist), RING * TIMER);
    //FragColor = vec4(dist,dist,dist, RING * TIMER);
}

//atan

//fwidth
//smoothstep
//mix