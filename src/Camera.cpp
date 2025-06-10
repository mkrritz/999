
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
    
    camera->aspect         = WINDOW_WIDTH / WINDOW_HEIGHT;
    camera->render_passes  = 2;
    
    glm::vec4 position(150.0f, 100.0f, 150.0f, 1.0f);
    
    glCreateBuffers(1, &camera->position_buffer);
    glNamedBufferStorage(camera->position_buffer,
                         sizeof(glm::vec4),
                         &position[0],
                         GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    camera->position = (glm::vec4*)glMapNamedBufferRange(camera->position_buffer,
                                                         0,
                                                         sizeof(glm::vec4),
                                                         GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    
    camera->sampler_clamp_nearest  = Sampler(GL_CLAMP_TO_EDGE, GL_NEAREST);
    camera->sampler_clamp_linear   = Sampler(GL_CLAMP_TO_EDGE, GL_LINEAR);
    camera->sampler_repeat_linear  = Sampler(GL_REPEAT, GL_LINEAR);
    
    glGenVertexArrays(1, &camera->vao);
    
}

