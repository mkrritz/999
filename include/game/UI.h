#ifndef UI_HPP
#define UI_HPP

enum struct Quads {
    SCREEN,
    GAME_SCREEN,
    // SETTINGS_SCREEN,
    // MENU_SCREEN,
    HIGHLIGHT,
    Count
};

struct Quad {
    GLuint framebuffer;
    GLuint texture, depthbuffer;
    glm::vec4 bounds;
};

struct UI {
    GLuint quad_vao, vbo, ibo;
    Quad quads[(uint32_t)Quads::Count];
};

#endif