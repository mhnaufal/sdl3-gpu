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

#define CHECK_SDL_ERROR(CTX, ERROR_MSG, RETURN)                                                    \
    CTX.error = SDL_GetError();                                                                    \
    if (*CTX.error != NULL) {                                                                      \
        fprintf(stderr, ERROR_MSG);                                                                \
        return RETURN;                                                                             \
    }

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

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
auto dot(Vec2& a, Vec2& b) -> float;
auto cross(Vec2& a, Vec2& b) -> float;
auto normalize(Vec2& a) -> void;
} // namespace Vec2

/****************************************/
// Global Context
/****************************************/
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

Context ctxp{};

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr int FPS = 60;
constexpr int MILLISEC_PER_FRAME = (1000 / FPS);

auto create_window(Context ctx) -> bool;
auto destroy_window(Context ctx) -> void;

auto update_framerate(void) -> void;

auto clear_framebuffer(Context ctx, uint32_t color) -> void;
auto render_framebuffer(Context ctx) -> void;

auto draw_pixel(Context ctx, uint8_t x, uint8_t y, uint32_t color);
auto render(Vec2::Vec2* vertices) -> void;
auto fill_triangle(Context ctx, Vec2::Vec2 v0, Vec2::Vec2 v1, Vec2::Vec2 v2, uint32_t color)
    -> void;
auto check_if_point_inside(Vec2::Vec2 p, float x_min, float x_max, float y_min, float y_max)
    -> bool;

auto process_input(Context ctx) -> void;

/****************************************/
// Window
/****************************************/
auto create_window(Context ctx) -> bool
{
    if (SDL_Init(SDL_INIT_VIDEO) != true) {
        fprintf(stderr, "Failed to initialize SDL");
        return false;
    }

    ctx.window = SDL_CreateWindow(
        "PikumaTryRaster", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
    CHECK_SDL_ERROR(ctx, "Failed to create Window", false);

    ctx.renderer = SDL_CreateRenderer(ctx.window, nullptr);
    CHECK_SDL_ERROR(ctx, "Failed to create Renderer", false);

    // ctx.frame_buffer = (uint32_t*)malloc(sizeof(uint32_t) * SCREEN_WIDTH * SCREEN_HEIGHT);
    ctx.frame_buffer = new uint32_t[sizeof(uint32_t) * SCREEN_WIDTH * SCREEN_HEIGHT];

    ctx.frame_buffer_texture = SDL_CreateTexture(
        ctx.renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);
    CHECK_SDL_ERROR(ctx, "Failed to create Frame Buffer Texture", false);

    return true;
}

inline auto draw_pixel(Context ctx, uint8_t x, uint8_t y, uint32_t color)
{
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }
    ctx.frame_buffer[(SCREEN_WIDTH * y) + x] = color;
}

inline auto render_framebuffer(Context ctx) -> void
{
    SDL_UpdateTexture(
        ctx.frame_buffer_texture,
        nullptr,
        ctx.frame_buffer,
        (int)(SCREEN_WIDTH * sizeof(uint32_t))); // (ukuran window * size tiap pixel) karena
                                                 // per-pixel itu color uint32_t, maka pakenya itu
    CHECK_SDL_ERROR(ctx, "Failed to update Frame Buffer Texture", );

    SDL_RenderTexture(ctx.renderer, ctx.frame_buffer_texture, nullptr, nullptr);
    SDL_RenderPresent(ctx.renderer);
}

inline auto clear_framebuffer(Context ctx, uint32_t color) -> void
{
    for (auto i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) { ctx.frame_buffer[i] = color; }
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

inline auto destroy_window(Context ctx) -> void
{
    delete[] ctx.frame_buffer;
    SDL_DestroyTexture(ctx.frame_buffer_texture);
    SDL_DestroyRenderer(ctx.renderer);
    SDL_DestroyWindow(ctx.window);
    SDL_Quit();
}

inline auto process_input(Context ctx) -> void
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

inline auto render(Context ctx, Vec2::Vec2* vertices) -> void
{
    clear_framebuffer(ctx, 0xEEBB8822);

    // TODO: adjust automatically

    fill_triangle(ctx, vertices[0], vertices[1], vertices[2], 0xFF1111FF);

    render_framebuffer(ctx);
}

inline auto
fill_triangle(Context ctx, Vec2::Vec2 v0, Vec2::Vec2 v1, Vec2::Vec2 v2, uint32_t color) -> void
{
    // Find bounding box of all pixels
    auto x_min = MIN(MIN(v0.x, v1.x), v2.x);
    auto y_min = MIN(MIN(v0.y, v1.y), v2.y);
    auto x_max = MAX(MAX(v0.x, v1.x), v2.x);
    auto y_max = MAX(MAX(v0.y, v1.y), v2.y);

    // Loop all candidates pixel
    for (auto y = y_min; y <= y_max; y++) {
        for (auto x = x_min; x <= x_max; x++) {
            Vec2::Vec2 p = {x, y};
            if (check_if_point_inside(p, x_min, x_max, y_min, y_max)) {
                draw_pixel(ctx, (uint8_t)p.x, (uint8_t)p.y, color);
            }
        }
    }
}

inline auto
check_if_point_inside(Vec2::Vec2 p, float x_min, float x_max, float y_min, float y_max) -> bool
{
    if (p.x < x_max && p.x > x_min && p.y < y_max && p.y > y_min) {
        return true;
    }
    return false;
}

/****************************************/
// Vector
/****************************************/

auto Vec2::add(Vec2& a, Vec2& b) -> Vec2
{
    Vec2 result{a.x + b.x, a.y + b.y};
    return result;
}

auto Vec2::sub(Vec2& a, Vec2& b) -> Vec2
{
    Vec2 result{a.x - b.x, a.y - b.y};
    return result;
}

auto Vec2::mul(Vec2& a, float f) -> Vec2
{
    Vec2 result{a.x * f, a.y * f};
    return result;
}

auto Vec2::div(Vec2& a, float f) -> Vec2
{
    Vec2 result{a.x / f, a.y / f};
    return result;
}

auto Vec2::length(Vec2& a) -> float
{
    return sqrtf(a.x * a.x + a.y * a.y);
}

auto Vec2::dot(Vec2& a, Vec2& b) -> float
{
    return (a.x * b.x) + (a.y * b.y);
}

auto Vec2::cross(Vec2& a, Vec2& b) -> float
{
    return a.x * b.y - a.y * b.x;
}

auto Vec2::normalize(Vec2& a) -> void
{
    float length = ::Vec2::length(a);
    a.x /= length;
    a.y /= length;
}
