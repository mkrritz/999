#ifndef UI_HPP
#define UI_HPP

enum struct Quads {
    FINAL_SCREEN,
    GAME_SCREEN,
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