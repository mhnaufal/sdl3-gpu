#pragma once

#include "SDL3/SDL.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_gpu.h"
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
    bool is_playing{false};

    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* frame_buffer_texture{};
    SDL_Event event{};
    SDL_GPUDevice* gpu_device{};
    SDL_GPUCommandBuffer* command_buffer{};
    SDL_GPUTexture* gpu_texture{};

    uint16_t window_width{1280};
    uint16_t window_height{720};
    const char* error{};
};
} // namespace context

namespace gpu {
auto create_window(context::ContextGPU& ctx) -> bool;
auto destroy_window(context::ContextGPU& ctx) -> void;
auto process_input(context::ContextGPU& ctx) -> void;
auto init_gpu_device(context::ContextGPU& ctx) -> bool;
auto init_command_buffer(context::ContextGPU& ctx) -> bool;

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

inline auto init_gpu_device(context::ContextGPU& ctx) -> bool
{
    ctx.gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    auto ok = SDL_ClaimWindowForGPUDevice(ctx.gpu_device, ctx.window);

    return ok;
}

inline auto init_command_buffer(context::ContextGPU& ctx) -> bool
{
    ctx.command_buffer = SDL_AcquireGPUCommandBuffer(ctx.gpu_device);
    auto ok = SDL_WaitAndAcquireGPUSwapchainTexture(
        ctx.command_buffer, ctx.window, &ctx.gpu_texture, nullptr, nullptr);

    return ok;
}

} // namespace gpu