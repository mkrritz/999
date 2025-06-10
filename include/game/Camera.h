#ifndef CAMERA_HPP
#define CAMERA_HPP

static const uint32_t FIRST_PASS = 0;
static const uint32_t SECOND_PASS = 1;

static glm::vec3 Up = glm::vec3(0,1,0);

struct Camera {
    glm::vec4*   position;
    GLuint       position_buffer;
    glm::mat4*   vp;
    GLuint       view_projection_buffer;
    GLuint       view_buffer;
    glm::vec4*   forward;
    GLuint       forward_buffer;
    
    GLuint       vao;
    
    GLuint       sampler_clamp_nearest;
    GLuint       sampler_clamp_linear;
    GLuint       sampler_repeat_linear;
    
    float        aspect;
    uint32_t     render_passes;
};

#endif