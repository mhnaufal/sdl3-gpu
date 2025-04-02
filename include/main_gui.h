// NOT A PROJECT TO BE TAKEN SERIOUSLY
// GUI: ImGui (https://github.com/ocornut/imgui)

#pragma once

#include "SDL3/include/SDL3/SDL_gpu.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlgpu3.h"
#include "imgui/imgui_impl_sdlgpu3_shaders.h"

namespace context {
struct DrawData {
    ImVec4 clear_color{};
    ImDrawData* draw_data{};
};

struct ContextGui {
public:
    ContextGui() = delete;
    ContextGui(SDL_Window& window, SDL_GPUDevice& device)
        : m_window(&window)
        , m_gpu_device(&device)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        m_io = ImGui::GetIO();
        m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        m_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForSDLGPU(m_window);
        ImGui_ImplSDLGPU3_InitInfo init_info{};
        init_info.Device = m_gpu_device;
        init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(m_gpu_device, m_window);
        init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
        ImGui_ImplSDLGPU3_Init(&init_info);
    }

    ~ContextGui() = default;
    auto Destroy() -> void
    {
        SDL_WaitForGPUIdle(m_gpu_device);
        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui::DestroyContext();
    }

    auto update_imgui_frame() -> void
    {
        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    auto draw_imgui_gui(DrawData& dd) -> void
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Bubuk ENGINE");

        ImGui::Text("Coba delete draw call di sini!");

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit4("clear color", (float*)&dd.clear_color);

        if (ImGui::Button("Button")) {
            counter++;
        }

        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text(
            "Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / m_io.Framerate,
            m_io.Framerate);
        ImGui::End();

        ImGui::Render();
        dd.draw_data = ImGui::GetDrawData();
    }

    auto do_render_pass_imgui(context::ContextRender& ctxren, DrawData& dd) -> void
    {
        //* call to upload the vertex/index buffer!
        ImGui_ImplSDLGPU3_PrepareDrawData(dd.draw_data, ctxren.command_buffer);

        //* [5] Begin render pass: where we actually doing the rendering, which encoded into Command
        //Buffer
        auto color_info = SDL_GPUColorTargetInfo{};
        color_info.texture = ctxren.gpu_texture;
        color_info.clear_color =
            SDL_FColor{dd.clear_color.x, dd.clear_color.y, dd.clear_color.z, dd.clear_color.w};
        color_info.load_op = SDL_GPULoadOp::SDL_GPU_LOADOP_CLEAR;
        color_info.store_op = SDL_GPUStoreOp::SDL_GPU_STOREOP_STORE;
        color_info.mip_level = 0;
        color_info.layer_or_depth_plane = 0;
        color_info.cycle = false;
        auto render_pass = SDL_BeginGPURenderPass(ctxren.command_buffer, &color_info, 1, nullptr);

        //* [6] ImGui Draw
        ImGui_ImplSDLGPU3_RenderDrawData(dd.draw_data, ctxren.command_buffer, render_pass);

        //* [7] End render pass
        SDL_EndGPURenderPass(render_pass);
    }

private:
    SDL_Window* m_window{};
    SDL_GPUDevice* m_gpu_device{};
    ImGuiIO m_io{};
};

} // namespace context
