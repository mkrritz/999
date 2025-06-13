#version 430 core

#define TWOPI 6.2831850
#define PI    3.1415925

layout(location = 0) in vec3 _Position;
layout(location = 1) in vec2 _UV;

//layout(std430, binding = 6) readonly buffer CameraVP {
//    mat4 VP;
//};

uniform float Time;

out vec2 UV;

/* TODO:
    Camera distance from Position based
    Instead of .x, figure out maths based on right up and sign i guess, easy stuff
    
*/

float PulseEffect() {
    float period = 2.0;
    float s = 1.0 + 0.01 * sin(TWOPI * Time / period);
    return s;
}

/* 

vec3 ScreamEffect() {
    float frequency = 0.0125 / 2.0;
    float strength = 50.0;
    float t = strength * clamp(sin(((-TWOPI * frequency) * _Position.x) + Time) - 0.9, 0.0, 1.0);
    return mix(vec3(0), _Position + vec3(1, 1, 0), t);
}

vec3 PopUpEffect() {
    const float offset_based_on_max_length_of_rows = 1.0; // or just 0, both
    float frequency = 0.0125;
    float strength = 8.0;
    float t = strength * sin(((-TWOPI * frequency) * _Position.x) + Time);
    return mix(vec3(0,-1, 0), _Position + vec3(offset_based_on_max_length_of_rows, 1, 0), clamp(t, 0.0, 1.0));
}

vec3 WaveEffect() {
    vec3 p = _Position;
    float frequency = 0.125;
    float strength = 0.5;
    
    p.y += strength * sin(((TWOPI * frequency) * _Position.x) + Time);
    
    return p;
}

*/

void main() {
    //mat4 T = mat4(1.0);
    //vec3 p = Position + 2.0 * SignNW * Right + 2.0 * Up;
    //T[3] = vec4(p, 1.0);
    //
    //mat4 R = mat4(1.0);
    //float angle = radians(90.0 * (Right.z + SignNW + 1.0));
    //float f1 = cos(angle);
    //float f2 = sin(angle);
    //R[0][0] =  f1;
    //R[0][2] = -f2;
    //R[2][0] =  f2;
    //R[2][2] =  f1;
    //
    //uint text_type = uint(_Position.z);
    //vec3 LocalPosition = vec3(0);
    float s = 1.0;
    //if (text_type < 1u) {
    s = PulseEffect();
    //    LocalPosition +=  _Position;
    //} else if (text_type < 2u) {
    //    LocalPosition +=  WaveEffect();
    //} else if (text_type < 3u) {
    //    LocalPosition += PopUpEffect();
    //} else if (text_type < 4u) {
    //    LocalPosition += ScreamEffect();
    //} else {
    //    LocalPosition +=  _Position;
    //}
    //
    //mat4 S = mat4(1.0);
    //S[0][0] = s;
    //S[1][1] = s;
    //S[2][2] = 0.0;
    //
    //mat4 Transform = T * R * S;
    //mat4 MVP = VP * Transform;
    
    
    //gl_Position = MVP * vec4(LocalPosition, 1.0);
    gl_Position = vec4(_Position * s, 1.0);
    UV = _UV;
}
