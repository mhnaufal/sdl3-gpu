/*
! JANGAN PERNAH PAKAI VECTOR
*/

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

    /* Using Vertex Buffer
    * [1] Describe Vertex Attributes & Vertex Buffer in pipeline
    * [2] Create Vertex Data
    * [3] Create Vertex Buffer
    * [4] Upload Vertex Data to the Vertex Buffer
    *   - create transfer buffer
    *   - map transfer buffer mem and copy from cpu
    *   - begin copy pass
    *   - invoke upload commands
    *   - end copy pass and submit
    * [5] Bind Vertex Buffer to draw call
    ? https://vulkan-tutorial.com/Vertex_buffers/Vertex_buffer_creation
    */
    context::Vec3Buffer vertices[3] = {
        context::Vec3Buffer{{-0.5f, -0.5f, 0.0f}},
        context::Vec3Buffer{{0.0f, 0.5f, 0.0f}},
        context::Vec3Buffer{{0.5f, -0.5f, 0.0f}}};
    auto vertices_byte_size = LENGTH(vertices) * sizeof(vertices[0]);

    auto vertex_buffer_create_info = SDL_GPUBufferCreateInfo{};
    vertex_buffer_create_info.size = (Uint32)vertices_byte_size;
    vertex_buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    auto vertex_buffer = SDL_CreateGPUBuffer(ctxgpu.gpu_device, &vertex_buffer_create_info);

    auto transfer_buffer_create_info = SDL_GPUTransferBufferCreateInfo{};
    transfer_buffer_create_info.usage = SDL_GPUTransferBufferUsage::SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transfer_buffer_create_info.size = (Uint32)vertices_byte_size;
    auto transfer_buffer = SDL_CreateGPUTransferBuffer(ctxgpu.gpu_device, &transfer_buffer_create_info);

    auto transfer_memory = SDL_MapGPUTransferBuffer(ctxgpu.gpu_device, transfer_buffer, false);
    memcpy(transfer_memory, &vertices, vertices_byte_size);
    SDL_UnmapGPUTransferBuffer(ctxgpu.gpu_device, transfer_buffer);

    auto copy_cmd_buffer = SDL_AcquireGPUCommandBuffer(ctxgpu.gpu_device);
    auto copy_pass = SDL_BeginGPUCopyPass(copy_cmd_buffer);
        auto buffer_location = SDL_GPUTransferBufferLocation{};
        buffer_location.transfer_buffer = transfer_buffer;
        auto buffer_region = SDL_GPUBufferRegion{};
        buffer_region.buffer = vertex_buffer;
        buffer_region.size = (Uint32)vertices_byte_size;
        // buffer_region.offset = 0;
            SDL_UploadToGPUBuffer(copy_pass, &buffer_location, &buffer_region, false);
    SDL_EndGPUCopyPass(copy_pass);
    auto ok2 = SDL_SubmitGPUCommandBuffer(copy_cmd_buffer);
    SDL_ReleaseGPUTransferBuffer(ctxgpu.gpu_device, transfer_buffer);

    /* 
    ! because UBO exists inside vertex_glsl, but not fragment_glsl
    */
    gpu::create_gpu_shader(ctxgpu, std::string{"../../../../shaders/shader.spv.vert"}, context::ShaderType::VERTEX, 1);
    gpu::create_gpu_shader(ctxgpu, std::string{"../../../../shaders/shader.spv.frag"}, context::ShaderType::FRAGMENT, 0);

    gpu::create_graphic_pipeline(ctxgpu);
    SDL_ReleaseGPUShader(ctxgpu.gpu_device, ctxgpu.vertex_shader);
    SDL_ReleaseGPUShader(ctxgpu.gpu_device, ctxgpu.fragment_shader);

    int w{}, h{};
    SDL_GetWindowSize(ctxgpu.window, &w, &h);
    float fov = glm::radians(70.0f);
    float near_plane = 0.0001f;
    float far_plane = 1000.0f;
    glm::mat4 projection_mat = glm::perspective(fov, (float)w/h, near_plane, far_plane);

    ctxgpu.rotation_rad = glm::radians(-45.0f);
    ctxgpu.last_tick = SDL_GetTicks();

    while (ctxgpu.is_playing && ok && ok2) {
        ctxgpu.new_tick = SDL_GetTicks();
        float delta_tick = (float)(ctxgpu.new_tick - ctxgpu.last_tick) / 1000.0f;
        ctxgpu.last_tick = ctxgpu.new_tick;

        // [1] Process Events
        gpu::process_input(ctxgpu);

        // [2] Update game state

        // [3] Acquire command buffer
        // [4] Acquire swapchain texture: texture that will be rendered
        gpu::init_command_buffer(ctxgpu);

        ctxgpu.rotation_rad += glm::radians(10.0f) * delta_tick;
        glm::mat4 rotation_mat = glm::rotate(projection_mat, ctxgpu.rotation_rad, glm::vec3(0, 1, 0));
        auto ubo = context::UniformBufferObject{};
        ubo.mvp = glm::rotate((glm::translate(projection_mat, glm::vec3(0,0,-5))), ctxgpu.rotation_rad, glm::vec3(0,0,1));
        /*
        * glm::translate(projection_mat, glm::vec3(0,0,-5)); mundurin triangle ke belakang biar ga nabrak camera
        * glm::rotate((glm::translate(projection_mat, glm::vec3(0,0,-5))), ctxgpu.rotation_rad, glm::vec3(0,0,1)) rotasi searah jarum jam
        */

        if (ctxgpu.gpu_texture) {
            // [5] Begin render pass
            // where we actually doing the rendering, which encoded into Command Buffer
            auto color_info = SDL_GPUColorTargetInfo{};
            color_info.texture = ctxgpu.gpu_texture;
            color_info.clear_color = SDL_FColor{0.1f, 0.2f, 0.45f, 1.0f};
            color_info.load_op = SDL_GPULoadOp::SDL_GPU_LOADOP_CLEAR;
            color_info.store_op = SDL_GPUStoreOp::SDL_GPU_STOREOP_STORE;
            auto render_pass = SDL_BeginGPURenderPass(ctxgpu.command_buffer, &color_info, 1, nullptr);

            // [6] Draw
                // - Bind pipeline
                SDL_BindGPUGraphicsPipeline(render_pass, ctxgpu.graphic_pipeline);
                // - Bind vertex buffer/data & Bind unform buffer/data
                auto gpu_buffer_binding = SDL_GPUBufferBinding{};
                gpu_buffer_binding.buffer = vertex_buffer;
                SDL_BindGPUVertexBuffers(render_pass, 0, &gpu_buffer_binding, 1);

                SDL_PushGPUVertexUniformData(ctxgpu.command_buffer, 0 /*binding*/, &ubo, sizeof(ubo));
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