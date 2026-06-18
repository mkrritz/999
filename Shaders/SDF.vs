#version 430 core

#define TWOPI 6.2831850
#define PI    3.1415925

layout(location = 0) in vec3 _Position;
layout(location = 1) in vec2 _UV;

#include "UniformBufferObjects/Scene.glsl"

out vec2 UV;

vec3 PulseEffect() {
    float period = 1.0;
    float s = 1.0 + 0.01 * sin(TWOPI * TotalTime / period);
    return _Position * s;
}

vec3 WaveEffect() {
    vec3 p = _Position;
    float frequency = 1.0;
    float strength = 0.105;
    
    p.y += strength * sin(((TWOPI * frequency) * _Position.x) + TotalTime);
    
    return p;
}

void main() {
    vec3 Position = _Position;
    //Position = PulseEffect();
    //Position = WaveEffect();
    
    gl_Position = vec4(Position, 1.0);
    UV = _UV;
}
