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

struct VertexStageOutput
{
    CoarseVertex coarseVertex : CoarseVertex;
    float4 sv_position : SV_Position;
};

struct Fragment
{
    float4 color;
};

[shader("fragment")]
[numthreads(1,1,1)]
Fragment fragmentMain(CoarseVertex coarseVertex : CoarseVertex) : SV_Target
{
    float3 color = coarseVertex.color;
    Fragment output;
    output.color = float4(color, 1.0);
    return output;
}