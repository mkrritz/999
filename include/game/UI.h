#ifndef UI_HPP
#define UI_HPP

enum struct Quads {
    SCREEN,
    GAME_SCREEN,
    // SETTINGS_SCREEN,
    // MENU_SCREEN,
    Count
};

struct Quad {
    GLuint framebuffer;
    GLuint texture, depthbuffer;
    glm::vec2 position;
    glm::vec2 size;
};

struct UI {
    GLuint quad_vao, vbo, ibo;
    Quad quads[(uint32_t)Quads::Count];
};

#endif