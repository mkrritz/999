#ifndef UI_HPP
#define UI_HPP

static const float SCREEN_WINDOW_X = -1.0f;
static const float SCREEN_WINDOW_Y = -1.0f;
static const float SCREEN_WINDOW_W =  2.0f;
static const float SCREEN_WINDOW_H =  2.0f;

// The gameplay screen can be placed wherever, so these remain defines
// The whole renderer here works pretty much exclusively in NDC space
// The grid is a quad the needs to cover the entire screen so it can
// overlap with the text, which is a bunch of quads, also with vertices
// in NDC space.
#define GAME_SCREEN_WINDOW_X -0.8f
#define GAME_SCREEN_WINDOW_Y -0.8f
#define GAME_SCREEN_WINDOW_W  1.6f
#define GAME_SCREEN_WINDOW_H  1.6f

/*
#define SETTINGS_SCREEN_WINDOW_X -0.8f
#define SETTINGS_SCREEN_WINDOW_Y -0.8f
#define SETTINGS_SCREEN_WINDOW_W  1.6f
#define SETTINGS_SCREEN_WINDOW_H  1.6f

#define MENU_SCREEN_WINDOW_X -0.8f
#define MENU_SCREEN_WINDOW_Y -0.8f
#define MENU_SCREEN_WINDOW_W  1.6f
#define MENU_SCREEN_WINDOW_H  1.6f
*/

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