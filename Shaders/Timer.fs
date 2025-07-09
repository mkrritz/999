#version 430 core

in vec2 UV;
out vec4 FragColor;

uniform float Time;

const float DURATION = 6.0;
const float radius = 0.475;
const float thickness = 0.0125;
const vec3 color1 = vec3(1.0, 0.0, 0.0); // Red
const vec3 color2 = vec3(1.0, 1.0, 0.0); // Yellow
const float TWOPI = 6.2831850;
const float PI    = 3.1415925;

void main() {
    vec2 centerUV = UV - 0.5;
    float dist = length(centerUV);

    // Ring band mask
    float inner = radius - thickness;
    float outer = radius + thickness;
    float edge  = fwidth(dist);
    float ringMask = smoothstep(1*inner - 5*edge, 1*inner + 5*edge, 1*dist) *
              (1.0 - smoothstep(1*outer - 1*edge, 1*outer + 1*edge, 1*dist));

    // --- Angle calculations ---
    float fragAngle = atan(centerUV.y, centerUV.x); // [-π, π]
    if (fragAngle < 0.0) fragAngle += TWOPI;

    float startAngle = PI / 2.0;
    float endAngle = mod((DURATION - Time )/ DURATION, 1.0) * TWOPI;

    // Clockwise angular distance from startAngle to fragment
    float delta = startAngle - fragAngle;
    if (delta < 0.0) delta += TWOPI;

    // Alpha is 1 inside the sector from 12 o'clock to moving hand
    float alpha = step(delta, endAngle); // 1.0 if frag is within sweep

    // Final color
    vec3 color = mix(color1, color2, ringMask);
    FragColor = vec4(color, ringMask * alpha);
}
