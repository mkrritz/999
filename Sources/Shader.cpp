
#include "pch.h"

#define SHADER_SETUP

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/Shader.h"
#include "game/Globals.h"

using std::string;
using std::filesystem::path;

static const path SHADERS_DIRECTORY = L"Shaders/";

static Uniforms StringToEnum(const string& name) {
    static const std::unordered_map<string, Uniforms> map = {
        {"SignNW", Uniforms::SignNW},
        {"Sampler", Uniforms::Sampler},
        {"Position", Uniforms::Position},
        {"Right", Uniforms::Right},
        {"Size", Uniforms::Size},
    };
    return map.at(name);
}

static time_t GetFileModificationTime(const char* file_path) {
    struct stat file_stat;
    if (stat(file_path, &file_stat) != 0) {
        return 0;
    }
    return file_stat.st_mtime;
}

static void ProcessIncludes(string& source) {
    for (size_t pos = 0; (pos = source.find("#include", pos)) != string::npos;) {
        size_t q1 = source.find('"', pos);
        size_t q2 = source.find('"', q1 + 1);
        string file_path = (SHADERS_DIRECTORY / source.substr(q1 + 1, q2 - q1 - 1)).string();
        std::ifstream file(file_path);
        ASSERT(!file.is_open(), "File not found.");
        string content((std::istreambuf_iterator<char>(file)), {});
        source.replace(pos, q2 + 1 - pos, content);
        pos += content.size();
    }
}

static bool LoadCompileAndAttach(GLuint program_id,
                                 GLuint* const shader_id,
                                 const char* name,
                                 GLenum type,
                                 char* const shader_name,
                                 time_t& shader_time) {
    string ShaderCode;
    path shader_path = SHADERS_DIRECTORY / name;
    std::ifstream ShaderStream(shader_path.string().c_str(), std::ios::in);
    if (ShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << ShaderStream.rdbuf();
        ShaderCode = sstr.str();
        ShaderStream.close();
    } else {
        printf(" Missing file --- '%s'", name);
        return true;
    }
    
    ProcessIncludes(ShaderCode);
    
    *shader_id = glCreateShader(type);
    const char* SourcePointer = ShaderCode.c_str();
    glShaderSource(*shader_id, 1, &SourcePointer, nullptr);
    glCompileShader(*shader_id);

    int ErrorLogLength;
    glGetShaderiv(*shader_id, GL_INFO_LOG_LENGTH, &ErrorLogLength);
    if (ErrorLogLength > 0) {
        std::vector<char> ShaderErrorMessage(ErrorLogLength + 1);
        glGetShaderInfoLog(*shader_id, ErrorLogLength, nullptr, &ShaderErrorMessage[0]);
        printf("\n\n  [ERROR]%s:  %s\n", name, &ShaderErrorMessage[0]);
        glDeleteShader(*shader_id);
        return true;
    } else {
        printf(" %s", name);
        glAttachShader(program_id, *shader_id);
        shader_time = GetFileModificationTime(shader_path.string().c_str());
        strcpy(shader_name, shader_path.string().c_str());
    }
    
    return false;
}

static Shader* LoadShaders(Heap* const heap,
                           const uint32_t slot,
                           const ShaderConfig& config) {
    int ErrorLogLength;
    bool problem = false;
    GLuint vsID, gsID, fsID, csID, tesID, tcsID;
    
    heap->offset = slot * sizeof(Shader);
    Shader* shader = (Shader*)HeapAllocate(heap, sizeof(Shader));
    
    Shader temp; {
        temp.slot = slot;
        temp.ID = glCreateProgram();
        temp.vs_time  = 0;
        temp.tes_time = 0;
        temp.tcs_time = 0;
        temp.gs_time  = 0;
        temp.fs_time  = 0;
        temp.cs_time  = 0;
        temp.vs_name[0]  = '\0';
        temp.tes_name[0] = '\0';
        temp.tcs_name[0] = '\0';
        temp.gs_name[0]  = '\0';
        temp.fs_name[0]  = '\0';
        temp.cs_name[0]  = '\0';
        printf("  [%u] Shader ID %u [", slot, temp.ID);
        
        if (config.vs_name) {
            problem |= LoadCompileAndAttach(temp.ID, &vsID, config.vs_name, GL_VERTEX_SHADER, temp.vs_name, temp.vs_time);
        }
        
        if (config.tes_name) {
            problem |= LoadCompileAndAttach(temp.ID, &tesID, config.tes_name, GL_TESS_EVALUATION_SHADER, temp.tes_name, temp.tes_time);
        }
        
        if (config.tcs_name) {
            problem |= LoadCompileAndAttach(temp.ID, &tcsID, config.tcs_name, GL_TESS_CONTROL_SHADER, temp.tcs_name, temp.tcs_time);
        }
        
        if (config.gs_name) {
            problem |= LoadCompileAndAttach(temp.ID, &gsID, config.gs_name, GL_GEOMETRY_SHADER, temp.gs_name, temp.gs_time);
        }
        
        if (config.fs_name) {
            problem |= LoadCompileAndAttach(temp.ID, &fsID, config.fs_name, GL_FRAGMENT_SHADER, temp.fs_name, temp.fs_time);
        }
        
        if (config.cs_name) {
            problem |= LoadCompileAndAttach(temp.ID, &csID, config.cs_name, GL_COMPUTE_SHADER, temp.cs_name, temp.cs_time);
        }
        
        if (config.feedbacks) {
            if (strcmp(config.feedbacks[config.feedbacks_count], "Seperate") == 0)
                glTransformFeedbackVaryings(temp.ID, config.feedbacks_count, config.feedbacks, GL_SEPARATE_ATTRIBS);
            else if (strcmp(config.feedbacks[config.feedbacks_count], "Interleaved") == 0)
                glTransformFeedbackVaryings(temp.ID, config.feedbacks_count, config.feedbacks, GL_INTERLEAVED_ATTRIBS);
            else
                ASSERT(true, config.feedbacks[config.feedbacks_count]);
        }
        
        glLinkProgram(temp.ID);
        
        if (config.vs_name) {
            glDetachShader(temp.ID, vsID);
            glDeleteShader(vsID);
        }
                
        if (config.tes_name) {
            glDetachShader(temp.ID, tesID);
            glDeleteShader(tesID);
        }
        
        if (config.tcs_name) {
            glDetachShader(temp.ID, tcsID);
            glDeleteShader(tcsID);
        }
        
        if (config.gs_name) {
            glDetachShader(temp.ID, gsID);
            glDeleteShader(gsID);
        }
        
        if (config.fs_name) {
            glDetachShader(temp.ID, fsID);
            glDeleteShader(fsID);
        }
        
        if (config.cs_name) {
            glDetachShader(temp.ID, csID);
            glDeleteShader(csID);
        }
        
        glGetProgramiv(temp.ID, GL_INFO_LOG_LENGTH, &ErrorLogLength);
        if (!(problem || (ErrorLogLength > 0))) {
            for (uint32_t i = 0; i < config.uniforms_count; i++) {
                temp.uniform_name[i] = StringToEnum(config.uniforms[i]);
                temp.uniform_location[i] = glGetUniformLocation(temp.ID, config.uniforms[i]);
            }
            temp.uniforms_count = config.uniforms_count;
            glUseProgram(0);
            glDeleteProgram(shader->ID); // A value of 0 for program will be silently ignored.
            printf(" ] OK (%u, %u)\n", shader->ID, temp.ID);
            *shader = temp; // Overwrite shader
        } else {
            // Just delete the program of the failed attempt at reloading
            // Shader state remains unchanged
            std::vector<char> ProgramErrorMessage(ErrorLogLength + 1);
            glGetProgramInfoLog(temp.ID, ErrorLogLength, nullptr, &ProgramErrorMessage[0]);
            printf(" ] \n%s\n", &ProgramErrorMessage[0]);
            glDeleteProgram(temp.ID);
        }
    }
    
    return shader;
}

static void CheckForReload(Heap* const heap) {
    /*
        When any file is modified,
        the thread will loop over all loaded shader programs
        to find the ones that need reloading.
    */
    const Shader* const shaders = (const Shader*)heap->memory;
    uint64_t shaders_to_be_reloaded = 0;
    for (uint32_t index = 0; index < (uint32_t)Shaders::Count; index++) {
        const Shader* const shader = &shaders[index];
        bool modified = false;
        modified |= (shader->vs_time  < GetFileModificationTime(shader->vs_name));
        modified |= (shader->gs_time  < GetFileModificationTime(shader->gs_name));
        modified |= (shader->fs_time  < GetFileModificationTime(shader->fs_name));
        modified |= (shader->cs_time  < GetFileModificationTime(shader->cs_name));
        modified |= (shader->tcs_time < GetFileModificationTime(shader->tcs_name));
        modified |= (shader->tes_time < GetFileModificationTime(shader->tes_name));
        if (modified) {
            shaders_to_be_reloaded |= (1ULL << shader->slot);
        }
    }
    shader_slots_to_reload = shaders_to_be_reloaded;
}

static void* ObserveDirectoryChanges(void* arg) {
    const wchar_t* path = SHADERS_DIRECTORY.c_str();
    printf("\n--Tracking changes for '%ls' directory on a seperate thread.\n\n", path);
    
    HANDLE directory_handle = CreateFileW(path,
                                          FILE_LIST_DIRECTORY,
                                          FILE_SHARE_READ,
                                          nullptr,
                                          OPEN_EXISTING,
                                          FILE_FLAG_BACKUP_SEMANTICS,
                                          nullptr);
    if (directory_handle == INVALID_HANDLE_VALUE) {
        printf("  How is it possible for the path to not be 'Shaders/'?\n  Why is the path '%ls'?\n", path);
        return nullptr; // FIX THIS SOMETIME
    }
    
    double last_time = 0;
    char buffer[128];
    DWORD bytes_returned;
    Heap* const heap = (Heap*) arg;
    while (1) {
        // This may trigger multiple times for a single file, for various reasons.
        // Adding a timer to allow reloading only once per second.
        if (ReadDirectoryChangesW(directory_handle, buffer, sizeof(buffer), FALSE,
                                  FILE_NOTIFY_CHANGE_LAST_WRITE, &bytes_returned, nullptr, nullptr)) {
            WARNING(bytes_returned >= sizeof(buffer), "Results truncated, events may be missed.");
            FILE_NOTIFY_INFORMATION* info = (FILE_NOTIFY_INFORMATION*)buffer;
            double current_time = glfwGetTime();
            if (info->Action == FILE_ACTION_MODIFIED && (current_time - last_time > 1)) {
                last_time = current_time;
                CheckForReload(heap);
            }
        } else {
            printf("\nReadDirectoryChangesW failed.\n");
            break;
        }
    }
    
    return nullptr;
}

void ShadersInit(Heap* const heap, Shader* shaders[], bool observe_directory_changes) {
    for (uint32_t index = 0; index < (uint32_t)Shaders::Count; index++) {
        shaders[index] = LoadShaders(heap, index, SHADERS_CONFIG[index]);
    }
    
    if (observe_directory_changes) {
        pthread_t thread;
        pthread_create(&thread, nullptr, ObserveDirectoryChanges, (void*)heap);
        pthread_detach(thread);
    }
    
}

void ReloadShader(Heap* const heap, uint32_t index) {
    /* Note:
        The reloaded shader is overwriting the one already loaded,
        so we don't need to do anything with the returned result of LoadShaders.
    */
    printf("\n\n");
    (void)LoadShaders(heap, index, SHADERS_CONFIG[index]);
    printf("\n");
}

