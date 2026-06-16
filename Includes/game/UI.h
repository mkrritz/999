#ifndef UI_HPP
#define UI_HPP

enum struct UIIndex {      // Rename this to Quads because these carry
    Grid,                  // the position and the size of the quad to be rendered?
    Fullscreen,
    Highlight,
    Count
};

enum struct Quads {               // Renmame to RenderTarget or something?
    FULLSCREEN,                   // Because these carry the texture
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
    Quad quads[(uint32_t)Quads::Count];
    
    GLuint ubo_handle[static_cast<uint32_t>(UIIndex::Count)];
    UIUBO* ubo[static_cast<uint32_t>(UIIndex::Count)];
};

#endif