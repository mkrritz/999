
#include "pch.h"

#include "game/Assert.h"
#include "game/Input.h"
#include "game/Shader.h"
#include "game/Globals.h"
#include "game/Camera.h"
#include "game/Memory.h"
#include "game/Font.h"
#include "game/UI.h"
#include "game/Grid.h"
#include "game/Puzzle.h"
#include "game/Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

GLFWwindow* WindowInit();

//Memory
void HeapsInit(Heap*[]);
void PrintMemory(const Heap* const []);
//Memory

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
Input* NextRecordedInputState();
// Input

// UI
void UIInit(Heap* const);
void RenderGameScreen(const Shader* const [],
                      const Camera* const,
                      const Font* const [],
                      const UI* const,
                      const Input* const,
                      float,
                      const Puzzle* const);
void SaveGameScreen(const Shader* const [],
                       const Camera* const,
                       const UI* const,
                       float);
void RenderToScreen(const Shader* const [],
                    const Camera* const,
                    const UI* const,
                    float);
// UI

// Font
void FontsInit(Heap* const,
               Font* []);
// Font

// Grid
void GridInit(Heap* const,
              Puzzle* const,
              const uint32_t);
void SetText(Puzzle* const);
// Grid

static void PuzzleInit(Heap* const heap) {
    Puzzle* puzzle = (Puzzle*)HeapAllocate(heap, sizeof(Puzzle));
    puzzle->grid = nullptr;
    
    glGenVertexArrays(1, &puzzle->surface.vao);
    glGenBuffers(1, &puzzle->surface.vbo);
    glGenBuffers(1, &puzzle->surface.ibo);
    glBindVertexArray(puzzle->surface.vao);
    glBindBuffer(GL_ARRAY_BUFFER, puzzle->surface.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, TEXT_VERTEX_ATTRIBUTE_COUNT * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, TEXT_VERTEX_ATTRIBUTE_COUNT * sizeof(float), (void*)(3 * sizeof(GLfloat))); 
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, puzzle->surface.ibo);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

int main() {
    GLFWwindow* window;
    window = WindowInit();
    
    Input* input = NextRecordedInputState();
    
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
    
    PuzzleInit(heaps[(uint32_t)Heaps::Main]);
    Puzzle* const puzzle = (Puzzle*)heaps[(uint32_t)Heaps::Main]->memory;
    
    GridInit(heaps[(uint32_t)Heaps::Grid], puzzle, 5);
    SetText(puzzle);
    
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    //glDepthFunc(GL_LEQUAL);
    //glEnable(GL_CULL_FACE);
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
        
        while (shader_slots_to_reload) {
            uint32_t slot = __builtin_ctzll(shader_slots_to_reload);
            shader_slots_to_reload &= ~(1ULL << slot);
            ReloadShader(heaps[(uint32_t)Heaps::Shader], slot);
        }
        
        input = NextRecordedInputState();
        
        if (input->wheel) {
            puzzle->grid->N += int(input->wheel);
            input->wheel = 0;
            
            if (puzzle->grid->N < 1) {  // levels won't be controlled by scroll. it's uint32_t anyway..
                puzzle->grid->N = 1;    // levels won't be controlled by scroll. it's uint32_t anyway..
            }                   // levels won't be controlled by scroll. it's uint32_t anyway..
            if (puzzle->grid->N > 40) { // There is enough memory for 40 levels. So that's here so we don't crash.
                puzzle->grid->N = 1;    // There is enough memory for 40 levels. So that's here so we don't crash.
            }                   // There is enough memory for 40 levels. So that's here so we don't crash.
            
            GridInit(heaps[(uint32_t)Heaps::Grid], puzzle, puzzle->grid->N);
            SetText(puzzle);
            //PrintMemory(heaps);
        }
        
        if (input->special & InputM::F1) {
            if (input->toggles & InputM::Toggle_F1) {
                camera->render_passes = camera->render_passes % 2 + 1;
            }
            input->toggles &= ~InputM::Toggle_F1;
        } else input->toggles |= InputM::Toggle_F1;
        
        if (input->mouse & InputM::LEFT_CLICK) {
            if (input->toggles & InputM::Toggle_LC) {
                
                const uint32_t N = puzzle->grid->N;
                
                const float NDC_X = -1.0f + 2.0f * input->x_current / WINDOW_WIDTH;
                const float NDC_Y =  1.0f - 2.0f * input->y_current / WINDOW_HEIGHT;
                const float CellSize = 2.0f / float(N);
                const uint32_t GridX = uint32_t((NDC_X + 1.0f) / CellSize);
                const uint32_t GridY = N - uint32_t((1.0f - NDC_Y) / CellSize) - 1;
                
                printf("\nMouseNDC=(%.2f, %.2f) Grid(%.3f)[%u, %u] = %u\n", NDC_X, NDC_Y,
                                                                            CellSize,
                                                                            GridX, GridY,
                                                                            puzzle->grid->cells[GridY * N + GridX]);
            }
            input->toggles &= ~InputM::Toggle_LC;
        } else input->toggles |= InputM::Toggle_LC;
        
        while(AccumulatorTime >= dt) {
            AccumulatorTime -= dt;
            FixedUpdateCount += 1;
            
        }
        
        for (uint32_t pass = 0; pass < camera->render_passes; pass++) {
            if (pass == FIRST_PASS) {
                RenderGameScreen(shaders, camera, fonts, ui, input, TotalTime, puzzle);
            }
            
            if (pass == SECOND_PASS) {
                SaveGameScreen(shaders, camera, ui, TotalTime);
            }
            
        }
        RenderToScreen(shaders, camera, ui, TotalTime); // whatever..
        
        
        
        
        glfwSwapBuffers(window);
    }
    
    return 1;
}

