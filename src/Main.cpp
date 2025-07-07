
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
static std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

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
                   const glm::vec2&,
                   const glm::vec2&,
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
        
        // TODO: scale under mouse, like severance
        // TODO: Aspect cells?
        Quad& game_screen_quad = ui->quads[(uint32_t)Quads::GAME_SCREEN];
        const double GX      = game_screen_quad.position.x; // Grid x position in [-1, 1]
        const double GY      = game_screen_quad.position.y; // Grid y position in [-1, 1]
        const double GW      = game_screen_quad.size.x; // Grid width size in (0, 2] range
        const double GH      = game_screen_quad.size.y; // Grid height size in (0, 2] range
        const double HCS     = 0.5f * GW / double(RESOLUTION); // Half cell size
        const double CS      = 2.0f * HCS; // Cell size (Pefrect square grid)
        const double MOUSE_NDC_X   = glm::clamp(input->ndc_x, GX, GX + GW - HCS); // Clamped to respond to the window containing the first render pass
        const double MOUSE_NDC_Y   = glm::clamp(input->ndc_y, GY, GY + GH - HCS); // Clamped to respond to the window containing the first render pass
        const double LocalX  = (MOUSE_NDC_X - GX) / GW;
        const double LocalY  = (MOUSE_NDC_Y - GY) / GH;
        const uint32_t GridX = uint32_t(LocalX * RESOLUTION);
        const uint32_t GridY = uint32_t(LocalY * RESOLUTION);
        const double CellX   = GX + (GridX + 0.5f) * CS - HCS;
        const double CellY   = GY + (GridY + 0.5f) * CS - HCS;
        
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
                        if (RESOLUTION > 5) {
                            RESOLUTION = 1;
                            
                            float x = -0.7f + dist01(gen) * 1.4f; // do better maths
                            float y = -0.7f + dist01(gen) * 1.4f; // do better maths
                            float z =  0.1f + dist01(gen) * 0.2f; // 0.2 to become max based on size above
                            float w = z; // 0.4f + ((dist(gen) + 1.0f) * 1.0f / 2.0f); // TODO aspect?
                            game_screen_quad.position = glm::vec2(x, y);
                            game_screen_quad.size     = glm::vec2(z, w);
                        }
                        
                        GridInit(heaps[(uint32_t)Heaps::Grid], puzzle, RESOLUTION);
                        SetText(puzzle, RESOLUTION); // shuffle as level up or something
                    }
                }
                input->toggles &= ~InputM::Toggle_LC;
            } else input->toggles |= InputM::Toggle_LC;
            
        }
        
        RenderGame(shaders, camera, fonts, &ui->quads[(uint32_t)Quads::SCREEN], ui->quad_vao, input, TotalTime, puzzle, RESOLUTION);
        SaveRender(shaders, camera, ui->quad_vao, &ui->quads[(uint32_t)Quads::SCREEN], &game_screen_quad, TotalTime);
        // Not really SaveRender, do it BlitGameScreen
        
        HighlightCell(shaders, glm::vec2(CellX, CellY), glm::vec2(CS, CS), ui->quad_vao, TotalTime);
        
        RenderTimer(shaders, ui->quad_vao, TotalTime); // Make this a circle and render this
        // over the green area and then on top of that render the game screen
        
        RenderQuadToScreen(shaders, camera, ui->quad_vao, &game_screen_quad, TotalTime);
        
        glfwSwapBuffers(window);
    }
    
    return 1;
}

