#version 430 core

in vec2 UV;

layout(binding = 0) uniform sampler2D Sampler;

out vec4 FragColor;

const float Border = 0.5;

void main() {
    float Distance = texture(Sampler, UV).r;
    
    float AA = fwidth(Distance);
    float border = smoothstep(Border - AA, Border + AA, Distance);
    
    FragColor = vec4(Distance,Distance,Distance, border);
    //FragColor += vec4(0.1, 0.24313, 0.21568, 0.2);
    
    //FragColor = vec4(UV, 0, 1);
}