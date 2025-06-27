
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

static std::mt19937 gen(time(nullptr));
static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

GLFWwindow* WindowInit();

//Memory
void HeapsInit(Heap*[]);
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
void HighlightCell(const Shader* const [],
                   const glm::vec4&,
                   const GLuint,
                   const float);
void RenderGame(const Shader* const [],
                const Camera* const,
                const Font* const [],
                const Quad* const,
                const GLuint,
                const Input* const,
                float,
                const Puzzle* const puzzle,
                float);
void SaveRender(const Shader* const [],
                const Camera* const,
                const GLuint,
                const Quad* const,
                const Quad* const,
                float);
void RenderTimer(const Shader* const [],
                 const GLuint,
                 float);
void RenderQuadToScreen(const Shader* const [],
                        const Camera* const,
                        const GLuint,
                        const Quad* const,
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
void SetText(Puzzle* const,
             const uint32_t);
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
    
    uint32_t RESOLUTION = 1;
    GridInit(heaps[(uint32_t)Heaps::Grid], puzzle, RESOLUTION);
    SetText(puzzle, RESOLUTION);
    
    glDisable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    //glDepthFunc(GL_LEQUAL);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
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
        
        Quad& quad = ui->quads[(uint32_t)Quads::SCREEN_2]; // GRID_QUAD
        const double GX     = quad.position_and_size.x;
        const double GY     = quad.position_and_size.y;
        const double GW     = quad.position_and_size.z;
        const double GH     = quad.position_and_size.w;
        const double HCS    = 0.5f * GW / double(RESOLUTION);
        const double CS     = 2.0f * HCS;
        const double NDC_X  = glm::clamp(input->ndc_x, GX, GX + GW - HCS);
        const double NDC_Y  = glm::clamp(input->ndc_y, GY, GY + GH - HCS);
        const double LocalX = (NDC_X - GX) / GW;
        const double LocalY = (NDC_Y - GY) / GH;
        const uint32_t GridX = uint32_t(LocalX * RESOLUTION);
        const uint32_t GridY = uint32_t(LocalY * RESOLUTION);
        
        const double CellX = GX + (GridX + 0.5f) * CS - HCS;
        const double CellY = GY + (GridY + 0.5f) * CS - HCS;
        // Where do all these go?
        
        while(AccumulatorTime >= dt) {
            AccumulatorTime -= dt;
            FixedUpdateCount += 1;
            
            if (input->wheel) {
                input->wheel = 0;
            }
            
            if (input->mouse & InputM::LEFT_CLICK) {
                if (input->toggles & InputM::Toggle_LC) {
                    
                    if (puzzle->grid->cells[GridY * RESOLUTION + GridX] == 1) {
                        RESOLUTION++;
                        if (RESOLUTION > 10) {
                            RESOLUTION = 1;
                            
                            float x = -0.99f + ((dist(gen) + 1.0f) * 0.49f / 2.0f);    // do better maths, so it can be small and top right
                            float y = -0.99f + ((dist(gen) + 1.0f) * 0.49f / 2.0f);    // do better maths, so it can be small and top right
                            float z = 0.1f + ((dist(gen) + 1.0f) * 1.0f / 2.0f) * 1.4f;
                            float w = z; // 0.4f + ((dist(gen) + 1.0f) * 1.0f / 2.0f); // TODO aspect
                            quad.position_and_size = glm::vec4(x, y, z, w);
                        }
                        
                        GridInit(heaps[(uint32_t)Heaps::Grid], puzzle, RESOLUTION);
                        SetText(puzzle, RESOLUTION); // shuffle as level up or something
                    }
                }
                input->toggles &= ~InputM::Toggle_LC;
            } else input->toggles |= InputM::Toggle_LC;
            
        }
        
        RenderGame(shaders, camera, fonts, &ui->quads[(uint32_t)Quads::SCREEN_1], ui->quad_vao, input, TotalTime, puzzle, RESOLUTION);
        SaveRender(shaders, camera, ui->quad_vao, &ui->quads[(uint32_t)Quads::SCREEN_1], &ui->quads[(uint32_t)Quads::SCREEN_2], TotalTime);
        
        const glm::vec4 position_and_size(CellX, CellY, CS, CS); // do aspect?
        HighlightCell(shaders, position_and_size, ui->quad_vao, TotalTime);
        
        RenderTimer(shaders, ui->quad_vao, TotalTime);
        
        RenderQuadToScreen(shaders, camera, ui->quad_vao, &ui->quads[(uint32_t)Quads::SCREEN_2], TotalTime);
        
        glfwSwapBuffers(window);
    }
    
    return 1;
}

