// NOT A PROJECT TO BE TAKEN SERIOUSLY

#include "include/main_global.h"

#ifdef ANDROID
#elif defined(WINDOWS)
#    include "include/SDL3/include/SDL3/SDL_gpu.h"
#    include "include/main_audio.h"
#    include "include/main_gui.h"
#    include "include/main_helper.h"
#    include "include/main_physic.h"
#    include "include/main_render.h"
#endif

#ifdef ANDROID
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static SDL_FRect mouseposrect;

SDL_AppResult SDL_AppIterate(void *appstate)
{
    Uint8 r;
    /* fade between shades of red every 3 seconds, from 0 to 255. */
    r = (Uint8) ((((float) (SDL_GetTicks() % 3000)) / 3000.0f) * 255.0f);
    SDL_SetRenderDrawColor(renderer, r, 0, 0, 255);
    /* you have to draw the whole window every frame. Clearing it makes sure the whole thing is sane. */
    SDL_RenderClear(renderer);  /* clear whole window to that fade color. */
    /* set the color to white */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    /* draw a square where the mouse cursor currently is. */
    SDL_RenderFillRect(renderer, &mouseposrect);
    /* put everything we drew to the screen. */
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type) {
        case SDL_EVENT_QUIT:  /* triggers on last window close and other things. End the program. */
            return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("SDL Hello World Example", "1.0", "com.example.sdl-hello-world");
        if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Hello SDL", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("SDL_CreateWindowAndRenderer() failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
#elif WINDOWS
int main([[maybe_unused]] int argc, [[maybe_unused]] char const* argv[])
{
    fprintf(stdout, "####################\n### %s ###\n####################\n\n", PLATFORM_NAME);

    SDL_SetLogPriority(SDL_LOG_CATEGORY_VIDEO, SDL_LOG_PRIORITY_DEBUG);
    SDL_SetLogPriority(SDL_LOG_CATEGORY_GPU, SDL_LOG_PRIORITY_DEBUG);
    // SDL_SetHint(SDL_HINT_VIDEO_FORCE_EGL, "0");

    context::ContextAudio ctxaud{};
    context::ContextPhysic ctxphy{};
    context::ContextRender ctxren{};
    context::ContextGlobal ctxglob{};
    ctxglob.window_width = 1280;
    ctxglob.window_height = 720;

    ctxglob.is_playing = context::gpu::create_window(ctxren, ctxglob);

    /* 
    ? Vertex Buffer: 
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
    context::Vec3Buffer vertices[4] = {
        {
            {-0.5f, -0.5f, 0.0f},       // position
            {1.0f, 0.0f, 0.0f, 1.0f}    // color
        },
        {
            {0.5f, -0.5f, 0.0f},
            {0.0f, 1.0f, 0.0f, 1.0f}
        },
        {
            {-0.5f, 0.5f, 0.0f},
            {0.0f, 0.0f, 1.0f, 1.0f}
        },
        {
            {0.5f, 0.5f, 0.0f},
            {1.0f, 1.0f, 1.0f, 0.8f}
        }
    };
    auto vertices_byte_size = LENGTH(vertices) * sizeof(vertices[0]);

    auto vertex_buffer_create_info = SDL_GPUBufferCreateInfo{};
    vertex_buffer_create_info.size = (Uint32)vertices_byte_size;
    vertex_buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    auto vertex_buffer = SDL_CreateGPUBuffer(ctxren.gpu_device, &vertex_buffer_create_info);

    /*
    ? Index Buffer
    * instead of copy-pasting vertex buffer multiple times, we can use index buffer
    */
    Uint32 indices[] = {
        0, 1, 2,
        1, 2, 3
    };
    auto indices_byte_size = LENGTH(indices) * sizeof(indices[0]);

    auto index_buffer_create_info = SDL_GPUBufferCreateInfo{};
    index_buffer_create_info.size = (Uint32)indices_byte_size;
    index_buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    auto index_buffer = SDL_CreateGPUBuffer(ctxren.gpu_device, &index_buffer_create_info);

    auto transfer_buffer_create_info = SDL_GPUTransferBufferCreateInfo{};
    transfer_buffer_create_info.usage = SDL_GPUTransferBufferUsage::SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transfer_buffer_create_info.size = (Uint32)((Uint32)vertices_byte_size + (Uint32)indices_byte_size);
    auto transfer_buffer = SDL_CreateGPUTransferBuffer(ctxren.gpu_device, &transfer_buffer_create_info);

    uint8_t* transfer_memory = reinterpret_cast<uint8_t*>(SDL_MapGPUTransferBuffer(ctxren.gpu_device, transfer_buffer, false));
    memcpy(transfer_memory, &vertices, vertices_byte_size);
    memcpy(transfer_memory + vertices_byte_size, &indices, indices_byte_size);
    SDL_UnmapGPUTransferBuffer(ctxren.gpu_device, transfer_buffer);

    auto copy_cmd_buffer = SDL_AcquireGPUCommandBuffer(ctxren.gpu_device);
    auto copy_pass =
    SDL_BeginGPUCopyPass(copy_cmd_buffer);
        auto vertex_buffer_location = SDL_GPUTransferBufferLocation{};
        vertex_buffer_location.transfer_buffer = transfer_buffer;
        vertex_buffer_location.offset = 0;
        auto vertex_buffer_region = SDL_GPUBufferRegion{};
        vertex_buffer_region.buffer = vertex_buffer;                   //? use Vertex Buffer here
        vertex_buffer_region.offset = 0;
        vertex_buffer_region.size = (Uint32)vertices_byte_size;
        SDL_UploadToGPUBuffer(copy_pass, &vertex_buffer_location, &vertex_buffer_region, false);
        /********************************************/
        auto index_buffer_location = SDL_GPUTransferBufferLocation{};
        index_buffer_location.transfer_buffer = transfer_buffer;
        index_buffer_location.offset = (Uint32)vertices_byte_size;    //* since we're using Index Buffer, don't forget to add the offset based on the transfer buffer that we set.
        auto index_buffer_region = SDL_GPUBufferRegion{};
        index_buffer_region.buffer = index_buffer;                    //? use Index Buffer here
        index_buffer_region.offset = 0;                               //! destination offset
        index_buffer_region.size = (Uint32)indices_byte_size;
        SDL_UploadToGPUBuffer(copy_pass, &index_buffer_location, &index_buffer_region, false);
    SDL_EndGPUCopyPass(copy_pass);

    auto is_cmd_buffer_ready = SDL_SubmitGPUCommandBuffer(copy_cmd_buffer);
    SDL_ReleaseGPUTransferBuffer(ctxren.gpu_device, transfer_buffer);

    /*
    ? Texture Sampling
    * [1] Load texture pixels from file
    * [2] Create texture on the GPU
    * [3] Upload texture pixels to the GPU
    * [4] Assign texture coordinates to vertices
    * [5] Create sampler for the shaders
    * [6] Make sample colors from texture
    */
    {
        int width, height, channel;
        unsigned char* image_pixels = stbi_load("./resource/nvidia-logo.png", &width, &height, &channel, 0);
        printf("Image size: %d x %d x %d\n", width, height, channel);
        defer(stbi_image_free(image_pixels));

        auto gpu_sampler_create_info = SDL_GPUSamplerCreateInfo{};

        ctxren.sampler = SDL_CreateGPUSampler(ctxren.gpu_device, &gpu_sampler_create_info);
        SDL_CHECK_ERROR(ctxren, "Failed to GPU Sampler", false);
    }

    context::gpu::create_gpu_shader_sdl(
        ctxren, 
        std::string{"./shaders/shader.spv.vert"}, 
        context::ShaderType::VERTEX, 
        Uint32{1}                                                    // ! "1" Because UBO exists inside vertex_glsl, but not fragment_glsl
    );
    context::gpu::create_gpu_shader_sdl(
        ctxren, 
        std::string{"./shaders/shader.spv.frag"},
        context::ShaderType::FRAGMENT, 
        0
    );

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

    if (ctxglob.is_playing && is_cmd_buffer_ready) {
        ctxaud.PlayMusic("./resource/middleast.mp3");
    }

    while (ctxglob.is_playing && is_cmd_buffer_ready) {
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
            context::gpu::do_render_pass_sdl(ctxren, *vertex_buffer, ubo, *index_buffer);
#endif
        }

        //* [8] Submit command buffer
        SDL_SubmitGPUCommandBuffer(ctxren.command_buffer);
    }

#ifdef USE_IMGUI
    defer(ctxgui.Destroy());
#endif
    defer(context::gpu::destroy_window(ctxren));

    return 0;
}
#endif