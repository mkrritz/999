
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/UI.h"
#include "game/Font.h"
#include "game/Input.h"
#include "game/Shader.h"
#include "game/Camera.h"
#include "game/Window.h"

void RenderQuadToScreen(const Shader* const* shaders,
                        const UI* const ui,
                        const Camera* const camera,
                        const GLuint texture) {
    const Shader* shader = shaders[static_cast<uint32_t>(Shaders::DrawTexture)];
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // This means screen rendering.
    glClearColor(1, 0, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBindSampler(0, camera->sampler_clamp_linear);
    glBindTextureUnit(0, texture);
    glBindVertexArray(empty_vao);
    
    glUseProgram(shader->ID);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, ui->ubo_handle[static_cast<uint32_t>(UIIndex::Fullscreen)]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, 0);
    
    glBindTextureUnit(0, 0);
    glBindSampler(0, 0);
    glBindVertexArray(0);
}

void UIInit(Heap* const heap) {
    UI* ui = (UI*)HeapAllocate(heap, sizeof(UI));
    
    {
        Quad& quad = ui->quads[(uint32_t)Quads::FULLSCREEN];
        
        glCreateTextures(GL_TEXTURE_2D, 1, &quad.texture);
        glTextureStorage2D(quad.texture, 1, GL_RGBA8, WINDOW_WIDTH, WINDOW_HEIGHT);
        glCreateTextures(GL_TEXTURE_2D, 1, &quad.depthbuffer);
        glTextureStorage2D(quad.depthbuffer, 1, GL_DEPTH_COMPONENT24, WINDOW_WIDTH, WINDOW_HEIGHT);
        glCreateFramebuffers(1, &quad.framebuffer);
        glNamedFramebufferTexture(quad.framebuffer, GL_COLOR_ATTACHMENT0, quad.texture, 0);
        glNamedFramebufferTexture(quad.framebuffer, GL_DEPTH_ATTACHMENT, quad.depthbuffer, 0);
        
        if(glCheckNamedFramebufferStatus(quad.framebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            ASSERT(true, "Framebuffer Error");
        }
    }
    
    {
        Quad& quad = ui->quads[(uint32_t)Quads::GRID];
        
        glCreateTextures(GL_TEXTURE_2D, 1, &quad.texture);
        glTextureStorage2D(quad.texture, 1, GL_RGBA8, WINDOW_WIDTH, WINDOW_HEIGHT);
        glCreateTextures(GL_TEXTURE_2D, 1, &quad.depthbuffer);
        glTextureStorage2D(quad.depthbuffer, 1, GL_DEPTH_COMPONENT24, WINDOW_WIDTH, WINDOW_HEIGHT);
        glCreateFramebuffers(1, &quad.framebuffer);
        glNamedFramebufferTexture(quad.framebuffer, GL_COLOR_ATTACHMENT0, quad.texture, 0);
        glNamedFramebufferTexture(quad.framebuffer, GL_DEPTH_ATTACHMENT, quad.depthbuffer, 0);
        
        if(glCheckNamedFramebufferStatus(quad.framebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            ASSERT(true, "Framebuffer Error");
        }
    }
    
    glCreateBuffers(static_cast<uint32_t>(UIIndex::Count), ui->ubo_handle);
    for (uint32_t index = 0; index < static_cast<uint32_t>(UIIndex::Count); index++) {
        glNamedBufferStorage(ui->ubo_handle[index],
                             sizeof(UIUBO),
                             nullptr,
                             CPU_CAN_WRITE);
        ui->ubo[index] = (UIUBO*)glMapNamedBufferRange(ui->ubo_handle[index],
                                                       0,
                                                       sizeof(UIUBO),
                                                       CPU_CAN_WRITE);
    }
    ui->ubo[static_cast<uint32_t>(UIIndex::Fullscreen)]->rect = glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f);
    
    // This is set in main...
    ui->ubo[static_cast<uint32_t>(UIIndex::Highlight)]->rect = glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f);
    // This too so the grid can spawn wherever..
    ui->ubo[static_cast<uint32_t>(UIIndex::Grid)]->rect = glm::vec4(-0.8f, -0.8f, 1.6f, 1.6f);
    
}

