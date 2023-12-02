#define GBufferRS \
	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)," \
	"CBV(b0, flags = DATA_STATIC, visibility = SHADER_VISIBILITY_VERTEX)" \
	
cbuffer Constants
{
    float4x4 world_;
    float4x4 inv_world_;
    float4x4 view_projection_;
};

struct VSInput
{
    float4 position_ : POSITION;
	float4 tex_ : TEXCOORD0;
    float3 normal_ : NORMAL;
    float4 tangent_ : TANGENT;
};

struct VSOutput 
{ 
    float4 position_ : SV_POSITION; 
	float4 tex_ : TEXCOORD0;	
    float3 normal_ : TEXCOORD1;
	float3 tangent_ : TEXCOORD2;
	float3 bitangent_ : TEXCOORD3;
};

[RootSignature(GBufferRS)]
void main(in VSInput VSIn, out VSOutput VSOut)
{
    VSOut.position_ = mul(view_projection_, mul(world_, VSIn.position_));
	
    VSOut.normal_ = normalize(mul(float4(VSIn.normal_, 0.0), inv_world_).xyz);
	VSOut.tangent_ = normalize(mul(world_, float4(VSIn.tangent_.xyz, 0.0)).xyz);
	VSOut.bitangent_ = normalize(cross(VSOut.normal_, VSOut.tangent_));

	VSOut.tex_ = VSIn.tex_;
}
