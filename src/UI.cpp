
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/UI.h"
#include "game/Shader.h"
#include "game/Camera.h"
#include "game/Window.h"

#define QUAD_VERTEX_ATTRIBUTE_COUNT 2
#define TOTAL_VERTEX_ATTRIBUTE_COUNT (4 * QUAD_VERTEX_ATTRIBUTE_COUNT)
static uint32_t QuadIndices[6] = {1, 0, 2, 1, 2, 3};
static float QuadVertices[TOTAL_VERTEX_ATTRIBUTE_COUNT] = {
    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f
};

void RenderToScreen(const Shader* const shaders[],
                    const Camera* const camera,
                    const UI* const ui,
                    float total_time) {
    const Shader* shader;
    const Quad& quad = ui->quads[(uint32_t)Quads::FINAL_SCREEN]; // Yes that's ok. do better
    //const Quad& quad = ui->quads[(uint32_t)Quads::GAME_SCREEN];  // Yes that's ok. do better
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    glBindVertexArray(ui->quad_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindSampler(0, camera->sampler_clamp_linear);
    
    shader = shaders[(uint32_t)Shaders::TripleChannelQuad];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
        GLuint location = shader->uniform_location[U];
        switch(shader->uniform_name[U]) {
            case Uniforms::Sampler: glUniform1i(location, 0); break;
            case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(glm::vec4(-0.975f, -0.975f, 1.95f, 1.95f))); break;
            case Uniforms::Time:   glUniform1f(location, total_time); break;
            default: SHADER_WARNING("Uniform mismatch"); break;
        }
    }
    glBindTexture(GL_TEXTURE_2D, quad.texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindVertexArray(0);
    glBindSampler(0, 0);
}

void RenderFinalScreen(const Shader* const shaders[],
                      const Camera* const camera,
                      const UI* const ui,
                      float total_time) {
    const Shader* shader;
    const Quad& quad = ui->quads[(uint32_t)Quads::FINAL_SCREEN];
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ui->quads[(uint32_t)Quads::GAME_SCREEN].framebuffer);
    
    glBindVertexArray(ui->quad_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindSampler(0, camera->sampler_clamp_linear);
    
    shader = shaders[(uint32_t)Shaders::TripleChannelQuad];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
        GLuint location = shader->uniform_location[U];
        switch(shader->uniform_name[U]) {
            case Uniforms::Sampler: glUniform1i(location, 0); break;
            case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(glm::vec4(-0.95f, 0.25f, 0.7f, 0.7f))); break;
            case Uniforms::Time:   glUniform1f(location, total_time); break;
            default: SHADER_WARNING("Uniform mismatch"); break;
        }
    }
    glBindTexture(GL_TEXTURE_2D, quad.texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindVertexArray(0);
    glBindSampler(0, 0);
}

void RenderGameScreen(const Shader* const shaders[],
                      const Camera* const camera,
                      const UI* const ui,
                      float total_time) {
    const Shader* shader;
    const Quad& quad = ui->quads[(uint32_t)Quads::GAME_SCREEN];
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ui->quads[(uint32_t)Quads::FINAL_SCREEN].framebuffer);
    
    glBindVertexArray(ui->quad_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindSampler(0, camera->sampler_clamp_linear);
    
    shader = shaders[(uint32_t)Shaders::TripleChannelQuad];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
        GLuint location = shader->uniform_location[U];
        switch(shader->uniform_name[U]) {
            case Uniforms::Sampler: glUniform1i(location, 0); break;
            case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(glm::vec4(-0.95f, -0.95f, 0.7f, 0.7f))); break;
            case Uniforms::Time:   glUniform1f(location, total_time); break;
            default: SHADER_WARNING("Uniform mismatch"); break;
        }
    }
    glBindTexture(GL_TEXTURE_2D, quad.texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    shader = shaders[(uint32_t)Shaders::SingleChannelQuad];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
       GLuint location = shader->uniform_location[U];
       switch(shader->uniform_name[U]) {
           case Uniforms::Sampler: glUniform1i(location, 0); break;
           case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(glm::vec4(0.25f, 0.25f, 0.7f, 0.7f))); break;
           default: SHADER_WARNING("Uniform mismatch"); break;
       }
    }
    glBindTexture(GL_TEXTURE_2D, quad.depthbuffer);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindVertexArray(0);
    glBindSampler(0, 0);
}

void UIInit(Heap* const heap) {
    UI* ui = (UI*)HeapAllocate(heap, sizeof(UI));
    
    glGenVertexArrays(1, &ui->quad_vao);
    glGenBuffers(1, &ui->vbo);
    glGenBuffers(1, &ui->ibo);
    glBindVertexArray(ui->quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, ui->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, QUAD_VERTEX_ATTRIBUTE_COUNT * sizeof(float), (void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ui->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QuadIndices), QuadIndices, GL_STATIC_DRAW);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    {
        Quad& quad = ui->quads[(uint32_t)Quads::GAME_SCREEN];
        
        glGenTextures(1, &quad.texture);
        glBindTexture(GL_TEXTURE_2D, quad.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glGenTextures(1, &quad.depthbuffer);
        glBindTexture(GL_TEXTURE_2D, quad.depthbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    
        glGenFramebuffers(1, &quad.framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, quad.framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, quad.texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, quad.depthbuffer, 0);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) printf("\nFramebuffer Error\n"); // How is this triggered?
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
    
    {
        Quad& quad = ui->quads[(uint32_t)Quads::FINAL_SCREEN];
        
        glGenTextures(1, &quad.texture);
        glBindTexture(GL_TEXTURE_2D, quad.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glGenTextures(1, &quad.depthbuffer); // remove
        glBindTexture(GL_TEXTURE_2D, quad.depthbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    
        glGenFramebuffers(1, &quad.framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, quad.framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, quad.texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, quad.depthbuffer, 0);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) printf("\nFramebuffer Error\n"); // How is this triggered?
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
    
    
}

