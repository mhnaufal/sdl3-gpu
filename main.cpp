// #define SDL_MAIN_HANDLED

/* Pikuma
#include "include/main.h"
*/

#include "include/SDL3/include/SDL3/SDL_gpu.h"
#include "include/main_gpu.h"

#include <cstdio>
#include <iostream>

int main(int argc, char const* argv[])
{
    /* Pikuma
    Vec2::Vec2 vertices[3] = {
        {40, 40},
        {80, 40},
        {40, 80},
    };

    ctx.is_playing = create_window();

    while (ctx.is_playing) {
        process_input();

        update_framerate();

        render(vertices);
    }

    destroy_window();
    */

    /*
     * https://wiki.libsdl.org/SDL3/CategoryGPU
     */
    context::ContextGPU ctxgpu{};
    ctxgpu.window_width = 1280;
    ctxgpu.window_height = 720;

    ctxgpu.is_playing = gpu::create_window(ctxgpu);
    auto ok = gpu::init_gpu_device(ctxgpu);

    while (ctxgpu.is_playing && ok) {
        // [1] Process Events
        gpu::process_input(ctxgpu);

        // [2] Update game state

        // [3] Acquire command buffer
        // [4] Acquire swapchain texture: texture that will be rendered
        gpu::init_command_buffer(ctxgpu);

        // [5] Begin render pass
        // where we actually doing the rendering, which encoded into Command Buffer
        auto color_info = SDL_GPUColorTargetInfo{};
        color_info.texture = ctxgpu.gpu_texture;
        color_info.clear_color = SDL_FColor{1.0f, 0.1f, 0.1f, 1.0f};
        color_info.load_op = SDL_GPULoadOp::SDL_GPU_LOADOP_CLEAR;
        color_info.store_op = SDL_GPUStoreOp::SDL_GPU_STOREOP_STORE;
        auto render_pass = SDL_BeginGPURenderPass(ctxgpu.command_buffer, &color_info, 1, nullptr);

        // [6] Draw

        // [7] End render pass
        SDL_EndGPURenderPass(render_pass);

        // [8] Submit command buffer
        SDL_SubmitGPUCommandBuffer(ctxgpu.command_buffer);
    }
    gpu::destroy_window(ctxgpu);

    return 0;
}