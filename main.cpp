// #define SDL_MAIN_HANDLED

/* Pikuma
#include "include/main.h"
*/

#include "include/SDL3/include/SDL3/SDL_gpu.h"
#include "include/main_gpu.h"

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

    auto vert_code = gpu::read_shader_file("../../../../shaders/shader.spv.vert");
    auto shader_create_info_vert = SDL_GPUShaderCreateInfo{};
    shader_create_info_vert.code_size = vert_code.size();
    shader_create_info_vert.code = vert_code.data();
    shader_create_info_vert.entrypoint= "main";
    shader_create_info_vert.format = SDL_GPU_SHADERFORMAT_SPIRV;
    shader_create_info_vert.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    auto vert_shader = SDL_CreateGPUShader(ctxgpu.gpu_device, &shader_create_info_vert);

    auto frag_code = gpu::read_shader_file("../../../../shaders/shader.spv.frag");
    auto shader_create_info_frag = SDL_GPUShaderCreateInfo{};
    shader_create_info_frag.code_size = frag_code.size();
    shader_create_info_frag.code = frag_code.data();
    shader_create_info_frag.entrypoint= "main";
    shader_create_info_frag.format = SDL_GPU_SHADERFORMAT_SPIRV;
    shader_create_info_frag.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    auto frag_shader = SDL_CreateGPUShader(ctxgpu.gpu_device, &shader_create_info_frag);

    auto color_target_desc = SDL_GPUColorTargetDescription{};
    color_target_desc.format = SDL_GetGPUSwapchainTextureFormat(ctxgpu.gpu_device, ctxgpu.window);

    auto pipeline_target_info = SDL_GPUGraphicsPipelineTargetInfo{};
    pipeline_target_info.num_color_targets = 1;
    pipeline_target_info.color_target_descriptions = &color_target_desc;

    auto pipeline_create_info = SDL_GPUGraphicsPipelineCreateInfo{};
    pipeline_create_info.vertex_shader = vert_shader;
    pipeline_create_info.fragment_shader = frag_shader;
    pipeline_create_info.primitive_type = SDL_GPUPrimitiveType::SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipeline_create_info.target_info = pipeline_target_info;

    auto pipeline = SDL_CreateGPUGraphicsPipeline(ctxgpu.gpu_device, &pipeline_create_info);

    SDL_ReleaseGPUShader(ctxgpu.gpu_device, vert_shader);
    SDL_ReleaseGPUShader(ctxgpu.gpu_device, frag_shader);

    while (ctxgpu.is_playing && ok) {
        // [1] Process Events
        gpu::process_input(ctxgpu);

        // [2] Update game state

        // [3] Acquire command buffer
        // [4] Acquire swapchain texture: texture that will be rendered
        gpu::init_command_buffer(ctxgpu);

        if (ctxgpu.gpu_texture) {
            // [5] Begin render pass
            // where we actually doing the rendering, which encoded into Command Buffer
            auto color_info = SDL_GPUColorTargetInfo{};
            color_info.texture = ctxgpu.gpu_texture;
            color_info.clear_color = SDL_FColor{0.0f, 0.9f, 0.4f, 1.0f};
            color_info.load_op = SDL_GPULoadOp::SDL_GPU_LOADOP_CLEAR;
            color_info.store_op = SDL_GPUStoreOp::SDL_GPU_STOREOP_STORE;
            auto render_pass = SDL_BeginGPURenderPass(ctxgpu.command_buffer, &color_info, 1, nullptr);

            // [6] Draw
                // - Bind pipeline
                SDL_BindGPUGraphicsPipeline(render_pass, pipeline);
                // - Bind vertex buffer/data
                // - Bind unform buffer/data
                // - draw calls
                SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);
                

            // [7] End render pass
            SDL_EndGPURenderPass(render_pass);
        }

        // [8] Submit command buffer
        SDL_SubmitGPUCommandBuffer(ctxgpu.command_buffer);
    }
    gpu::destroy_window(ctxgpu);

    return 0;
}