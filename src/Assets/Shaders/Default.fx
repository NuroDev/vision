#include "Helper.fx"

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
Texture2D Texture : register(t0);
SamplerState PointSampler : register(s0);

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;
	
	output.Position = mul(input.Position, World);
	output.WorldPosition = output.Position;
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);

	// Multiply the normal by the world transform (to go from model space to world space)
	output.Normal = mul(float4(input.Normal, 1), World).xyz;
	
	// Pass texture straight over
	output.Tex = input.Tex;
	
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_TARGET
{
	float4 textureColor = { 1, 1, 1, 1 };

	// Normalize input normals
    input.Normal = normalize(input.Normal);
	
	if (Material.UseTexture)
		textureColor = Texture.Sample(PointSampler, input.Tex);

    LightingResult light = ComputeLighting(input.WorldPosition, input.Normal);
	
    float4 emissive = Material.Emissive;
    float4 ambient = Material.Ambient * GlobalAmbient;
    float4 diffuse = Material.Diffuse * light.Diffuse;
    float4 specular = Material.Specular * light.Specular;
	
	return (emissive + ambient + diffuse + specular) * textureColor;
}
