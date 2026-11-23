#include "pch.h"

#include "game/Shader.h"

extern const ShaderConfig FocusShaderPrograms[] = {
    { "Quad.vs",       nullptr, "DrawTexture.fs",   nullptr, nullptr, nullptr },
    { "Quad.vs",       nullptr, "Grid.fs",          nullptr, nullptr, nullptr },
    { "Quad.vs",       nullptr, "Highlight.fs",     nullptr, nullptr, nullptr },
    { "Characters.vs", nullptr, "GridCharacter.fs", nullptr, nullptr, nullptr },
    { "Quad.vs",       nullptr, "Timer.fs",         nullptr, nullptr, nullptr },
    { "Characters.vs", nullptr, "UICharacter.fs",   nullptr, nullptr, nullptr },
};

extern const size_t FocusShaderProgramCount =
    sizeof(FocusShaderPrograms) / sizeof(FocusShaderPrograms[0]);

static_assert(sizeof(FocusShaderPrograms) / sizeof(FocusShaderPrograms[0]) ==
              (size_t)Shaders::Count,
              "The enum and the Focus table must list the same programs.");
