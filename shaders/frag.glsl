#version 450
layout(row_major) uniform;
layout(row_major) buffer;

#line 20 0
layout(location = 0)
out vec4 entryPointParam_fragmentMain_color_0;


#line 9
layout(location = 0)
in vec3 coarseVertex_color_0;


#line 20
struct Fragment_0
{
    vec4 color_0;
};



void main()
{

    Fragment_0 output_0;
    output_0.color_0 = vec4(coarseVertex_color_0, 1.0);

#line 31
    entryPointParam_fragmentMain_color_0 = output_0.color_0;

#line 31
    return;
}

