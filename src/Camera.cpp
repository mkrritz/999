
#include "pch.h"

#include "game/Assert.h"
#include "game/Window.h"
#include "game/Camera.h"
#include "game/Shader.h"
#include "game/Memory.h"

static GLuint Sampler(GLuint wrap, GLuint filter) {
    GLuint s;
    glGenSamplers(1, &s);
    glSamplerParameteri(s, GL_TEXTURE_WRAP_S, wrap);
    glSamplerParameteri(s, GL_TEXTURE_WRAP_T, wrap);
    glSamplerParameteri(s, GL_TEXTURE_MIN_FILTER, filter);
    glSamplerParameteri(s, GL_TEXTURE_MAG_FILTER, filter); 
    return s;
}

void CameraInit(Heap* const heap) {
    Camera* camera = (Camera*)HeapAllocate(heap, sizeof(Camera));
    camera->sampler_clamp_nearest  = Sampler(GL_CLAMP_TO_EDGE, GL_NEAREST);
    camera->sampler_clamp_linear   = Sampler(GL_CLAMP_TO_EDGE, GL_LINEAR);
    camera->sampler_repeat_linear  = Sampler(GL_REPEAT, GL_LINEAR);
    
}

