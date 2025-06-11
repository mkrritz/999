#ifndef UI_HPP
#define UI_HPP

enum struct Quads {
    GAME_SCREEN,
    SAVED_GAME_SCREEN,
    // SETTINGS_SCREEN,
    // MENU_SCREEN,
    Count
};

struct Quad {
    GLuint framebuffer;
    GLuint texture, depthbuffer;
};

struct UI {
    GLuint quad_vao, vbo, ibo;
    Quad quads[(uint32_t)Quads::Count];
};

#endif