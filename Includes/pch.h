#ifndef PCH_H
#define PCH_H

#include <windows.h>
#include <pthread.h>

#include <immintrin.h>

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

consteval uint32_t CountDigits(uint32_t Value) {
    uint32_t DigitCount = 0u;
    while (Value > 0u) {
        Value /= 10u;
        DigitCount++;
    }
    return DigitCount ? DigitCount : 1u;
}

consteval uint32_t CountAllDigitsUpToValueSquared(uint32_t Value) {
    const uint32_t UpperLimit = Value * Value;
    uint32_t TotalDigits = 0;
    uint32_t DigitsPerNumber = 1;
    uint32_t RangeStart = 1;
    while (RangeStart <= UpperLimit) {
        const uint32_t RangeEnd = (RangeStart * 10 - 1 < UpperLimit)
            ? (RangeStart * 10 - 1)
            : UpperLimit;
        const uint32_t numbers_in_range = RangeEnd - RangeStart + 1;
        TotalDigits += numbers_in_range * DigitsPerNumber;
        RangeStart *= 10;
        DigitsPerNumber++;
    }
    return TotalDigits;
}

#endif