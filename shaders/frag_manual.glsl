#version 460

#pragma shader_stage(fragment)

layout(location = 0) in vec4 in_color; //* script ini menerima vec4 color dari vertex shader dan...
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec4 out_color; //* ...menghasilkan vec4 color yang tampil di layar

layout(set = 2, binding = 0) uniform sampler2D texture_sampler;

void main() {
	out_color = texture(texture_sampler, in_uv) * in_color; //* hasil dari texture dan color di kalikan
}