// #define SDL_MAIN_HANDLED

/* Pikuma
#include "include/main.h"
*/

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

    context::ContextGPU ctxgpu{};
    ctxgpu.window_width = 1280;
    ctxgpu.window_height = 720;

    ctxgpu.is_playing = gpu::create_window(ctxgpu);
    while (ctxgpu.is_playing) { 
        gpu::process_input(ctxgpu);
    }
    gpu::destroy_window(ctxgpu);

    return 0;
}