// NOT A PROJECT TO BE TAKEN SERIOUSLY

/*
! JANGAN PERNAH PAKAI std::vector
*/

#pragma once

#include <stdio.h>

#if defined(WINDOWS)
constexpr const char* PLATFORM_NAME = "WINDOWS";
#elif defined(ANDROID)
#    define SDL_MAIN_USE_CALLBACKS
#    include <SDL3/SDL.h>
#    include <SDL3/SDL_main.h>
#    include <android/log.h>
// constexpr const char* PLATFORM_NAME = "ANDROID";
#endif

namespace context {
struct ContextGlobal {
    bool is_playing{false};
    uint64_t last_tick = {};
    uint64_t new_tick = {};
    uint16_t window_width{1280};
    uint16_t window_height{720};
};
} // namespace context