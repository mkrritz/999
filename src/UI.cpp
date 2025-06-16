
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/UI.h"
#include "game/Text.h"
#include "game/Input.h"
#include "game/Shader.h"
#include "game/Camera.h"
#include "game/Window.h"

#define QUAD_VERTEX_ATTRIBUTE_COUNT 2
#define TOTAL_VERTEX_ATTRIBUTE_COUNT (4 * QUAD_VERTEX_ATTRIBUTE_COUNT)
static uint32_t QuadIndices[6] = {1, 0, 2, 1, 2, 3};
static float QuadVertices[TOTAL_VERTEX_ATTRIBUTE_COUNT] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f
};

void RenderToScreen(const Shader* const shaders[],
                    const Camera* const camera,
                    const UI* const ui,
                    float total_time) {
    const Shader* shader;
    const Quad* quad;
    switch(camera->render_passes) {
        case 1: {
            quad = &ui->quads[(uint32_t)Quads::GAME_SCREEN];
            break;
        }
        case 2: {
            quad = &ui->quads[(uint32_t)Quads::SAVED_GAME_SCREEN];
            break;
        }
    }
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClearColor(0, 0, 1, 0); // Blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBindVertexArray(ui->quad_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindSampler(0, camera->sampler_clamp_linear);
    
    shader = shaders[(uint32_t)Shaders::TripleChannelQuad];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
        GLuint location = shader->uniform_location[U];
        switch(shader->uniform_name[U]) {
            case Uniforms::Sampler: glUniform1i(location, 0); break;
            //case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(glm::vec4(-0.9f, -0.9f, 1.8f, 1.8f))); break;
            case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f))); break;
            case Uniforms::Time:   glUniform1f(location, total_time); break;
            default: SHADER_WARNING("Uniform mismatch"); break;
        }
    }
    glBindTexture(GL_TEXTURE_2D, quad->texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindVertexArray(0);
    glBindSampler(0, 0);
}

void SaveGameScreen(const Shader* const shaders[],
                    const Camera* const camera,
                    const UI* const ui,
                    float total_time) {
    const Shader* shader;
    const Quad* quad = &ui->quads[(uint32_t)Quads::GAME_SCREEN];
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ui->quads[(uint32_t)Quads::SAVED_GAME_SCREEN].framebuffer);
    glClearColor(0, 1, 0, 0); // Green
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBindVertexArray(ui->quad_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindSampler(0, camera->sampler_clamp_linear);
    
    shader = shaders[(uint32_t)Shaders::TripleChannelQuad];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
        GLuint location = shader->uniform_location[U];
        switch(shader->uniform_name[U]) {
            case Uniforms::Sampler: glUniform1i(location, 0); break;
            case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(glm::vec4(-0.65f, 0.1f, 0.7f, 0.7f))); break;
            case Uniforms::Time:   glUniform1f(location, total_time); break;
            default: SHADER_WARNING("Uniform mismatch"); break;
        }
    }
    glBindTexture(GL_TEXTURE_2D, quad->texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindVertexArray(0);
    glBindSampler(0, 0);
}

void RenderGameScreen(const Shader* const shaders[],
                      const Camera* const camera,
                      const Font* const fonts[],
                      const UI* const ui,
                      const InputState* const input,
                      float TotalTime,
                      TextField& level,
                      float resolution) {
    const Shader* shader;
    const Quad* quad = &ui->quads[(uint32_t)Quads::GAME_SCREEN];
    const Font* const font = fonts[(uint32_t)FontType::English];
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, quad->framebuffer);
    
    glBindSampler(0, camera->sampler_clamp_linear);
    
    glClearColor(1, 0, 0, 0); // Red
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    shader = shaders[(uint32_t)Shaders::SingleChannelQuad];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
       GLuint location = shader->uniform_location[U];
       switch(shader->uniform_name[U]) {
           case Uniforms::Resolution: glUniform1f(location, resolution); break;
           case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f))); break;
           default: SHADER_WARNING("Uniform mismatch"); break;
       }
    }
    glBindVertexArray(ui->quad_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    shader = shaders[(uint32_t)Shaders::SDF];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
        GLuint location = shader->uniform_location[U];
        switch(shader->uniform_name[U]) {
            case Uniforms::Time: glUniform1f(location, TotalTime); break;
            case Uniforms::Sampler: glUniform1i(location, 0); break;
            default: SHADER_WARNING("Uniform mismatch"); break;
        }
    }
    glBindTexture(GL_TEXTURE_2D, font->texture);
    glBindVertexArray(level.vao);
    glDrawElements(GL_TRIANGLES, 6 * level.length, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    const float NDC_X = -1.0f + 2.0f * input->x_current / WINDOW_WIDTH;
    const float NDC_Y =  1.0f - 2.0f * input->y_current / WINDOW_HEIGHT;
    const float CellSize = 2.0f / resolution;
    const uint32_t GridX = uint32_t((NDC_X + 1.0f) / CellSize);
    const uint32_t GridY = uint32_t((1.0f - NDC_Y) / CellSize);
    const float HighlightX = -1.0f + (GridX + 0.5f) * CellSize;
    const float HighlightY =  1.0f - (GridY + 0.5f) * CellSize;
    const float HighlightW = 1.0f / resolution;
    
    shader = shaders[(uint32_t)Shaders::Highlight];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
        GLuint location = shader->uniform_location[U];
        switch(shader->uniform_name[U]) {
            case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(glm::vec4(HighlightX - HighlightW,
                                                                                               HighlightY - HighlightW,
                                                                                               2.0f * HighlightW,
                                                                                               2.0f * HighlightW))); break;
            case Uniforms::Time:   glUniform1f(location, TotalTime); break;
            default: SHADER_WARNING("Uniform mismatch"); break;
        }
    }
    glBindVertexArray(ui->quad_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    
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
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) printf("\nFramebuffer Error\n");
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
    
    {
        Quad& quad = ui->quads[(uint32_t)Quads::SAVED_GAME_SCREEN];
        
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
        Quad& quad = ui->quads[(uint32_t)Quads::HIGHLIGHT];
        
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

