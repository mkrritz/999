#ifndef SHADER_HPP
#define SHADER_HPP

/* Notes
    
    When adding/removing a shader we update two things.
        1). 'enum struct Shaders' in 'Shader.h'
        2). 'ShaderConfig SHADERS_CONFIG[]' in 'Shader.h'
        
        These two must be in the same order!
        
        That way we can index to shaders through Shaders::Name,
        and Name will really be that one because
        it's in that index at SHADERS_CONFIG
    
    When adding/removing a uniform we update two things.
        1). 'enum struct Uniforms' in 'Shader.h'
        2). 'static Uniforms StringToEnum(const string& name)' in 'Shader.cpp'
        
        What 'enum struct Uniforms' stores, is the names of all
        uniforms used in all shaders, ever.
    
*/

enum struct Shaders {
    TripleChannelQuad,
    SingleChannelQuad,
    SDF,
    Count // Not really a shader.
};

enum struct Uniforms {
    SignNW,
    Time,
    Sampler,
    Position,
    Right,
    Resolution,
    Up,
    PositionAndSize,
    Count // Not really a uniform.
};

struct Shader {
    uint32_t slot;
    GLuint ID;
    #define MAX_UNIFORMS 16
    Uniforms uniform_name[MAX_UNIFORMS];
    GLuint uniform_location[MAX_UNIFORMS];
    uint32_t uniforms_count;
    
    #define MAX_FILE_NAME 32
    char vs_name[MAX_FILE_NAME];
    char gs_name[MAX_FILE_NAME];
    char fs_name[MAX_FILE_NAME];
    char cs_name[MAX_FILE_NAME];
    char tes_name[MAX_FILE_NAME];
    char tcs_name[MAX_FILE_NAME];
    time_t vs_time;
    time_t gs_time;
    time_t fs_time;
    time_t cs_time;
    time_t tes_time;
    time_t tcs_time;
};

#if defined SHADER_SETUP

struct ShaderConfig {
    const char* vs_name;
    const char* gs_name;
    const char* fs_name;
    const char* cs_name;
    const char* tcs_name;
    const char* tes_name;
    const char** uniforms;
    uint32_t uniforms_count;
    const char** feedbacks;
    uint32_t feedbacks_count;
};

/*
    {
        Vertex.vs,
        Geometry.gs,
        Fragment.fs,
        Compute.cs,
        Control.tcs,
        Evaluation.tes,
        Uniforms,
        UniformCount,
        Feedbacks,
        FeedbacksCount
    }
*/
const ShaderConfig SHADERS_CONFIG[] = {
    { "Quad.vs", nullptr, "TripleChannelQuad.fs", nullptr, nullptr, nullptr, (const char*[]){"Sampler", "PositionAndSize", "Time"}, 3, nullptr, 0 },
    { "Quad.vs", nullptr, "SingleChannelQuad.fs", nullptr, nullptr, nullptr, (const char*[]){"Resolution", "PositionAndSize"}, 2, nullptr, 0 },
    { "SDF.vs", nullptr, "SDF.fs", nullptr, nullptr, nullptr, (const char*[]){"Time", "Sampler"}, 2, nullptr, 0 },
};

#endif

#endif
