
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/Font.h"

#include "stb_image/stb_image.h"

using std::string;
using std::filesystem::path;

static const path FONT_DIRECTORY = "Fonts/";

static const glm::vec2 FontUV[64] = {
    glm::vec2(0    , 0.875), // 0
    glm::vec2(0.125, 0.875), // 1
    glm::vec2(0.25 , 0.875), // 2
    glm::vec2(0.375, 0.875), // 3
    glm::vec2(0.5  , 0.875), // 4
    glm::vec2(0.625, 0.875), // 5
    glm::vec2(0.75 , 0.875), // 6
    glm::vec2(0.875, 0.875), // 7
    glm::vec2(0    , 0.75 ), // 8
    glm::vec2(0.125, 0.75 ), // 9
    glm::vec2(0.25 , 0.75 ), // A
    glm::vec2(0.375, 0.75 ), // B
    glm::vec2(0.5  , 0.75 ), // C
    glm::vec2(0.625, 0.75 ), // D
    glm::vec2(0.75 , 0.75 ), // E
    glm::vec2(0.875, 0.75 ), // F
    glm::vec2(0    , 0.625), // G
    glm::vec2(0.125, 0.625), // H
    glm::vec2(0.25 , 0.625), // I
    glm::vec2(0.375, 0.625), // J
    glm::vec2(0.5  , 0.625), // K
    glm::vec2(0.625, 0.625), // L
    glm::vec2(0.75 , 0.625), // M
    glm::vec2(0.875, 0.625), // N
    glm::vec2(0    , 0.5  ), // O
    glm::vec2(0.125, 0.5  ), // P
    glm::vec2(0.25 , 0.5  ), // Q
    glm::vec2(0.375, 0.5  ), // R
    glm::vec2(0.5  , 0.5  ), // S
    glm::vec2(0.625, 0.5  ), // T
    glm::vec2(0.75 , 0.5  ), // U
    glm::vec2(0.875, 0.5  ), // V
    glm::vec2(0    , 0.375), // W
    glm::vec2(0.125, 0.375), // X
    glm::vec2(0.25 , 0.375), // Y
    glm::vec2(0.375, 0.375), // Z
    glm::vec2(0.5  , 0.375), // a
    glm::vec2(0.625, 0.375), // b
    glm::vec2(0.75 , 0.375), // c
    glm::vec2(0.875, 0.375), // d
    glm::vec2(0    , 0.25 ), // e
    glm::vec2(0.125, 0.25 ), // f
    glm::vec2(0.25 , 0.25 ), // g
    glm::vec2(0.375, 0.25 ), // h
    glm::vec2(0.5  , 0.25 ), // i
    glm::vec2(0.625, 0.25 ), // j
    glm::vec2(0.75 , 0.25 ), // k
    glm::vec2(0.875, 0.25 ), // l
    glm::vec2(0    , 0.125), // m
    glm::vec2(0.125, 0.125), // n
    glm::vec2(0.25 , 0.125), // o
    glm::vec2(0.375, 0.125), // p
    glm::vec2(0.5  , 0.125), // q
    glm::vec2(0.625, 0.125), // r
    glm::vec2(0.75 , 0.125), // s
    glm::vec2(0.875, 0.125), // t
    glm::vec2(0    ,     0), // u
    glm::vec2(0.125,     0), // v
    glm::vec2(0.25 ,     0), // w
    glm::vec2(0.375,     0), // x
    glm::vec2(0.5  ,     0), // y
    glm::vec2(0.625,     0), // z
    glm::vec2(0.75 ,     0), // Space
    glm::vec2(0.875,     0), // w/e 
};

static uint8_t char_to_normalized_ascii(char c) {
    if (c >= '0' && c <= '9') c -= '0';
    else if (c >= 'A' && c <= 'Z') c -= ('A' - 10);
    else if (c >= 'a' && c <= 'z') c -= ('a' - 36);
    else if (c == ' ') c = 62;
    else {
        printf("Character '%c' not supported by font.\n", c);
        c = 62;
    }
    return (uint8_t) c;
}

static const string FONTS[(uint32_t)FontType::Count] = {
    // Keep these in the same order as the 'enum struct FontType'
    "font_1.png",
    //"font_1.png",
    //"font_1.png",
};

static void LoadFont(const std::string& name) {
    path font_path = FONT_DIRECTORY / name;
    int W, H, C;
    unsigned char *img = stbi_load(font_path.string().c_str(), &W, &H, &C, 1);
    ASSERT (!img, "Font image missing.");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, W, H, 0, GL_RED, GL_UNSIGNED_BYTE, img);
    stbi_image_free(img);
}

void FontsInit(Heap* const heap,
               Font* fonts[]) {
    stbi_set_flip_vertically_on_load(true);
    for (uint32_t index = 0; index < (uint32_t)FontType::Count; index++) {
        fonts[index] = (Font*)HeapAllocate(heap, sizeof(Font));
        glGenTextures(1, &fonts[index]->texture);
        glBindTexture(GL_TEXTURE_2D, fonts[index]->texture);
        LoadFont(FONTS[index]);
    }
    stbi_set_flip_vertically_on_load(false);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Replace this with glBindTextureUnit ???
}

glm::vec2 GetAtlasUV(char c) {
    uint8_t index = char_to_normalized_ascii(c);
    return FontUV[index];
}
