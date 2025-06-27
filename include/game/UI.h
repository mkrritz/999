#ifndef UI_HPP
#define UI_HPP

enum struct Quads {
    SCREEN_1,
    SCREEN_2,
    // SETTINGS_SCREEN,
    // MENU_SCREEN,
    Count
};

struct Quad {
    GLuint framebuffer;
    GLuint texture, depthbuffer;
    glm::vec4 position_and_size;
};

struct UI {
    GLuint quad_vao, vbo, ibo;
    Quad quads[(uint32_t)Quads::Count];
};

#endif