#version 460

#pragma shader_stage(fragment)

layout(location = 0) in vec4 in_color; //* script ini menerima vec4 color dari vertex shader dan...
layout(location = 0) out vec4 out_color; //* ...menghasilkan vec4 color yang tampil di layar

void main() {
	out_color = in_color;
}