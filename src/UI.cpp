
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/UI.h"
#include "game/Font.h"
#include "game/Input.h"
#include "game/Puzzle.h"
#include "game/Grid.h"
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
                    const UI* const ui, // Just a texture argument here will do
                    float total_time) {
    const Shader* shader;
    const Quad* quad;
    switch(camera->render_passes) {
        case 1: {
            quad = &ui->quads[(uint32_t)Quads::SCREEN];
            break;
        }
        case 2: {
            quad = &ui->quads[(uint32_t)Quads::GAME_SCREEN];
            break;
        }
    }
    
    glBindSampler(0, camera->sampler_clamp_linear);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClearColor(0, 0, 1, 0); // Blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    shader = shaders[(uint32_t)Shaders::DrawTexture];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
        GLuint location = shader->uniform_location[U];
        switch(shader->uniform_name[U]) {
            case Uniforms::Sampler: glUniform1i(location, 0); break;
            case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f))); break;
            case Uniforms::Time:   glUniform1f(location, total_time); break;
            default: SHADER_WARNING("Uniform mismatch"); break;
        }
    }
    glBindTexture(GL_TEXTURE_2D, quad->texture);
    glBindVertexArray(ui->quad_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glBindVertexArray(0);
    glBindSampler(0, 0);
}

void HighlightCell(const Shader* const shaders[],
                   const uint32_t resolution,
                   const Quad* const quad,
                   const Input* const input,
                   const GLuint quad_vao,
                   const float TotalTime) {
    const Shader* shader = shaders[(uint32_t)Shaders::Highlight];
    
    const double WindowX      = quad->bounds.x; // do aspect maths too?
    const double WindowY      = quad->bounds.y; // do aspect maths too?
    const double WindowWidth  = quad->bounds.z; // do aspect maths too?
    const double WindowHeight = quad->bounds.w; // do aspect maths too?
    const double HalfCellSize = WindowWidth / (2.0f * double(resolution));
    const double CellSize     = 2.0f * HalfCellSize;
    
    const double NDC_X      = glm::clamp(input->ndc_x, WindowX, WindowX + WindowWidth  - HalfCellSize);
    const double NDC_Y      = glm::clamp(input->ndc_y, WindowY, WindowY + WindowHeight - HalfCellSize);
    const double CellX      = (NDC_X - WindowX) / WindowWidth;
    const double CellY      = (NDC_Y - WindowY) / WindowHeight;
    const uint32_t GridX    = uint32_t(CellX * resolution);
    const uint32_t GridY    = uint32_t(CellY * resolution);
    const double HighlightX = WindowX + (GridX + 0.5f) * CellSize;
    const double HighlightY = WindowY + (GridY + 0.5f) * CellSize;
    
    glEnable(GL_BLEND);
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
        GLuint location = shader->uniform_location[U];
        switch(shader->uniform_name[U]) {
            case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(glm::vec4(HighlightX - HalfCellSize,
                                                                                               HighlightY - HalfCellSize,
                                                                                               CellSize,
                                                                                               CellSize))); break;
            case Uniforms::Time:   glUniform1f(location, TotalTime); break;
            default: SHADER_WARNING("Uniform mismatch"); break;
        }
    }
    glBindVertexArray(quad_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDisable(GL_BLEND);
}

void SaveRender(const Shader* const shaders[],
                const Camera* const camera,
                const Font* const fonts[],
                const UI* const ui,
                const Input* const input,
                float TotalTime) {
    const Shader* shader;
    const Quad* source_quad = &ui->quads[(uint32_t)Quads::SCREEN];      // Do this with parameters so it can be used
    const Quad* target_quad = &ui->quads[(uint32_t)Quads::GAME_SCREEN]; // Do this with parameters so it can be used
    
    glBindSampler(0, camera->sampler_clamp_linear);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target_quad->framebuffer);
    glClearColor(0, 1, 0, 0); // Green
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    shader = shaders[(uint32_t)Shaders::DrawTexture];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
        GLuint location = shader->uniform_location[U];
        switch(shader->uniform_name[U]) {
            case Uniforms::Sampler: glUniform1i(location, 0); break;
            case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(target_quad->bounds)); break;
            case Uniforms::Time:   glUniform1f(location, TotalTime); break;
            default: SHADER_WARNING("Uniform mismatch"); break;
        }
    }
    glBindVertexArray(ui->quad_vao);
    glBindTexture(GL_TEXTURE_2D, source_quad->texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glEnable(GL_BLEND);
    shader = shaders[(uint32_t)Shaders::Timer];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
       GLuint location = shader->uniform_location[U];
       switch(shader->uniform_name[U]) {
           case Uniforms::Time: glUniform1f(location, TotalTime); break;
           case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(glm::vec4(-0.9f, 0.85f, 1.8f, 0.1f))); break;
           default: SHADER_WARNING("Uniform mismatch"); break;
       }
    }
    glBindVertexArray(ui->quad_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glDisable(GL_BLEND);
    
    glBindVertexArray(0);
    glBindSampler(0, 0);
}

void RenderGame(const Shader* const shaders[],
                const Camera* const camera,
                const Font* const fonts[],
                const UI* const ui,
                const Input* const input,
                float TotalTime,
                const Puzzle* const puzzle) {
    const Shader* shader;
    const Quad* quad = &ui->quads[(uint32_t)Quads::SCREEN];
    const Font* const font = fonts[(uint32_t)FontType::English];
    
    glBindSampler(0, camera->sampler_clamp_linear);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, quad->framebuffer);
    glClearColor(1, 0, 0, 0); // Red
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    shader = shaders[(uint32_t)Shaders::Grid];
    glUseProgram(shader->ID);
    for (uint32_t U = 0; U < shader->uniforms_count; U++) {
       GLuint location = shader->uniform_location[U];
       switch(shader->uniform_name[U]) {
           case Uniforms::Resolution: glUniform1f(location, puzzle->grid->N); break;
           case Uniforms::PositionAndSize: glUniform4fv(location, 1, glm::value_ptr(quad->bounds)); break;
           default: SHADER_WARNING("Uniform mismatch"); break;
       }
    }
    glBindVertexArray(ui->quad_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
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
    glBindVertexArray(puzzle->surface.vao);
    glDrawElements(GL_TRIANGLES, 6 * puzzle->surface.quads_count, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    
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
        Quad& quad = ui->quads[(uint32_t)Quads::SCREEN];
        
        quad.bounds = glm::vec4(-1.0f, -1.0f, 2.0f, 2.0f);
        
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
        
        quad.bounds = glm::vec4(-0.8f, -0.8f, 1.6f, 1.6f);
        
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
        
        // no bounds here at all
        
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

