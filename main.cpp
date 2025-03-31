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
    context::ContextAudio ctxaud{};
    context::ContextGraphic ctxgpc{};
    ctxgpc.window_width = 1280;
    ctxgpc.window_height = 720;

    ctxgpc.is_playing = gpu::create_window(ctxgpc);
    auto ok = gpu::init_gpu_device(ctxgpc);

    /* Using Vertex Buffer: 
    ? Dengan ini kita bisa spesify data vertices yg ingin kita gambar lewat C++ instead of dari shader.
    ? Kita tinggal ngasih tau bagaimana struktur data yg kita kirim ke GPU
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
        {
            {-0.5f, -0.5f, 0.0f},
            {1.0f, 0.0f, 0.0f, 1.0f}
        },
        {
            {0.0f, 0.5f, 0.0f},
            {0.5f, 1.0f, 0.0f, 1.0f}
        },
        {
            {0.5f, -0.5f, 0.0f},
            {0.2f, 0.7f, 0.3f, 1.0f}
        }
    };
    auto vertices_byte_size = LENGTH(vertices) * sizeof(vertices[0]);

    auto vertex_buffer_create_info = SDL_GPUBufferCreateInfo{};
    vertex_buffer_create_info.size = (Uint32)vertices_byte_size;
    vertex_buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    auto vertex_buffer = SDL_CreateGPUBuffer(ctxgpc.gpu_device, &vertex_buffer_create_info);

    auto transfer_buffer_create_info = SDL_GPUTransferBufferCreateInfo{};
    transfer_buffer_create_info.usage = SDL_GPUTransferBufferUsage::SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transfer_buffer_create_info.size = (Uint32)vertices_byte_size;
    auto transfer_buffer = SDL_CreateGPUTransferBuffer(ctxgpc.gpu_device, &transfer_buffer_create_info);

    auto transfer_memory = SDL_MapGPUTransferBuffer(ctxgpc.gpu_device, transfer_buffer, false);
    memcpy(transfer_memory, &vertices, vertices_byte_size);
    SDL_UnmapGPUTransferBuffer(ctxgpc.gpu_device, transfer_buffer);

    auto copy_cmd_buffer = SDL_AcquireGPUCommandBuffer(ctxgpc.gpu_device);
    auto copy_pass =
    SDL_BeginGPUCopyPass(copy_cmd_buffer);
        auto buffer_location = SDL_GPUTransferBufferLocation{};
        buffer_location.transfer_buffer = transfer_buffer;
        auto buffer_region = SDL_GPUBufferRegion{};
        buffer_region.buffer = vertex_buffer;
        buffer_region.size = (Uint32)vertices_byte_size;
        buffer_region.offset = 0;
            SDL_UploadToGPUBuffer(copy_pass, &buffer_location, &buffer_region, false);
    SDL_EndGPUCopyPass(copy_pass);

    auto ok2 = SDL_SubmitGPUCommandBuffer(copy_cmd_buffer);
    SDL_ReleaseGPUTransferBuffer(ctxgpc.gpu_device, transfer_buffer);

    /* 
    ! "1" Because UBO exists inside vertex_glsl, but not fragment_glsl
    */
    gpu::create_gpu_shader(ctxgpc, std::string{"../../../../shaders/shader.spv.vert"}, context::ShaderType::VERTEX, 1);
    gpu::create_gpu_shader(ctxgpc, std::string{"../../../../shaders/shader.spv.frag"}, context::ShaderType::FRAGMENT, 0);

    gpu::create_graphic_pipeline(ctxgpc);
    SDL_ReleaseGPUShader(ctxgpc.gpu_device, ctxgpc.vertex_shader);
    SDL_ReleaseGPUShader(ctxgpc.gpu_device, ctxgpc.fragment_shader);

    int w{}, h{};
    SDL_GetWindowSize(ctxgpc.window, &w, &h);
    float fov = glm::radians(70.0f);
    float near_plane = 0.0001f;
    float far_plane = 1000.0f;
    glm::mat4 projection_mat = glm::perspective(fov, (float)w/h, near_plane, far_plane);

    ctxgpc.rotation_rad = glm::radians(-45.0f);
    ctxgpc.last_tick = SDL_GetTicks();

    if (ctxgpc.is_playing && ok && ok2) {
        ctxaud.PlayMusic("../resource/middleast.mp3");
    }

    while (ctxgpc.is_playing && ok && ok2) {
        //? FMOD AUDIO
        ctxaud.UpdateMusic();

        //? GAME TICKS
        ctxgpc.new_tick = SDL_GetTicks();
        float delta_tick = (float)(ctxgpc.new_tick - ctxgpc.last_tick) / 1000.0f;
        ctxgpc.last_tick = ctxgpc.new_tick;

        //? RENDERING
        // [1] Process Events
        gpu::process_input(ctxgpc);

        // [2] Update game state
        // [3] Acquire command buffer
        // [4] Acquire swapchain texture: texture that will be rendered
        gpu::init_command_buffer(ctxgpc);

        ctxgpc.rotation_rad += glm::radians(10.0f) * delta_tick;
        glm::mat4 rotation_mat = glm::rotate(projection_mat, ctxgpc.rotation_rad, glm::vec3(0, 1, 0));
        auto ubo = context::UniformBufferObject{};
        ubo.mvp = glm::rotate((glm::translate(projection_mat, glm::vec3(0,0,-2))), ctxgpc.rotation_rad, glm::vec3(0,0,1));
        /*
        ! glm::translate(projection_mat, glm::vec3(0,0,-5)); mundurin triangle ke belakang biar ga nabrak camera
        ! glm::rotate((glm::translate(projection_mat, glm::vec3(0,0,-5))), ctxgpc.rotation_rad, glm::vec3(0,0,1)) rotasi searah jarum jam
        */

        if (ctxgpc.gpu_texture) {
            // [5] Begin render pass
            // where we actually doing the rendering, which encoded into Command Buffer
            auto color_info = SDL_GPUColorTargetInfo{};
            color_info.texture = ctxgpc.gpu_texture;
            color_info.clear_color = SDL_FColor{0.05f, 0.1f, 0.35f, 1.0f};
            color_info.load_op = SDL_GPULoadOp::SDL_GPU_LOADOP_CLEAR;
            color_info.store_op = SDL_GPUStoreOp::SDL_GPU_STOREOP_STORE;
            auto render_pass = SDL_BeginGPURenderPass(ctxgpc.command_buffer, &color_info, 1, nullptr);

            // [6] Draw
                // - Bind pipeline
                SDL_BindGPUGraphicsPipeline(render_pass, ctxgpc.graphic_pipeline);
                // - Bind vertex buffer/data & Bind unform buffer/data
                auto gpu_buffer_binding = SDL_GPUBufferBinding{};
                gpu_buffer_binding.buffer = vertex_buffer;
                SDL_BindGPUVertexBuffers(render_pass, 0, &gpu_buffer_binding, 1);

                SDL_PushGPUVertexUniformData(ctxgpc.command_buffer, 0 /*binding*/, &ubo, sizeof(ubo));
                // - draw calls
                SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);

            // [7] End render pass
            SDL_EndGPURenderPass(render_pass);
        }

        // [8] Submit command buffer
        SDL_SubmitGPUCommandBuffer(ctxgpc.command_buffer);
    }
    gpu::destroy_window(ctxgpc);

    return 0;
}