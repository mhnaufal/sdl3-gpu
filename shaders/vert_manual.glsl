#version 460

#pragma shader_stage(vertex)

/*
! set=1 gotten from https://wiki.libsdl.org/SDL3/SDL_CreateGPUShader
*/
layout(set=1, binding=0) uniform UBO {
    mat4 mvp; // model-view-projection
};
layout(location=0) in vec3 in_position;

void main() {
    gl_Position = mvp * vec4(in_position, 1);
}