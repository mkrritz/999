#ifndef TEXT_HPP
#define TEXT_HPP

#define TEXT_VERTEX_ATTRIBUTE_COUNT 5

enum struct FontType {
    // Keep these in the same order as the FONTS array
    // in order to have the same kind of indexing as with 
    // heaps and shaders. more to come, or not, so idk yet.
    English,
    Count
};

struct Font {
    GLuint texture;
};

struct TextInfo {
    uint32_t length;
    uint32_t height;
    uint32_t width;
};

struct TextField {
    GLuint vao, vbo, ibo;
    glm::mat4 transform;
    
    std::string text;
    TextInfo info;
};

#endif

