// NOT A PROJECT TO BE TAKEN SERIOUSLY
// Render: SDL3 (https://www.libsdl.org/)
// The Forge (https://github.com/ConfettiFX/The-Forge/wiki/)

#pragma once

// bubuk
#include "main_helper.h"
#include "main_global.h"

// SDL
#include "SDL3/include/SDL3/SDL.h"
#include "SDL3/include/SDL3/SDL_error.h"
#include "SDL3/include/SDL3/SDL_events.h"
#include "SDL3/include/SDL3/SDL_gpu.h"
#include "SDL3/include/SDL3/SDL_pixels.h"
#include "SDL3/include/SDL3/SDL_render.h"
#include "SDL3/include/SDL3/SDL_time.h"
#include "SDL3/include/SDL3/SDL_timer.h"
#include "SDL3/include/SDL3/SDL_video.h"

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

    const char* error{};
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
};

struct ContextRenderForge {};

namespace gpu {
auto create_window(context::ContextRender& ctxren, context::ContextGlobal& ctxglob) -> bool;
auto destroy_window(context::ContextRender& ctx) -> void;
auto process_input(context::ContextRender& ctxren, context::ContextGlobal& ctxglob) -> void;
auto init_gpu_device(context::ContextRender& ctx) -> bool;
auto init_command_buffer(context::ContextRender& ctx) -> bool;
auto read_shader_file(const char* path) -> std::vector<Uint8>;
auto create_gpu_shader(
    context::ContextRender& ctx,
    const std::string& path,
    context::ShaderType shader_type,
    Uint32 num_uniform_buffer) -> void;
auto create_graphic_pipeline(context::ContextRender& ctx) -> void;

/****************************************/
// Window
/****************************************/
auto create_window(context::ContextRender& ctxren, context::ContextGlobal& ctxglob) -> bool
{
    if (SDL_Init(SDL_INIT_VIDEO) != true) {
        fprintf(stderr, "Failed to initialize SDL");
        return false;
    }

    ctxren.window = SDL_CreateWindow(
        "SDL 3 GPU Vulkan",
        ctxglob.window_width,
        ctxglob.window_height,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
    SDL_CHECK_ERROR(ctxren, "Failed to create Window", false);

    ctxren.renderer = SDL_CreateRenderer(ctxren.window, nullptr);
    SDL_CHECK_ERROR(ctxren, "Failed to create Renderer", false);

    ctxren.frame_buffer_texture = SDL_CreateTexture(
        ctxren.renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        ctxglob.window_width,
        ctxglob.window_height);
    SDL_CHECK_ERROR(ctxren, "Failed to create Frame Buffer Texture", false);

    return true;
}

inline auto destroy_window(context::ContextRender& ctx) -> void
{
    SDL_DestroyTexture(ctx.frame_buffer_texture);
    SDL_DestroyRenderer(ctx.renderer);
    SDL_DestroyWindow(ctx.window);
    SDL_Quit();
}

inline auto process_input(context::ContextRender& ctxren, context::ContextGlobal& ctxglob) -> void
{
    while (SDL_PollEvent(&ctxren.event)) {
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

inline auto init_gpu_device(context::ContextRender& ctx) -> bool
{
    ctx.gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    auto ok = SDL_ClaimWindowForGPUDevice(ctx.gpu_device, ctx.window);

    return ok;
}

inline auto init_command_buffer(context::ContextRender& ctx) -> bool
{
    ctx.command_buffer = SDL_AcquireGPUCommandBuffer(ctx.gpu_device);
    auto ok = SDL_WaitAndAcquireGPUSwapchainTexture(
        ctx.command_buffer, ctx.window, &ctx.gpu_texture, nullptr, nullptr);

    return ok;
}

inline auto read_shader_file(const char* path) -> std::vector<Uint8>
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<Uint8> code(size);

    file.read(reinterpret_cast<char*>(code.data()), size);
    return code;
}

inline auto gpu::create_gpu_shader(
    context::ContextRender& ctx,
    const std::string& path,
    context::ShaderType shader_type,
    Uint32 num_uniform_buffer) -> void
{
    auto shader_code = gpu::read_shader_file(path.c_str());
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

    auto shader = SDL_CreateGPUShader(ctx.gpu_device, &shader_create_info);

    if (shader_type == context::ShaderType::VERTEX) {
        ctx.vertex_shader = shader;
    }
    else if (shader_type == context::ShaderType::FRAGMENT) {
        ctx.fragment_shader = shader;
    }
}

inline auto gpu::create_graphic_pipeline(context::ContextRender& ctx) -> void
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

    SDL_GPUVertexAttribute vas[] = {vertex_attribute_pos, vertex_attribute_color};

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
        (Uint32)LENGTH(vas); /* banyaknya vertex attribute(vas) yg ada hanya 2 */
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

} // namespace gpu

} // namespace context
