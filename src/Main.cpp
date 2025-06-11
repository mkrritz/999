
#include "pch.h"

#include "game/Assert.h"
#include "game/Input.h"
#include "game/Shader.h"
#include "game/Globals.h"
#include "game/Camera.h"
#include "game/Memory.h"
#include "game/UI.h"
#include "game/Window.h"

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
    
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    //glDepthFunc(GL_LEQUAL);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendEquation(GL_FUNC_ADD);
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

