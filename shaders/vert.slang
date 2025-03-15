// https://github.com/shader-slang/slang/blob/master/examples/triangle/shaders.slang

struct AssembledVertex
{
    float3 position: POSITION;
    float3 color : COLOR;
};

struct CoarseVertex
{
    float3 color;
};

struct Vertex
{
    float4 color;
};

cbuffer Uniforms
{
    float4x4 modelViewProjection;
}

struct VertexStageOutput
{
    CoarseVertex    coarseVertex    : CoarseVertex;
    float4          sv_position     : SV_Position;
};

[shader("vertex")]
[numthreads(1,1,1)]
VertexStageOutput VertexMain(AssembledVertex assembledVertex)
{
    VertexStageOutput output;

    float3 position = assembledVertex.position;
    float3 color    = assembledVertex.color;

    output.coarseVertex.color = color;
    output.sv_position = mul(modelViewProjection, float4(position, 1.0));

    return output;
}