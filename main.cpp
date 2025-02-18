// #define SDL_MAIN_HANDLED
#include "include/main.h"

#include <cstdio>
#include <iostream>

int main(int argc, char const* argv[])
{
    Vec2::Vec2 v{10.0f, 10.0f};

    printf(MAG "x: %f | y: %f", v.x, v.y);

    ctx.is_playing = create_window();

    while(ctx.is_playing) {
        SDL_PollEvent(&ctx.event);
        if (ctx.event.type == SDL_EVENT_QUIT || ctx.event.key.key == SDLK_ESCAPE) {
            ctx.is_playing = false;
        }

        update_framerate();

        clear_framebuffer(0xFFBB8800);

        render_framebuffer();
    }

    destroy_window();

    return 0;
}