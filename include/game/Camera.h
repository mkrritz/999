#ifndef CAMERA_HPP
#define CAMERA_HPP

struct Camera {
    GLuint   sampler_clamp_nearest;
    GLuint   sampler_clamp_linear;
    GLuint   sampler_repeat_linear;
};

#endif