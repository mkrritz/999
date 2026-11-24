#ifndef SHADER_HPP
#define SHADER_HPP

#include <Focus.h>

enum struct Shaders {
    DrawTexture,
    Grid,
    Highlight,
    SDF,
    Timer,
    Level,
    Count // Not really a shader.
};

extern ShaderConfig Programs[];

#endif
