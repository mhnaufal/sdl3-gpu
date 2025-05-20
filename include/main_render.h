// NOT A PROJECT TO BE TAKEN SERIOUSLY
// Render: SDL3 (https://www.libsdl.org/)
// The Forge (https://github.com/ConfettiFX/The-Forge/wiki/)

#pragma once
#define VK_USE_PLATFORM_WIN32_KHR

// bubuk
#include "imgui/imgui_impl_sdl3.h"
#include "main_helper.h"
#include "main_global.h"

// SDL
#include "SDL3/include/SDL3/SDL.h"
#include "SDL3/include/SDL3/SDL_events.h"
#include "SDL3/include/SDL3/SDL_gpu.h"
#include "SDL3/include/SDL3/SDL_pixels.h"
#include "SDL3/include/SDL3/SDL_render.h"
#include "SDL3/include/SDL3/SDL_time.h"
#include "SDL3/include/SDL3/SDL_timer.h"
#include "SDL3/include/SDL3/SDL_video.h"
#include "SDL3/include/SDL3/SDL_log.h"
#include "SDL3/include/SDL3/SDL_hints.h"

// NVRHI
#ifndef ANDROID
#include "nvrhi/nvrhi.h"
#endif

// STD
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

/****************************************/
// Render Context
/****************************************/
namespace context {

enum class ShaderType { VERTEX, FRAGMENT };

struct ContextRender {
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* frame_buffer_texture{};
    SDL_Event event{};
    SDL_GPUDevice* gpu_device{};
    SDL_GPUCommandBuffer* command_buffer{};
    SDL_GPUTexture* gpu_texture{};
    SDL_GPUShader* vertex_shader{};
    SDL_GPUShader* fragment_shader{};
    SDL_GPUGraphicsPipeline* graphic_pipeline{};
    SDL_GPUTexture* gpu_sampler_texture{};
    SDL_GPUSampler* sampler{};

    float rotation_rad{};
};

/*
 * Based on definition inside vert_manual.glsl
 */
struct UniformBufferObject {
    glm::mat4x4 mvp{};
};

struct Vec3Buffer {
    glm::vec3 position{};
    SDL_FColor color{};
    glm::vec2 uv{};
};

struct ContextRenderForge {};

namespace gpu {

auto create_window(context::ContextRender& ctxren, context::ContextGlobal& ctxglob) -> bool;
auto destroy_window(context::ContextRender& ctx) -> void;
auto process_input(context::ContextRender& ctxren, context::ContextGlobal& ctxglob) -> void;
auto init_gpu_device_sdl(context::ContextRender& ctx) -> bool;
auto init_command_buffer_sdl(context::ContextRender& ctx) -> bool;
auto create_gpu_shader_sdl(
    context::ContextRender& ctx,
    const std::string& path,
    context::ShaderType shader_type,
    Uint32 num_uniform_buffer,
    Uint32 num_sampler) -> void;
auto create_graphic_pipeline_sdl(context::ContextRender& ctx) -> void;
auto do_render_pass_sdl(
    context::ContextRender& ctxren,
    SDL_GPUBuffer& vertex_buffer,
    context::UniformBufferObject& ubo,
    SDL_GPUBuffer& index_buffer) -> void;

/****************************************/
// Window & Renderer
/****************************************/
auto create_window(context::ContextRender& ctxren, context::ContextGlobal& ctxglob) -> bool
{
    SDL_SetHint(SDL_HINT_GPU_DRIVER, "vulkan");

    if (SDL_Init(SDL_INIT_VIDEO) != true) {
        fprintf(stderr, "Failed to initialize SDL");
        return false;
    }

    ctxren.window = SDL_CreateWindow(
        "Bubuk Engine",
        ctxglob.window_width,
        ctxglob.window_height,
        SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    SDL_CHECK_ERROR("Failed to create Window", false);

    //* Latest NVIDIA problem
    //* https://forums.developer.nvidia.com/t/crash-when-creating-vulkan-swapchain-with-driver-545-on-wayland/284574
    //* Need to be called inside HERE!
    auto ok = init_gpu_device_sdl(ctxren);
    SDL_CHECK_ERROR("Failed to initialize GPU device to be used", false);

    ctxren.renderer = SDL_CreateRenderer(ctxren.window, nullptr);
    SDL_CHECK_ERROR("Failed to create Renderer", false);

    ctxren.frame_buffer_texture = SDL_CreateTexture(
        ctxren.renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        ctxglob.window_width,
        ctxglob.window_height);
    SDL_CHECK_ERROR("Failed to create Frame Buffer Texture", false);

    return ok;
}

inline auto destroy_window(context::ContextRender& ctx) -> void
{
    if (ctx.frame_buffer_texture) SDL_DestroyTexture(ctx.frame_buffer_texture);
    if (ctx.renderer) SDL_DestroyRenderer(ctx.renderer);
    if (ctx.gpu_device && ctx.window) SDL_ReleaseWindowFromGPUDevice(ctx.gpu_device, ctx.window);
    // SDL_DestroyGPUDevice(ctx.gpu_device); // TODO: why?
    if (ctx.window) SDL_DestroyWindow(ctx.window);
    SDL_Quit();
}

inline auto process_input(context::ContextRender& ctxren, context::ContextGlobal& ctxglob) -> void
{
    while (SDL_PollEvent(&ctxren.event)) {
#ifdef USE_IMGUI
        ImGui_ImplSDL3_ProcessEvent(&ctxren.event);
#endif

        switch (ctxren.event.type) {
        case SDL_EVENT_QUIT:
            ctxglob.is_playing = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (ctxren.event.key.key == SDLK_ESCAPE) {
                ctxglob.is_playing = false;
                break;
            }
        }
    }
}

/****************************************/
// SDL3
/****************************************/
inline auto init_gpu_device_sdl(context::ContextRender& ctx) -> bool
{
    std::string name{};
    int device_count = SDL_GetNumGPUDrivers();
    for (int i = 0; i < device_count; i++) {
        name = SDL_GetGPUDriver(i);
        printf("[DEBUG] Windows GPU Driver [%d]: %s\n", i, name.c_str());
#ifdef ANDROID
        ANDROID_LOG_DEBUG("Android GPU Driver [%d]: %s\n", i, name.c_str());
#endif
    }

#ifdef ANDROID
    ctx.gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, name.c_str());
#elif defined(WINDOWS)
    ctx.gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, name.c_str());
#endif
    SDL_CHECK_ERROR("Failed to create the GPU device", false);
    auto ok = SDL_ClaimWindowForGPUDevice(ctx.gpu_device, ctx.window);

    return ok;
}


inline auto init_command_buffer_sdl(context::ContextRender& ctx) -> bool
{
    ctx.command_buffer = SDL_AcquireGPUCommandBuffer(ctx.gpu_device);

    auto ok = SDL_WaitAndAcquireGPUSwapchainTexture(
        ctx.command_buffer, ctx.window, &ctx.gpu_texture, nullptr, nullptr);

    return ok;
}

inline auto create_gpu_shader_sdl(
    context::ContextRender& ctx,
    const std::string& path,
    context::ShaderType shader_type,
    Uint32 num_uniform_buffer,
    Uint32 num_sampler
) -> void
{
    auto shader_code = read_shader_file(path.c_str());
    auto shader_create_info = SDL_GPUShaderCreateInfo{};
    shader_create_info.code_size = shader_code.size();
    shader_create_info.code = shader_code.data();
    shader_create_info.entrypoint = "main";
    shader_create_info.format = SDL_GPU_SHADERFORMAT_SPIRV;
    if (shader_type == context::ShaderType::VERTEX) {
        shader_create_info.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    }
    else if (shader_type == context::ShaderType::FRAGMENT) {
        shader_create_info.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    }
    shader_create_info.num_uniform_buffers = num_uniform_buffer;
    shader_create_info.num_samplers = num_sampler;

    auto shader = SDL_CreateGPUShader(ctx.gpu_device, &shader_create_info);

    if (shader_type == context::ShaderType::VERTEX) {
        ctx.vertex_shader = shader;
    }
    else if (shader_type == context::ShaderType::FRAGMENT) {
        ctx.fragment_shader = shader;
    }
}

inline auto create_graphic_pipeline_sdl(context::ContextRender& ctx) -> void
{
    auto color_target_desc = SDL_GPUColorTargetDescription{};
    color_target_desc.format = SDL_GetGPUSwapchainTextureFormat(ctx.gpu_device, ctx.window);

    auto pipeline_target_info = SDL_GPUGraphicsPipelineTargetInfo{};
    pipeline_target_info.num_color_targets = 1;
    pipeline_target_info.color_target_descriptions = &color_target_desc;

    //* Vertex Attribute: definisi struktur dari vertex buffer yg akan dikirim ke GPU
    auto vertex_attribute_pos = SDL_GPUVertexAttribute{};
    vertex_attribute_pos.location = 0; /* didapat dari shader code */
    vertex_attribute_pos.buffer_slot = 0;
    vertex_attribute_pos.offset =
        offsetof(context::Vec3Buffer, position); /* offset data yg bakal dikirim ke GPU */
    vertex_attribute_pos.format = SDL_GPUVertexElementFormat::SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;

    auto vertex_attribute_color = SDL_GPUVertexAttribute{};
    vertex_attribute_color.location = 1;
    vertex_attribute_color.buffer_slot = 0;
    vertex_attribute_color.offset = offsetof(context::Vec3Buffer, color);
    vertex_attribute_color.format = SDL_GPUVertexElementFormat::SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;

    auto vertext_attribute_uv = SDL_GPUVertexAttribute{};
    vertext_attribute_uv.location = 2;
    vertext_attribute_uv.buffer_slot = 0;
    vertext_attribute_uv.offset = offsetof(context::Vec3Buffer, uv);
    vertext_attribute_uv.format = SDL_GPUVertexElementFormat::SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;

    SDL_GPUVertexAttribute vas[] = {vertex_attribute_pos, vertex_attribute_color, vertext_attribute_uv};

    //* Vertex Buffer: struktur vertex buffer yg dipasang ke pipeline
    auto vertex_buffer_description = SDL_GPUVertexBufferDescription{};
    vertex_buffer_description.slot = 0;
    vertex_buffer_description.pitch = sizeof(context::Vec3Buffer);
    vertex_buffer_description.input_rate = SDL_GPUVertexInputRate::SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertex_buffer_description.instance_step_rate = 0;

    auto vertex_input_state = SDL_GPUVertexInputState{};
    vertex_input_state.num_vertex_buffers = 1;
    vertex_input_state.vertex_buffer_descriptions = &vertex_buffer_description;
    vertex_input_state.num_vertex_attributes =
        (Uint32)LENGTH(vas); /* banyaknya vertex attribute(vas) yg ada hanya 3 */
    vertex_input_state.vertex_attributes = vas;

    auto pipeline_create_info = SDL_GPUGraphicsPipelineCreateInfo{};
    pipeline_create_info.vertex_shader = ctx.vertex_shader;
    pipeline_create_info.fragment_shader = ctx.fragment_shader;
    pipeline_create_info.primitive_type = SDL_GPUPrimitiveType::SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipeline_create_info.target_info = pipeline_target_info;
    pipeline_create_info.vertex_input_state = vertex_input_state;

    auto pipeline = SDL_CreateGPUGraphicsPipeline(ctx.gpu_device, &pipeline_create_info);
    ctx.graphic_pipeline = pipeline;
}

inline auto do_render_pass_sdl(context::ContextRender& ctxren, SDL_GPUBuffer& vertex_buffer, context::UniformBufferObject& ubo, SDL_GPUBuffer& index_buffer) -> void
{
    //* [5] Begin render pass: where we actually doing the rendering, which encoded into Command Buffer
    auto color_info = SDL_GPUColorTargetInfo{};
    color_info.texture = ctxren.gpu_texture;
    color_info.clear_color = SDL_FColor{0.05f, 0.1f, 0.35f, 1.0f};
    color_info.load_op = SDL_GPULoadOp::SDL_GPU_LOADOP_CLEAR;
    color_info.store_op = SDL_GPUStoreOp::SDL_GPU_STOREOP_STORE;
    color_info.mip_level = 0;
    color_info.layer_or_depth_plane = 0;
    color_info.cycle = false;
    auto render_pass = SDL_BeginGPURenderPass(ctxren.command_buffer, &color_info, 1, nullptr);

    //* [6] Draw
        // - Bind pipeline
        SDL_BindGPUGraphicsPipeline(render_pass, ctxren.graphic_pipeline);

        //? Vertex Buffer | Bind vertex buffer/data & Bind unform buffer/data
        auto gpu_buffer_binding = SDL_GPUBufferBinding{};
        gpu_buffer_binding.buffer = &vertex_buffer;
        SDL_BindGPUVertexBuffers(render_pass, 0, &gpu_buffer_binding, 1);

        //? Index Buffer | - Bind index buffer/data & Bind unform buffer/data
        gpu_buffer_binding = SDL_GPUBufferBinding{};
        gpu_buffer_binding.buffer = &index_buffer;
        SDL_BindGPUIndexBuffer(render_pass, &gpu_buffer_binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        SDL_PushGPUVertexUniformData(
            ctxren.command_buffer,
            0, // binding Id yg ada di dalam vert_manual.glsl
            &ubo,
            sizeof(ubo));

            auto gpu_texture_sampler_binding = SDL_GPUTextureSamplerBinding{};
            gpu_texture_sampler_binding.texture = ctxren.gpu_sampler_texture;
            gpu_texture_sampler_binding.sampler = ctxren.sampler;
            SDL_BindGPUFragmentSamplers(render_pass, 0, &gpu_texture_sampler_binding, 1);

        // - draw calls
        // SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);          //? use Vertex Buffer here
        SDL_DrawGPUIndexedPrimitives(render_pass, 6, 1, 0, 0, 0);   //? use Index Buffer here

    //* [7] End render pass
    SDL_EndGPURenderPass(render_pass);
}

/****************************************/
// Forge
/****************************************/

} // namespace gpu

} // namespace context
