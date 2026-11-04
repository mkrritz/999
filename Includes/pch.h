#ifndef PCH_H
#define PCH_H

#include <windows.h>
#include <pthread.h>

#include <immintrin.h>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

//#include <AL/al.h>
//#include <AL/alc.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <fstream>
#include <sys/stat.h>

#include <chrono>
#include <random>
#include <algorithm> // Remove

extern GLuint empty_vao; // For invoking shaders with attributeless vertices.

const uint32_t ZERO = 0;

// https://docs.gl/gl4/glDrawArraysIndirect
typedef struct {
    GLuint Count;
    GLuint InstanceCount;
    GLuint First;
    GLuint BaseInstance;
} DrawArraysIndirectCommand;

// https://docs.gl/gl4/glDrawElementsIndirect
typedef struct {
    GLuint Count;
    GLuint InstanceCount;
    GLuint FirstIndex;
    GLuint BaseVertex;
    GLuint BaseInstance;
} DrawElementsIndirectCommand;

// https://docs.gl/gl4/glDispatchComputeIndirect
typedef struct {
    GLuint X;
    GLuint Y;
    GLuint Z;
} DispatchIndirectCommand;

#define CPU_CAN_WRITE          (GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)
#define CPU_CAN_READ           ( GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)
#define CPU_CAN_READ_AND_WRITE (GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT)

#endif