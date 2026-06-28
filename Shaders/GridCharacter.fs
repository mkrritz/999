#version 430 core

in vec2 UV;

layout(binding = 0) uniform sampler2D Sampler;

out vec4 FragColor;

const float FILL = 0.9;
const float BORDER = 0.8;

const vec3  FILL_COLOR = vec3(0.647, 0.0, 0.125);
const vec3  BORDER_COLOR = vec3(0.78, 0.173, 0.255);
const vec3  QUAD_COLOR = vec3(0.1, 0.24313, 0.21568);

void main() {
    float d = texture(Sampler, UV).r;
    float aaf = fwidth(d);
    float fill  = smoothstep(FILL - aaf, FILL + aaf, d);
    float border = smoothstep(BORDER - aaf, BORDER + aaf, d);

    FragColor = vec4(mix(BORDER_COLOR, FILL_COLOR, fill), border);
    
    FragColor += vec4(QUAD_COLOR, 0.2);
    //FragColor = vec4(UV, 0, 1);
}