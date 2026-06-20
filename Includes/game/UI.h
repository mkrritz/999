#ifndef UI_HPP
#define UI_HPP

enum struct Quads {
    Grid,
    Fullscreen,
    Highlight,
    Count
};

enum struct RenderTarget {
    FULLSCREEN,
    GRID,
    // SETTINGS_SCREEN,
    // MENU_SCREEN,
    Count
};

struct UIUBO {
    glm::vec4 rect;
};

struct Quad {
    GLuint framebuffer;
    GLuint texture, depthbuffer;
};

struct UI {
    Quad quads[(uint32_t)RenderTarget::Count];
    
    GLuint ubo_handle[(uint32_t)Quads::Count];
    UIUBO* ubo[(uint32_t)Quads::Count];
};

#endif