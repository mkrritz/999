#version 430 core

in vec2 UV;

out vec4 FragColor;

uniform sampler2D Sampler;

void main()
{
    float depth = texture(Sampler, UV).r;
    depth = pow(depth, 425); // why is this reasonable? that's the far plane. 0-1 mapped to pow? still in 0-1
    // 300 * 300 = 90000 * 2 = 180000 = 424.26406871192 * 424.26406871192
    FragColor = vec4(vec3(depth), 1.0);
    //FragColor = vec4(vec3(depth-1,1-depth*depth,depth*depth), 1.0);
}
