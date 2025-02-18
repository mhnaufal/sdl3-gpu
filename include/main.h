#pragma once

#include "SDL3/SDL.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_time.h"
#include "SDL3/SDL_timer.h"

#include <cstdint>
#include <cstdlib>
#include <stdio.h>

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"

#define CHECK_SDL_ERROR(ERROR_MSG, RETURN)                                                         \
    ctx.error = SDL_GetError();                                                                    \
    if (*ctx.error != NULL) {                                                                          \
        fprintf(stderr, ERROR_MSG);                                                                \
        return RETURN;                                                                             \
    }

namespace Vec2 {
struct Vec2 {
    float x;
    float y;
};

auto add(Vec2& a, Vec2& b) -> Vec2;
auto sub(Vec2& a, Vec2& b) -> Vec2;
auto mul(Vec2& a, float f) -> Vec2;
auto div(Vec2& a, float f) -> Vec2;
auto length(Vec2& a) -> float;
auto dot(Vec2& a, Vec2& b) -> Vec2;
auto cross(Vec2& a, Vec2& b) -> Vec2;
auto normalize(Vec2& a) -> void;
} // namespace Vec2

constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 128;
constexpr int FPS = 30;
constexpr int MILLISEC_PER_FRAME = (1000 / FPS);

auto create_window(void) -> bool;
auto destroy_window(void) -> void;

auto update_framerate(void) -> void;

auto clear_framebuffer(uint32_t color) -> void;
auto render_framebuffer(void) -> void;

auto draw_pixel(uint8_t x, uint8_t y, uint32_t color);

struct Context {
    static inline bool is_playing{false};

    static inline SDL_Window* window{};
    static inline SDL_Renderer* renderer{};
    static inline uint32_t* frame_buffer{};
    static inline SDL_Texture* frame_buffer_texture{};
    static inline SDL_Event event{};

    static inline uint16_t window_width{800};
    static inline uint16_t window_height{800};
    static inline const char* error{};
};

Context ctx{};

auto create_window(void) -> bool
{
    if (SDL_Init(SDL_INIT_VIDEO) != true) {
        fprintf(stderr, "Failed to initialize SDL");
        return false;
    }

    ctx.window =
        SDL_CreateWindow("PikumaTryRaster", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
    CHECK_SDL_ERROR("Failed to create Window", false);

    ctx.renderer = SDL_CreateRenderer(ctx.window, nullptr);
    CHECK_SDL_ERROR("Failed to create Renderer", false);

    // ctx.frame_buffer = (uint32_t*)malloc(sizeof(uint32_t) * SCREEN_WIDTH * SCREEN_HEIGHT);
    ctx.frame_buffer = new uint32_t[sizeof(uint32_t) * SCREEN_WIDTH * SCREEN_HEIGHT];

    ctx.frame_buffer_texture = SDL_CreateTexture(
        ctx.renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);
    CHECK_SDL_ERROR("Failed to create Frame Buffer Texture", false);

    return true;
}

inline auto draw_pixel(uint8_t x, uint8_t y, uint32_t color)
{
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }
    ctx.frame_buffer[(SCREEN_WIDTH * y) + x] = color;
}

inline auto render_framebuffer(void) -> void
{
    SDL_UpdateTexture(
        ctx.frame_buffer_texture,
        nullptr,
        ctx.frame_buffer,
        (int)(SCREEN_WIDTH * sizeof(uint32_t))); // (ukuran window * size tiap pixel) karena
                                                 // per-pixel itu color uint32_t, maka pakenya itu
    CHECK_SDL_ERROR("Failed to update Frame Buffer Texture", );

    SDL_RenderTexture(ctx.renderer, ctx.frame_buffer_texture, nullptr, nullptr);
    SDL_RenderPresent(ctx.renderer);
}

inline auto clear_framebuffer(uint32_t color) -> void
{
    for (auto i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) { 
        ctx.frame_buffer[i] = color; 
    }
}

inline auto update_framerate(void) -> void
{
    static uint64_t previous_frame_time = 0;
    uint64_t time_elapsed = SDL_GetTicks() - previous_frame_time;
    uint64_t time_to_wait = MILLISEC_PER_FRAME - time_elapsed;
    if (time_to_wait > 0 && time_to_wait < MILLISEC_PER_FRAME) {
        SDL_Delay((Uint32)time_to_wait);
    }
    previous_frame_time = SDL_GetTicks();
}

inline auto destroy_window(void) -> void
{
    delete[] ctx.frame_buffer;
    SDL_DestroyTexture(ctx.frame_buffer_texture);
    SDL_DestroyRenderer(ctx.renderer);
    SDL_DestroyWindow(ctx.window);
    SDL_Quit();
}
