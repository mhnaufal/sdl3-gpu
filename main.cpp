// NOT A PROJECT TO BE TAKEN SERIOUSLY

#include "include/main_render.h"
#include "include/main_audio.h"
#include "include/main_global.h"
#include "include/main_helper.h"
#include "include/main_gui.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char const* argv[])
{
    printf("####################\n### %s ###\n####################\n\n", PLATFORM_NAME);

    context::ContextAudio ctxaud{};
    context::ContextRender ctxren{};
    context::ContextGlobal ctxglob{};
    ctxglob.window_width = 1280;
    ctxglob.window_height = 720;

    ctxglob.is_playing = context::gpu::create_window(ctxren, ctxglob);
    auto ok = context::gpu::init_gpu_device_sdl(ctxren);

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
            {-0.5f, -0.5f, 0.0f},       // position
            {1.0f, 0.0f, 0.0f, 1.0f}    // color
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
    auto vertex_buffer = SDL_CreateGPUBuffer(ctxren.gpu_device, &vertex_buffer_create_info);

    auto transfer_buffer_create_info = SDL_GPUTransferBufferCreateInfo{};
    transfer_buffer_create_info.usage = SDL_GPUTransferBufferUsage::SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transfer_buffer_create_info.size = (Uint32)vertices_byte_size;
    auto transfer_buffer = SDL_CreateGPUTransferBuffer(ctxren.gpu_device, &transfer_buffer_create_info);

    auto transfer_memory = SDL_MapGPUTransferBuffer(ctxren.gpu_device, transfer_buffer, false);
    memcpy(transfer_memory, &vertices, vertices_byte_size);
    SDL_UnmapGPUTransferBuffer(ctxren.gpu_device, transfer_buffer);

    auto copy_cmd_buffer = SDL_AcquireGPUCommandBuffer(ctxren.gpu_device);
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
    SDL_ReleaseGPUTransferBuffer(ctxren.gpu_device, transfer_buffer);

    /* 
    ! "1" Because UBO exists inside vertex_glsl, but not fragment_glsl
    */
    context::gpu::create_gpu_shader_sdl(ctxren, std::string{"./shaders/shader.spv.vert"}, context::ShaderType::VERTEX, 1);
    context::gpu::create_gpu_shader_sdl(ctxren, std::string{"./shaders/shader.spv.frag"}, context::ShaderType::FRAGMENT, 0);

    context::gpu::create_graphic_pipeline_sdl(ctxren);
    SDL_ReleaseGPUShader(ctxren.gpu_device, ctxren.vertex_shader);
    SDL_ReleaseGPUShader(ctxren.gpu_device, ctxren.fragment_shader);

#ifdef USE_IMGUI
    context::ContextGui ctxgui(*ctxren.window, *ctxren.gpu_device);
    auto dd = context::DrawData{};
    dd.clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
#endif

    //? DRAWING
    int w{}, h{};
    SDL_GetWindowSize(ctxren.window, &w, &h);
    float fov = glm::radians(70.0f);
    float near_plane = 0.0001f;
    float far_plane = 1000.0f;
    glm::mat4 projection_mat = glm::perspective(fov, (float)w/h, near_plane, far_plane);

    ctxren.rotation_rad = glm::radians(-45.0f);
    ctxglob.last_tick = SDL_GetTicks();

    if (ctxglob.is_playing && ok && ok2) {
        ctxaud.PlayMusic("./resource/middleast.mp3");
    }

    while (ctxglob.is_playing && ok && ok2) {
        //? FMOD AUDIO
        ctxaud.UpdateMusic();

        //? GAME TICKS
        ctxglob.new_tick = SDL_GetTicks();
        float delta_tick = (float)(ctxglob.new_tick - ctxglob.last_tick) / 1000.0f;
        ctxglob.last_tick = ctxglob.new_tick;

        //? RENDERING
        //* [1] Process Events
        context::gpu::process_input(ctxren, ctxglob);

#ifdef USE_IMGUI
    ctxgui.update_imgui_frame();
    ctxgui.draw_imgui_gui(dd);
#endif

        //* [2] Update game state
        //* [3] Acquire command buffer
        //* [4] Acquire swapchain texture: texture that will be rendered
        context::gpu::init_command_buffer_sdl(ctxren);

        /*
        ! glm::translate(projection_mat, glm::vec3(0,0,-5)); mundurin triangle ke belakang biar ga nabrak camera
        ! glm::rotate((glm::translate(projection_mat, glm::vec3(0,0,-5))), ctxren.rotation_rad, glm::vec3(0,0,1)) rotasi searah jarum jam
        */
        ctxren.rotation_rad += glm::radians(10.0f) * delta_tick;
        glm::mat4 rotation_mat = glm::rotate(projection_mat, ctxren.rotation_rad, glm::vec3(0, 1, 0));
        auto ubo = context::UniformBufferObject{};
        ubo.mvp = glm::rotate((glm::translate(projection_mat, glm::vec3(0,0,-2))), ctxren.rotation_rad, glm::vec3(0,0,1));

        if (ctxren.gpu_texture) {
#ifdef USE_IMGUI
            ctxgui.do_render_pass_imgui(ctxren, dd);
#else
            context::gpu::do_render_pass_sdl(ctxren, *vertex_buffer, ubo);
#endif
        }

        // [8] Submit command buffer
        SDL_SubmitGPUCommandBuffer(ctxren.command_buffer);
    }

#ifdef USE_IMGUI
    defer(ctxgui.Destroy());
#endif
    defer(context::gpu::destroy_window(ctxren));

    return 0;
}