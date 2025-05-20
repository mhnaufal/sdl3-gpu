#version 460

#pragma shader_stage(vertex)

/*
! set=1 gotten from https://wiki.libsdl.org/SDL3/SDL_CreateGPUShader
*/
layout(set = 1, binding = 0) uniform UBO {
    mat4 mvp; // model-view-projection
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color; //* script ini menerima input color dari C++ code/user yang kemudian...
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec4 out_color; //* ...akan di proses dan hasilnya masuk ke out_color dan dikirim ke fragment shader
layout(location = 1) out vec2 out_uv;

void main() {
    gl_Position = mvp * vec4(in_position, 1);
    out_color = in_color;
    out_uv = in_uv;
}