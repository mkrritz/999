#include "pch.h"

#include "game/Shader.h"

ShaderConfig Programs[] = {
    { "Quad.vs",       nullptr, "DrawTexture.fs",   nullptr, nullptr, nullptr },
    { "Quad.vs",       nullptr, "Grid.fs",          nullptr, nullptr, nullptr },
    { "Quad.vs",       nullptr, "Highlight.fs",     nullptr, nullptr, nullptr },
    { "Characters.vs", nullptr, "GridCharacter.fs", nullptr, nullptr, nullptr },
    { "Quad.vs",       nullptr, "Timer.fs",         nullptr, nullptr, nullptr },
    { "Characters.vs", nullptr, "UICharacter.fs",   nullptr, nullptr, nullptr },
};

static_assert(sizeof(Programs) / sizeof(Programs[0]) == (size_t)Shaders::Count,
              "Mismatch between Programs and 'enum struct Shaders'.");
