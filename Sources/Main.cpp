
#include "pch.h"

#include "game/Assert.h"
#include "game/Input.h"
#include "game/Shader.h"
#include "game/Globals.h"
#include "game/Camera.h"
#include "game/Random.h"
#include "game/Memory.h"
#include "game/Font.h"
#include "game/UI.h"
#include "game/Puzzle.h"
#include "game/Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

std::mt19937 gen(time(nullptr));
std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

GLuint empty_vao;

GLFWwindow* WindowInit();

#if 1

//Memory
void HeapsInit(Heap*[]);
void PrintMemory(const Heap* const*);
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
                   const UI* const,
                   GLuint);
void RenderGame(const Shader* const [],
                const Camera* const,
                const Font* const [],
                const UI* const,
                const Puzzle* const,
                GLuint);
void BlitGameScreen(const Shader* const [],
                    const Camera* const,
                    const UI* const,
                    GLuint,
                    GLuint);
void RenderTimer(const Shader* const [],
                 const UI* const,
                 GLuint);

void RenderQuadToScreen(const Shader* const*,
                        const UI* const,
                        const GLuint,
                        const GLuint);
// UI

// Font
void FontsInit(Heap* const,
               Font* []);
// Font

// Grid
void SetText(Puzzle* const,
             const uint32_t);
// Grid

#endif

struct SceneUBO {          // WHere does thsi go?
    glm::vec4 timers;      // WHere does thsi go?
};                         // WHere does thsi go?

/*  How rendering is done:
        There are two render targets.
        Both of them have the same size. (WINDOW_WIDTH, WINDOW_HEIGHT)
        Some things are rendered to the FULLSCREEN target.
        Then some things are rendered on the GRID target.
            One of the things that is rendered on the GRID target, is the contents
            of the FULLSCREEN rendered target.
        The final screen rendering comes from the GRID target. A full screen quad
        is rendered with the contents sampled from the GRID target.
    
    TODO: DSA text.
    TODO: Aspect cells?
    TODO: scale under mouse, radially, like severance. No cursor?
    TODO: Remove news, pass heaps.
    
    Levels:
        PulseEffect, WaveEffect
        Changing target position every frame. (Moving grid around)
        Skip various numbers at level generation.
        Do fonts other than numbers. Emojis and such.
    Score:
        Add cell value or something.
    Time:
        Add cell value to timer or something.
    Powerups:
        999:
    
*/

int main() {
    GLFWwindow* window;
    window = WindowInit();
    
    glCreateVertexArrays(1, &empty_vao);
    
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
    
    Puzzle* const puzzle = (Puzzle*)HeapAllocate(heaps[(uint32_t)Heaps::Main], sizeof(Puzzle));
    glCreateBuffers(1, &puzzle->vbo); // Not really a vbo. Rename..
    
    uint32_t RESOLUTION = 1;
    HeapReset(heaps[(uint32_t)Heaps::Grid]);
    puzzle->cells = (uint32_t*)HeapAllocate(heaps[(uint32_t)Heaps::Grid], RESOLUTION * RESOLUTION * sizeof(uint32_t));
    SetText(puzzle, RESOLUTION);
    
    GLuint scene_ubo_handle;
    glCreateBuffers(1, &scene_ubo_handle);
    glNamedBufferStorage(scene_ubo_handle,
                         sizeof(SceneUBO),
                         nullptr,
                         CPU_CAN_WRITE);
    SceneUBO* scene_ubo = (SceneUBO*)glMapNamedBufferRange(scene_ubo_handle,
                                                           0,
                                                           sizeof(SceneUBO),
                                                           CPU_CAN_WRITE);
    scene_ubo->timers.y = RESOLUTION; // Another
    
    glDisable(GL_DEPTH_TEST);
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
    
    glBindVertexArray(empty_vao); // Only quad rendering going on. No VAO needed.
    
    while (!(input->special & InputM::ESC)) {
        CurrectTime = glfwGetTime();
        DeltaTime = CurrectTime - LastTime;
        LastTime = CurrectTime;
        AccumulatorTime += DeltaTime;
        TotalTime += DeltaTime;
        EveryTwoSeconds += DeltaTime;
        
        scene_ubo->timers.x = TotalTime;
        
        glfwPollEvents();
        
        input = NextRecordedInputState();
        
        while (shader_slots_to_reload) {
            uint32_t slot = __builtin_ctzll(shader_slots_to_reload);
            shader_slots_to_reload &= ~(1ULL << slot);
            ReloadShader(heaps[(uint32_t)Heaps::Shader], slot);
        }
        
        while(AccumulatorTime >= dt) {
            AccumulatorTime -= dt;
            
            glm::vec4 GXGYGWGH = ui->ubo[(uint32_t)Quads::Grid]->rect;
            const double GX          = GXGYGWGH.x; // Grid x position in [-1, 1]
            const double GY          = GXGYGWGH.y; // Grid y position in [-1, 1]
            const double GW          = GXGYGWGH.z; // Grid width size in (0, 2] range
            const double GH          = GXGYGWGH.w; // Grid height size in (0, 2] range
            const double HCS         = 0.5f * GW / double(RESOLUTION); // Half cell size
            const double CS          = 2.0f * HCS; // Cell size (Pefrect square grid)
            const double MOUSE_NDC_X = glm::clamp(input->ndc_x, GX, GX + GW - HCS);
            const double MOUSE_NDC_Y = glm::clamp(input->ndc_y, GY, GY + GH - HCS);
            const double LocalX      = (MOUSE_NDC_X - GX) / GW;
            const double LocalY      = (MOUSE_NDC_Y - GY) / GH;
            const uint32_t GridX     = uint32_t(LocalX * RESOLUTION);
            const uint32_t GridY     = uint32_t(LocalY * RESOLUTION);
            const double CellX       = GX + (GridX + 0.5f) * CS - HCS;
            const double CellY       = GY + (GridY + 0.5f) * CS - HCS;
            ui->ubo[(uint32_t)Quads::Highlight]->rect = glm::vec4(CellX, CellY, CS, CS);
            
            if (input->mouse & InputM::LEFT_CLICK) {
                if (input->toggles & InputM::Toggle_LC) {
                    
                    if (puzzle->cells[GridY * RESOLUTION + GridX] == 1) { // R*R and other modes
                        RESOLUTION++;
                        if (RESOLUTION > MAX_LEVEL) {
                            RESOLUTION = 1;
                            
                            float x = -0.7f + dist(gen) * 0.3f; // do better maths
                            float y = -0.7f + dist(gen) * 0.3f; // do better maths
                            float z =  1.2f + dist(gen) * 0.2f; // 0.2 to become max based on size above
                            float w = z; // 0.4f + ((dist(gen) + 1.0f) * 1.0f / 2.0f); // TODO aspect?
                            
                            ui->ubo[(uint32_t)Quads::Grid]->rect = glm::vec4(x, y, z, w);
                        }
                        scene_ubo->timers.y = RESOLUTION;
                        
                        HeapReset(heaps[(uint32_t)Heaps::Grid]);
                        puzzle->cells = (uint32_t*)HeapAllocate(heaps[(uint32_t)Heaps::Grid], RESOLUTION * RESOLUTION * sizeof(uint32_t));
                        SetText(puzzle, RESOLUTION);
                        
                        TotalTime -= 1.0f;
                    }
                    
                }
                input->toggles &= ~InputM::Toggle_LC;
            } else input->toggles |= InputM::Toggle_LC;
            
            
            if (input->letters & InputM::H) {
                if (input->toggles & InputM::H) {
                    PrintMemory(heaps);
                }
                input->toggles &= ~InputM::H;
            } else input->toggles |= InputM::H;
        }
        
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ui->quads[(uint32_t)RenderTarget::FULLSCREEN].framebuffer);
        glClearColor(1, 0, 0, 0); // Red
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        RenderGame(shaders, camera, fonts, ui, puzzle, scene_ubo_handle);
        
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ui->quads[(uint32_t)RenderTarget::GRID].framebuffer);
        glClearColor(0, 1, 0, 0); // Green
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glEnable(GL_BLEND);
        
        glUseProgram(shaders[(uint32_t)Shaders::Timer]->ID);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene_ubo_handle);
        glBindBufferBase(GL_UNIFORM_BUFFER, 2, ui->ubo_handle[(uint32_t)Quads::Fullscreen]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        glBindSampler(0, camera->sampler_repeat_linear);
        glBindTextureUnit(0, ui->quads[(uint32_t)RenderTarget::FULLSCREEN].texture);
        
        glUseProgram(shaders[(uint32_t)Shaders::DrawTexture]->ID);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene_ubo_handle);
        glBindBufferBase(GL_UNIFORM_BUFFER, 2, ui->ubo_handle[(uint32_t)Quads::Grid]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        glBindSampler(0, 0);
        glBindTextureUnit(0, 0);
        
        glUseProgram(shaders[(uint32_t)Shaders::Highlight]->ID);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene_ubo_handle);
        glBindBufferBase(GL_UNIFORM_BUFFER, 2, ui->ubo_handle[(uint32_t)Quads::Highlight]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        glDisable(GL_BLEND);
        
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glClearColor(0, 0, 1, 0); // Blue
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        RenderQuadToScreen(shaders, ui, camera->sampler_clamp_linear, ui->quads[(uint32_t)RenderTarget::GRID].texture);
        
        glfwSwapBuffers(window);
    }
    
    glBindVertexArray(0);
    
    return 1;
}

