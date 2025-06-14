
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
                      const Font* const [],
                      const UI* const,
                      float,
                      TextField&);
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
static std::random_device rd;
static std::mt19937 gen(time(nullptr));

static string GenerateGrid(uint32_t N) {
    uint32_t total = N * N;
    
    std::vector<uint32_t> numbers(total);
    for (uint32_t i = 0; i < total; ++i) {
        numbers[i] = i + 1;
    }
    //std::shuffle(numbers.begin(), numbers.end(), gen);
    
    string grid;
    for (uint32_t i = 0; i < total; ++i) {
        grid += std::to_string(numbers[i]);
        if ((i + 1) % N == 0) {
            grid += '\n';
        } else {
            grid += ' ';
        }
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
    
    TextField level;
    glGenVertexArrays(1, &level.vao);
    glGenBuffers(1, &level.vbo);
    glGenBuffers(1, &level.ibo);
    glBindVertexArray(level.vao);
    glBindBuffer(GL_ARRAY_BUFFER, level.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, TEXT_VERTEX_ATTRIBUTE_COUNT * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, TEXT_VERTEX_ATTRIBUTE_COUNT * sizeof(float), (void*)(3 * sizeof(GLfloat))); 
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, level.ibo);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    string grid = GenerateGrid(camera->resolution);
    // string grid = "Text strings\nto be used\nfor progress"; TODO
    SetText(level, grid.c_str(), camera->resolution);
    
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
        
        if (input->wheel) {
            camera->resolution += input->wheel;
            input->wheel = 0;
            camera->resolution = glm::abs(camera->resolution);
            grid = GenerateGrid(camera->resolution); // TODO BETTER PARSING
            SetText(level, grid.c_str(), camera->resolution);
        }
        
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
                RenderGameScreen(shaders, camera, fonts, ui, TotalTime, level);
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

