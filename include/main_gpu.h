#pragma once

#include "SDL3/SDL.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_time.h"
#include "SDL3/SDL_timer.h"

#include <cstdint>
#include <cstdlib>
#include <stdio.h>

#define CHECK_SDL_ERROR(CTX, ERROR_MSG, RETURN)                                                    \
    CTX.error = SDL_GetError();                                                                    \
    if (*CTX.error != NULL) {                                                                      \
        fprintf(stderr, ERROR_MSG);                                                                \
        fprintf(stderr, CTX.error);                                                                \
        return RETURN;                                                                             \
    }

/****************************************/
// Global Context
/****************************************/
namespace context {

struct ContextGPU {
    static inline bool is_playing{false};

    static inline SDL_Window* window{};
    static inline SDL_Renderer* renderer{};
    static inline SDL_Texture* frame_buffer_texture{};
    static inline SDL_Event event{};

    static inline uint16_t window_width{1280};
    static inline uint16_t window_height{720};
    static inline const char* error{};
};
} // namespace context

namespace gpu {
auto create_window(context::ContextGPU& ctx) -> bool;
auto destroy_window(context::ContextGPU& ctx) -> void;
auto process_input(context::ContextGPU& ctx) -> void;

/****************************************/
// Window
/****************************************/
auto create_window(context::ContextGPU& ctx) -> bool
{
    if (SDL_Init(SDL_INIT_VIDEO) != true) {
        fprintf(stderr, "Failed to initialize SDL");
        return false;
    }

    ctx.window = SDL_CreateWindow(
        "SDL 3 GPU Vulkan",
        ctx.window_width,
        ctx.window_height,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
    CHECK_SDL_ERROR(ctx, "Failed to create Window", false);

    ctx.renderer = SDL_CreateRenderer(ctx.window, nullptr);
    CHECK_SDL_ERROR(ctx, "Failed to create Renderer", false);

    ctx.frame_buffer_texture = SDL_CreateTexture(
        ctx.renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        ctx.window_width,
        ctx.window_height);
    CHECK_SDL_ERROR(ctx, "Failed to create Frame Buffer Texture", false);

    return true;
}

inline auto destroy_window(context::ContextGPU& ctx) -> void
{
    SDL_DestroyTexture(ctx.frame_buffer_texture);
    SDL_DestroyRenderer(ctx.renderer);
    SDL_DestroyWindow(ctx.window);
    SDL_Quit();
}

inline auto process_input(context::ContextGPU& ctx) -> void
{
    while (SDL_PollEvent(&ctx.event)) {
        switch (ctx.event.type) {
        case SDL_EVENT_QUIT:
            ctx.is_playing = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (ctx.event.key.key == SDLK_ESCAPE) {
                ctx.is_playing = false;
                break;
            }
        }
    }
}
} // namespace gpu