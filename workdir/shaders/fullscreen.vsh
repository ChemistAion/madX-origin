//#define FullScreenRS "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)"
	
static const float4 FullScreenVertsPos[3] = 
{
	float4(-1, 1, 1, 1),
	float4(3, 1, 1, 1),
	float4(-1, -3, 1, 1)
};

static const float2 FullScreenVertsUVs[3] =
{
	float2(0, 0),
	float2(2, 0),
	float2(0, 2)
};
	
struct VERTEX_OUT
{
    float2 vTexture : TEXCOORD;
    float4 vPosition : SV_POSITION;
};

//[RootSignature(FullScreenRS)]
VERTEX_OUT main(uint vertexId : SV_VertexID)
{
    VERTEX_OUT Output;
	
    Output.vPosition = FullScreenVertsPos[vertexId];
    Output.vTexture = FullScreenVertsUVs[vertexId];
    return Output;
};