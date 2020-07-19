#include "Helper.fx"

//--------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------
Texture2D Texture : register(t0);
SamplerState PointSampler : register(s0);

//--------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------
struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 Color : SV_TARGET_COLOR;
    float4 Normal : SV_TARGET_NORMAL;
};

//--------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;
    
	// Set a base/default 4th unit for the position for proper matrix calculations
    input.Position = float4(input.Position.xyz, 1.0f);

    // Calculate the position of the vertex against the world, view & projection matrices
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    // Pass texture co-ords straight over
    output.Tex = input.Tex;
    
    // Calculate the normal vector against the world matrix, then normalize it
    output.Normal = normalize(mul(input.Normal, (float3x3) World));
	
	return output;
}

//--------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------
PS_OUTPUT PS(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;
	
    output.Color = Texture.Sample(PointSampler, input.Tex);
    output.Normal = float4(input.Normal, 1.0f);
	
    return output;
}
