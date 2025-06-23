#version 430 core

in vec2 UV;
out vec4 FragColor;

uniform float Time; // Time in seconds

// Constants
const float DURATION = 10.0;
const vec4 color1 = vec4(1.0, 0.0, 0.0, 1.0); // Red
const vec4 color2 = vec4(1.0, 1.0, 0.0, 1.0); // Yellow

// Gradient function
vec4 getGradientValue(float x)
{
    float val = clamp((x + 1.0) * 0.5, 0.0, 1.0); // Map UV.x from [-1,1] to [0,1]
    vec4 color = mix(color1, color2, val);
    color.a = clamp(color.a, min(color1.a, color2.a), max(color1.a, color2.a));
    return color;
}

void main()
{
    // Calculate progress from 0 to 1 over 5 seconds
    float progress = clamp(mod(Time, DURATION) / DURATION, 0.0, 1.0);

    // Convert UV.x from [-1, 1] to [0, 1]
    float uvX = (UV.x + 1.0) * 0.5;

    // Compute base color from gradient
    vec4 color = getGradientValue(UV.x);

    // Apply alpha based on whether this pixel is within progress region
    float alpha = smoothstep(0.01, 0.05,  uvX - progress);
    FragColor = vec4(color.rgb, color.a * alpha);
}
