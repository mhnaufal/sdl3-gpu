// NOT A PROJECT TO BE TAKEN SERIOUSLY

#pragma once

#define STB_IMAGE_IMPLEMENTATION

// Thirdparties
#include "FMOD/fmod_errors.h"
#include "SDL3/include/SDL3/SDL_error.h"
#include "stb_image.h"

// STL
#include <fstream>
#include <vector>

/****************************************/
// Helper Macros
/****************************************/
#ifdef ANDROID
#define SDL_CHECK_ERROR(ERROR_MSG, RETURN)                                                         \
    do {                                                                                           \
        const char* error = SDL_GetError();                                                        \
        if (error == nullptr || *error != '\0') {                                                  \
            ANDROID_LOG_ERROR("%s: %s\n", ERROR_MSG, error);                                         \
            SDL_ClearError();                                                                      \
            return RETURN;                                                                         \
        }                                                                                          \
    } while (0);
#elif defined(WINDOWS)
#define SDL_CHECK_ERROR(ERROR_MSG, RETURN)                                                         \
    do {                                                                                           \
        const char* error = SDL_GetError();                                                        \
        if (error == nullptr || *error != '\0') {                                                  \
            fprintf(stderr, "%s: %s\n", ERROR_MSG, error);                                           \
            SDL_ClearError();                                                                      \
            return RETURN;                                                                         \
        }                                                                                          \
    } while (0);
#endif

#define LENGTH(x) (sizeof(x) / sizeof((x)[0]))

#define FMOD_CHECK_ERROR(x)                                                                        \
    if (x != FMOD_OK) {                                                                            \
        fprintf(stderr, FMOD_ErrorString(x));                                                      \
    }

//* Source: https://www.gingerbill.org/article/2015/08/19/defer-in-cpp/
template<typename F>
struct privDefer {
    F f;
    privDefer(F f)
        : f(f)
    {
    }
    ~privDefer() { f(); }
};

template<typename F>
privDefer<F> defer_func(F f)
{
    return privDefer<F>(f);
}

#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x) DEFER_2(x, __COUNTER__)
#define defer(code) auto DEFER_3(_defer_) = defer_func([&]() { code; })

/****************************************/
// Helper Functions
/****************************************/
inline auto read_shader_file(const char* path) -> std::vector<Uint8>
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<Uint8> code(size);

    file.read(reinterpret_cast<char*>(code.data()), size);
    return code;
}
