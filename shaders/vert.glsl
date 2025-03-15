#version 450
layout(row_major) uniform;
layout(row_major) buffer;

#line 11475 0
struct _MatrixStorage_float4x4_ColMajorstd140_0
{
    vec4  data_0[4];
};


#line 11475
struct SLANG_ParameterGroup_Uniforms_std140_0
{
    _MatrixStorage_float4x4_ColMajorstd140_0 modelViewProjection_0;
};


#line 19 1
layout(binding = 0)
layout(std140) uniform block_SLANG_ParameterGroup_Uniforms_std140_0
{
    _MatrixStorage_float4x4_ColMajorstd140_0 modelViewProjection_0;
}Uniforms_0;

#line 19
mat4x4 unpackStorage_0(_MatrixStorage_float4x4_ColMajorstd140_0 _S1)
{

#line 19
    return mat4x4(_S1.data_0[0][0], _S1.data_0[1][0], _S1.data_0[2][0], _S1.data_0[3][0], _S1.data_0[0][1], _S1.data_0[1][1], _S1.data_0[2][1], _S1.data_0[3][1], _S1.data_0[0][2], _S1.data_0[1][2], _S1.data_0[2][2], _S1.data_0[3][2], _S1.data_0[0][3], _S1.data_0[1][3], _S1.data_0[2][3], _S1.data_0[3][3]);
}


#line 3
layout(location = 0)
out vec3 entryPointParam_VertexMain_coarseVertex_color_0;


#line 3
layout(location = 0)
in vec3 assembledVertex_position_0;


#line 3
layout(location = 1)
in vec3 assembledVertex_color_0;




struct CoarseVertex_0
{
    vec3 color_0;
};


#line 24
struct VertexStageOutput_0
{
    CoarseVertex_0 coarseVertex_0;
    vec4 sv_position_0;
};



void main()
{
    VertexStageOutput_0 output_0;

#line 39
    output_0.coarseVertex_0.color_0 = assembledVertex_color_0;
    output_0.sv_position_0 = (((vec4(assembledVertex_position_0, 1.0)) * (unpackStorage_0(Uniforms_0.modelViewProjection_0))));

    VertexStageOutput_0 _S2 = output_0;

#line 42
    entryPointParam_VertexMain_coarseVertex_color_0 = output_0.coarseVertex_0.color_0;

#line 42
    gl_Position = _S2.sv_position_0;

#line 42
    return;
}

