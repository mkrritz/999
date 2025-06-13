#version 430 core

in vec2 UV;

uniform sampler2D Sampler;

out vec4 FragColor;

const float FILL = 0.8;
const float BORDER = FILL * 0.7;
const vec3  FILL_COLOR = vec3(0.0353,0,0.2);
// const vec3  BORDER_COLOR = vec3(0.752941);
const vec3  BORDER_COLOR = vec3(1.0, 0.84313, 0.21568);

void main() {
    float d = texture(Sampler, UV).r;
    float aaf = fwidth(d);
    float fill  = smoothstep(FILL - aaf, FILL + aaf, d);
    float border = smoothstep(BORDER - aaf, BORDER + aaf, d);

    FragColor = vec4(mix(BORDER_COLOR, FILL_COLOR, fill), border);
    //FragColor = vec4(BORDER_COLOR, 0.6);
}