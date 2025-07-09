
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/UI.h"
#include "game/Font.h"
#include "game/Input.h"
#include "game/Shader.h"
#include "game/Camera.h"
#include "game/Window.h"

void RenderQuadToScreen(const Shader* const shaders[],
                        const Camera* const camera,
                        const GLuint quad_vao,
                        const Quad* const quad,
                        float total_time) {
    const Shader* shader;
    
    glBindSampler(0, camera->sampler_clamp_linear);
    
    shader = shaders[(uint32_t)Shaders::DrawTexture];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
        GLuint location = shader->uniform_location[U];
        switch(shader->uniform_name[U]) {
            case Uniforms::Sampler:  glUniform1i(location, 0); break;
            case Uniforms::Position: glUniform2fv(location, 1, glm::value_ptr(glm::vec2(SCREEN_WINDOW_X, SCREEN_WINDOW_Y))); break;
            case Uniforms::Size:     glUniform2fv(location, 1, glm::value_ptr(glm::vec2(SCREEN_WINDOW_W, SCREEN_WINDOW_H))); break;
            case Uniforms::Time:     glUniform1f(location, total_time); break;
            default: SHADER_WARNING("Uniform mismatch"); break;
        }
    }
    glBindTexture(GL_TEXTURE_2D, quad->texture);
    glBindVertexArray(quad_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindVertexArray(0);
    glBindSampler(0, 0);
}

void HighlightCell(const Shader* const shaders[],
                   const glm::vec2& position,
                   const glm::vec2& size,
                   const GLuint quad_vao,
                   const float TotalTime) {
    const Shader* shader = shaders[(uint32_t)Shaders::Highlight];
    
    glEnable(GL_BLEND);
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
        GLuint location = shader->uniform_location[U];
        switch(shader->uniform_name[U]) {
            case Uniforms::Position: glUniform2fv(location, 1, glm::value_ptr(position)); break;
            case Uniforms::Size:     glUniform2fv(location, 1, glm::value_ptr(size)); break;
            case Uniforms::Time:     glUniform1f(location, TotalTime); break;
            default: SHADER_WARNING("Uniform mismatch"); break;
        }
    }
    glBindVertexArray(quad_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDisable(GL_BLEND);
    
    glBindVertexArray(0); // Not really required, if all rendering is quads anyway. Grouping them later maybe.
}

void RenderTimer(const Shader* const shaders[],
                 const GLuint quad_vao,
                 float TotalTime) {
    const Shader* shader;
    
    glEnable(GL_BLEND);
    shader = shaders[(uint32_t)Shaders::Timer];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
       GLuint location = shader->uniform_location[U];
       switch(shader->uniform_name[U]) {
           case Uniforms::Time: glUniform1f(location, TotalTime); break;
           case Uniforms::Position: glUniform2fv(location, 1, glm::value_ptr(glm::vec2(-1.0f, -1.0f))); break;
           case Uniforms::Size:     glUniform2fv(location, 1, glm::value_ptr(glm::vec2( 2.0f, 2.0f))); break;
           default: SHADER_WARNING("Uniform mismatch"); break;
       }
    }
    glBindVertexArray(quad_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDisable(GL_BLEND);
    
    glBindVertexArray(0); // Not really required, if all rendering is quads anyway. Grouping them later maybe.
}

void BlitGameScreen(const Shader* const shaders[],
                    const Camera* const camera,
                    const GLuint quad_vao,
                    const Quad* const source,
                    const Quad* const target,
                    float TotalTime) {
    const Shader* shader;
    // Not really a blit, but a quad rendered at some position with some size
    // with an already drawn texture to sample from and cover the entire quad.
    // Which works just fine for the current setup of generating the text for
    // the grid. Maybe bound text generation in a subset of NDC, based on quad size too.
    //glBindSampler(0, camera->sampler_clamp_linear);
    glBindSampler(0, camera->sampler_repeat_linear);
    
    shader = shaders[(uint32_t)Shaders::DrawTexture];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
        GLuint location = shader->uniform_location[U];
        switch(shader->uniform_name[U]) {
            case Uniforms::Sampler:  glUniform1i(location, 0); break;
            case Uniforms::Position: glUniform2fv(location, 1, glm::value_ptr(target->position)); break;
            case Uniforms::Size:     glUniform2fv(location, 1, glm::value_ptr(target->size)); break;
            case Uniforms::Time:     glUniform1f(location, TotalTime); break;
            default: SHADER_WARNING("Uniform mismatch"); break;
        }
    }
    glBindVertexArray(quad_vao);
    glBindTexture(GL_TEXTURE_2D, source->texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindVertexArray(0);
    glBindSampler(0, 0);
}

#define QUAD_VERTEX_ATTRIBUTE_COUNT 2
#define TOTAL_VERTEX_ATTRIBUTE_COUNT (4 * QUAD_VERTEX_ATTRIBUTE_COUNT)
static uint32_t QuadIndices[6] = {1, 0, 2, 1, 2, 3};
static float QuadVertices[TOTAL_VERTEX_ATTRIBUTE_COUNT] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f
};

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
        Quad& quad = ui->quads[(uint32_t)Quads::SCREEN];
        
        quad.position = glm::vec2(SCREEN_WINDOW_X, SCREEN_WINDOW_Y);
        quad.size     = glm::vec2(SCREEN_WINDOW_W, SCREEN_WINDOW_H);
        
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
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) printf("\nFramebuffer Error\n");
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
    
    {
        Quad& quad = ui->quads[(uint32_t)Quads::GAME_SCREEN];
        
        quad.position = glm::vec2(GAME_SCREEN_WINDOW_X, GAME_SCREEN_WINDOW_Y);
        quad.size     = glm::vec2(GAME_SCREEN_WINDOW_W, GAME_SCREEN_WINDOW_H);
        
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
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) printf("\nFramebuffer Error\n");
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
    
}

