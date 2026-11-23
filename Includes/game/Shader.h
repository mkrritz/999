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

extern const ShaderConfig FocusShaderPrograms[];
extern const size_t FocusShaderProgramCount;

#endif
