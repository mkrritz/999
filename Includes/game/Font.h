#ifndef FONT_HPP
#define FONT_HPP

#define TEXT_VERTEX_ATTRIBUTE_COUNT 5

enum struct FontType {
    English,
    Count
};

struct Font {
    GLuint texture;
};

#endif
