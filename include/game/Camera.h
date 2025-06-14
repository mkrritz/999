#ifndef CAMERA_HPP
#define CAMERA_HPP

static const uint32_t FIRST_PASS = 0;
static const uint32_t SECOND_PASS = 1;

static glm::vec3 Up = glm::vec3(0,1,0);

struct Camera {
    uint32_t     render_passes;
    GLuint       sampler_clamp_nearest;
    GLuint       sampler_clamp_linear;
    GLuint       sampler_repeat_linear;
    
    // Game specific stuff, where to store?
    float     resolution;
};

#endif