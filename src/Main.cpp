
#include "pch.h"

#include "game/Assert.h"
#include "game/Input.h"
#include "game/Shader.h"
#include "game/Globals.h"
#include "game/Camera.h"
#include "game/Memory.h"
#include "game/Text.h"
#include "game/UI.h"
#include "game/Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

GLFWwindow* WindowInit();

// Camera
void CameraInit(Heap* const);
// Camera

// Shader
void ShadersInit(Heap* const,
                 Shader*[], bool);
void ReloadShader(Heap* const,
                  uint32_t);
// Shader

// Input
InputState* NextRecordedInputState();
// Input

// UI
void UIInit(Heap* const);
void RenderGameScreen(const Shader* const [],
                      const Camera* const,
                      const UI* const,
                      float);
void SaveGameScreen(const Shader* const [],
                       const Camera* const,
                       const UI* const,
                       float);
void RenderToScreen(const Shader* const [],
                    const Camera* const,
                    const UI* const,
                    float);
//

// Text
void FontsInit(Heap* const,
               Font* []);
//

using std::string;

string GenerateGrid(int N) {
    int maxVal = N * N;
    string grid;
    
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int num = rand() % (maxVal + 1); // random in [0, N*N]
            grid += std::to_string(num);
        }
        grid += '\n';
    }
    
    return grid;
}


int main() {
    GLFWwindow* window;
    window = WindowInit();
    
    InputState* input = NextRecordedInputState();
    
    Heap* heaps[(uint32_t)Heaps::Count] = {nullptr};
    HeapsInit(heaps);
    
    Shader* shaders[(uint32_t)Shaders::Count] = {nullptr};
    ShadersInit(heaps[(uint32_t)Heaps::Shader], shaders, true);
    
    CameraInit(heaps[(uint32_t)Heaps::Camera]);
    Camera* const camera = (Camera*)heaps[(uint32_t)Heaps::Camera]->memory;
    
    UIInit(heaps[(uint32_t)Heaps::UI]);
    UI* const ui = (UI*)heaps[(uint32_t)Heaps::UI]->memory;
    
    Font* fonts[(uint32_t)FontType::Count] = {nullptr};
    FontsInit(heaps[(uint32_t)Heaps::Font], fonts);
    
    string grid = GenerateGrid(3);
    
    TextField tf[5];
    for(uint32_t i = 0; i < 5; i++) {
        
        glGenVertexArrays(1, &tf[i].vao);
        glGenBuffers(1, &tf[i].vbo);
        glGenBuffers(1, &tf[i].ibo);
        glBindVertexArray(tf[i].vao);
        glBindBuffer(GL_ARRAY_BUFFER, tf[i].vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, TEXT_VERTEX_ATTRIBUTE_COUNT * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, TEXT_VERTEX_ATTRIBUTE_COUNT * sizeof(float), (void*)(3 * sizeof(GLfloat))); 
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tf[i].ibo);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        switch(i) {
            case 0: {
                SetText(tf[i], grid.c_str());
                break;
            }
            case 1: {
                SetText(tf[i], "912\n345\n678");
                break;
            }
            case 2: {
                SetText(tf[i], "912\n345\n678");
                break;
            }
            case 3: {
                SetText(tf[i], "912\n345\n678");
                break;
            }
            case 4: {
                SetText(tf[i], "912\n345\n678");
                break;
            }
        }
        
    }
    
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    //glDepthFunc(GL_LEQUAL);
    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    
    glPointSize(5);
    glLineWidth(2);
    glfwSwapInterval(1); //  fast rendering with 0, dt in fixed, DeltaTime out.. obviously
    
    const float dt = 1.0f/60.0f;
    float CurrectTime, LastTime, DeltaTime;
    float AccumulatorTime, TotalTime, EveryTwoSeconds;
    LastTime = glfwGetTime();
    CurrectTime = DeltaTime = TotalTime = EveryTwoSeconds = 0;
    AccumulatorTime = dt;
    uint32_t UpdateCount= 0;
    uint32_t FixedUpdateCount= 0;
    
    uint32_t text_to_render = 0;
    
    while (!(input->special & InputM::ESC)) {
        CurrectTime = glfwGetTime();
        DeltaTime = CurrectTime - LastTime;
        LastTime = CurrectTime;
        AccumulatorTime += DeltaTime;
        TotalTime += DeltaTime;
        EveryTwoSeconds += DeltaTime;
        
        UpdateCount += 1;
        glfwPollEvents();
        
        input = NextRecordedInputState();
        
        camera->resolution += input->wheel;
        text_to_render = (text_to_render + glm::abs(input->wheel) ) % 5;
        input->wheel = 0;
        
        if (input->special & InputM::F1) {
            if (input->toggles & InputM::Toggle_F1) {
                camera->render_passes = camera->render_passes % 2 + 1;
            }
            input->toggles &= ~InputM::Toggle_F1;
        } else input->toggles |= InputM::Toggle_F1;
        
        while (shader_slots_to_reload) {
            uint32_t slot = __builtin_ctzll(shader_slots_to_reload);
            shader_slots_to_reload &= ~(1ULL << slot);
            ReloadShader(heaps[(uint32_t)Heaps::Shader], slot);
        }
        
        while(AccumulatorTime >= dt) {
            AccumulatorTime -= dt;
            FixedUpdateCount += 1;
            
        }
        
        
        for (uint32_t pass = 0; pass < camera->render_passes; pass++) {
            if (pass == FIRST_PASS) {
                RenderGameScreen(shaders, camera, ui, TotalTime);
                
                // Text
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, fonts[(uint32_t)FontType::English]->texture);
                glBindSampler(0, camera->sampler_clamp_linear);
                
                glUseProgram(shaders[(uint32_t)Shaders::SDF]->ID);
                for (uint32_t U = 0; U < shaders[(uint32_t)Shaders::SDF]->uniforms_count; U++) {
                    GLuint location = shaders[(uint32_t)Shaders::SDF]->uniform_location[U];
                    switch(shaders[(uint32_t)Shaders::SDF]->uniform_name[U]) {
                        case Uniforms::Time: glUniform1f(location, TotalTime); break;
                        case Uniforms::Sampler: glUniform1i(location, 0); break;
                        default: SHADER_WARNING("Uniform mismatch"); break;
                    }
                }
                glBindVertexArray(tf[text_to_render].vao);
                glDrawElements(GL_TRIANGLES, 6 * tf[text_to_render].info.length, GL_UNSIGNED_INT, 0);
                
                glBindVertexArray(0);
                glBindSampler(0, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glDisable(GL_BLEND);
                glEnable(GL_DEPTH_TEST);
                // Text
            }
            
            if (pass == SECOND_PASS) {
                SaveGameScreen(shaders, camera, ui, TotalTime);
            }
            
            RenderToScreen(shaders, camera, ui, TotalTime);
        }
        
        glfwSwapBuffers(window);
    }
    
    return 1;
}

